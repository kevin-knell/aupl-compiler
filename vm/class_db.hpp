#pragma once
#include <vector>
#include <functional>
#include <iostream>
#include <cstring>
#include <type_traits>
#include <tuple>
#include "value.hpp"

namespace vm {

class Value;
using MethodFunc = std::function<void(Value*&, void* raw_obj)>;
using StaticMethodFunc = std::function<void(Value*&)>;

struct MethodPair {
    std::string name;
    MethodFunc value_call;
    MethodFunc pointer_call;
};

class ClassBind {
public:
    std::string name;
    int id;
    std::vector<MethodPair> static_methods;
    std::vector<MethodPair> methods;
};

template <class Method>
MethodFunc bind_method(Method method);

template <class Method>
MethodFunc bind_method_ptr_args(Method method);

class ClassDB {
public:
    std::vector<ClassBind> classes;

    void register_class(int id, std::string name) {
        ClassBind class_bind;
        class_bind.id = id;
        class_bind.name = name;
        classes.push_back(class_bind);
    }

    template<typename Method>
    void register_method(int class_id, std::string name, Method method) {
        MethodPair pair;
        pair.name = name;
        pair.value_call = bind_method(method);
        pair.pointer_call = bind_method_ptr_args(method);
        classes[class_id].methods.push_back(std::move(pair));
    }
};

// MethodTraits to extract class, return type, argument types and constness
template<typename Method>
struct MethodTraits;

// Non-const method
template<typename T, typename Ret, typename... Args>
struct MethodTraits<Ret (T::*)(Args...)> {
    using ClassType = T;
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr bool is_const = false;
};

// Const method
template<typename T, typename Ret, typename... Args>
struct MethodTraits<Ret (T::*)(Args...) const> {
    using ClassType = T;
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr bool is_const = true;
};

// Non-const method
template<typename Ret, typename... Args>
struct MethodTraits<Ret (*)(Args...)> {
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr bool is_const = false;
};

// Helper: safely create a tuple of pointers for arguments (or empty tuple)
template<typename T>
struct ArgToPtr {
    using type = std::conditional_t<
        std::is_reference_v<T>,
        std::add_pointer_t<std::remove_reference_t<T>>, // T&  => T*
        std::add_pointer_t<T>                           // T   => T*
    >;
};

template<typename Tuple>
struct ArgPtrTupleHelper;

template<typename... Args>
struct ArgPtrTupleHelper<std::tuple<Args...>> {
    using type = std::tuple<typename ArgToPtr<Args>::type...>;
};



// Helper to pop a pointer from stack (Value*& sp)
inline void pop_ptr_from_stack(Value*& sp, void* out, size_t size) {
    sp -= size;
    std::memcpy(out, sp, size);
}

// bind_method_ptr_args: binds method by extracting pointers from stack
template<typename Method>
MethodFunc bind_method_ptr_args(Method method) {
    using Traits = MethodTraits<Method>;
    using T = typename Traits::ClassType;
    using Ret = typename Traits::ReturnType;
    using ArgTuple = typename Traits::ArgTuple;
    //using ObjType = std::conditional_t<Traits::is_const, const T*, T*>;

    using ArgPtrTuple = typename ArgPtrTupleHelper<ArgTuple>::type;

    return [method](Value*& sp, void* raw_obj) {
        ArgPtrTuple arg_ptrs;

        // Unpack pointers for args from stack, right to left
        auto pop_ptr = [&](auto*& out) {
            pop_ptr_from_stack(sp, &out, sizeof(out));
        };

        // Cast to correct type
        using ObjType = std::conditional_t<Traits::is_const, const T*, T*>;
        ObjType obj = reinterpret_cast<ObjType>(raw_obj);

        // Use index sequence to unpack
        std::apply([&](auto*&... unpacked) {
            (pop_ptr(unpacked), ...);
        }, arg_ptrs);

        if constexpr (!std::is_void_v<Ret>) {
            Ret result = std::apply([&](auto*... unpacked) {
                return (obj->*method)(*unpacked...);
            }, arg_ptrs);

            std::memcpy(sp, &result, sizeof(result));
            sp += sizeof(result);
        } else {
            std::apply([&](auto*... unpacked) {
                (obj->*method)(*unpacked...);
            }, arg_ptrs);
        }
    };
}

template<typename T>
struct ArgStorage {
    using type = std::conditional_t<
        std::is_reference_v<T>,
        std::remove_reference_t<T>*,  // use T* for references
        T                             // use value for non-references
    >;
};

template<typename Tuple>
struct ArgStorageTuple;

template<typename... Args>
struct ArgStorageTuple<std::tuple<Args...>> {
    using type = std::tuple<typename ArgStorage<Args>::type...>;
};

// bind_method: binds method by extracting values from stack
template<typename Method, typename ObjType, typename Ret>
struct MethodCaller {
    Method method;
    ObjType obj;

    MethodCaller(Method m, ObjType o) : method(m), obj(o) {}

    template<typename... Args>
    Ret operator()(Args&&... unpacked) {
        return (obj->*method)(
            ([&]() -> decltype(auto) {
                if constexpr (std::is_pointer_v<std::decay_t<Args>>)
                    return *unpacked;
                else
                    return unpacked;
            }())...
        );
    }
};

template<typename Method>
MethodFunc bind_method(Method method) {
    using Traits = MethodTraits<Method>;
    using T = typename Traits::ClassType;
    using Ret = typename Traits::ReturnType;
    using ArgTuple = typename Traits::ArgTuple;
    using ArgStorage = typename ArgStorageTuple<ArgTuple>::type;

    return [method](Value*& sp, void* raw_obj) {
        ArgStorage arg_storage;

        auto pop_any = [&](auto& slot) {
            pop_ptr_from_stack(sp, &slot, sizeof(slot));
        };

        std::apply([&](auto&... unpacked) {
            (pop_any(unpacked), ...);
        }, arg_storage);

        using ObjType = std::conditional_t<Traits::is_const, const T*, T*>;
        ObjType obj = reinterpret_cast<ObjType>(raw_obj);

        if constexpr (!std::is_void_v<Ret>) {
            MethodCaller<Method, ObjType, Ret> caller(method, obj);
            Ret result = std::apply(caller, arg_storage);
            std::memcpy(sp, &result, sizeof(result));
            sp += sizeof(result);
        } else {
            MethodCaller<Method, ObjType, void> caller(method, obj);
            std::apply(caller, arg_storage);
        }
    };
}

template<typename Method, typename Ret>
struct StaticMethodCaller {
    Method method;

    StaticMethodCaller(Method m) : method(m) {}

    template<typename... Args>
    Ret operator()(Args&&... unpacked) {
        return method(
            ([&]() -> decltype(auto) {
                if constexpr (std::is_pointer_v<std::decay_t<Args>>)
                    return *unpacked;
                else
                    return unpacked;
            }())...
        );
    }
};


template<typename StaticMethod>
StaticMethodFunc bind_static_method(StaticMethod method) {
    using Traits = MethodTraits<StaticMethod>;
    using Ret = typename Traits::ReturnType;
    using ArgTuple = typename Traits::ArgTuple;
    using ArgStorage = typename ArgStorageTuple<ArgTuple>::type;

    return [method](Value*& sp) {
        ArgStorage arg_storage;

        auto pop_any = [&](auto& slot) {
            pop_ptr_from_stack(sp, &slot, sizeof(slot));
        };

        std::apply([&](auto&... unpacked) {
            (pop_any(unpacked), ...);
        }, arg_storage);

        if constexpr (!std::is_void_v<Ret>) {
            StaticMethodCaller<StaticMethod, Ret> caller(method);
            Ret result = std::apply(caller, arg_storage);
            std::memcpy(sp, &result, sizeof(result));
            sp += sizeof(result);
        } else {
            StaticMethodCaller<StaticMethod, void> caller(method);
            std::apply(caller, arg_storage);
        }
    };
}




} // namespace vm

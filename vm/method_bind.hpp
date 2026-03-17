#pragma once

#include <functional>
#include <cstring>
#include <type_traits>

#include "value.hpp"

namespace vm {

using MethodFunc = std::function<void(Value* args, void* raw_obj, void* ret_ptr)>;

template<typename Constructor>
MethodFunc bind_constructor();

template <class Method>
MethodFunc bind_method(Method method);

template <class Method>
MethodFunc bind_method_ptr_args(Method method);

template <class Method>
MethodFunc bind_static_method(Method method);

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

// Helper to pop a pointer from stack (Value*& sp)
inline void get_from_array(Value*& args, void* out, size_t size) {
    std::memcpy(out, args, size);
	args += size;
}

template<typename Method>
MethodFunc bind_method(Method method) {
    using Traits = MethodTraits<Method>;
    using T = typename Traits::ClassType;
    using Ret = typename Traits::ReturnType;
    using ArgTuple = typename Traits::ArgTuple;
    using ArgStorage = typename ArgPtrTupleHelper<ArgTuple>::type;

    return [method](Value* args, void* raw_obj, void* ret_ptr) {
        ArgStorage arg_storage;

        // For each tuple element: point it at the stack slot
        auto pop_any = [&](auto& slot) {
            using Obj = std::remove_pointer_t<std::decay_t<decltype(*slot)>>;

            slot = reinterpret_cast<Obj*>(args);
            args += sizeof(Obj);
        };

        std::apply([&](auto&... unpacked) {
            (pop_any(unpacked), ...);
        }, arg_storage);

        using ObjType = std::conditional_t<Traits::is_const, const T*, T*>;
        ObjType obj = reinterpret_cast<ObjType>(raw_obj);

        if constexpr (!std::is_same_v<Ret, void>) {
            MethodCaller<Method, ObjType, Ret> caller(method, obj);
            Ret result = std::apply(caller, arg_storage);
            new (ret_ptr) Ret(std::move(result));
        } else {
            MethodCaller<Method, ObjType, void> caller(method, obj);
            std::apply(caller, arg_storage);
        }
    };
}

template<typename Constructor>
MethodFunc bind_constructor() {
    using Traits = ConstructorTraits<Constructor>;
    using ClassType = typename Traits::ClassType;
    using ArgTuple = typename Traits::ArgTuple;
    using ArgStorage = typename ArgStorageTuple<ArgTuple>::type;

    return [](Value* args, void* raw_obj, void*) {
        ArgStorage arg_storage;

        auto pop_any = [&](auto& slot) {
            get_from_array(args, &slot, sizeof(slot));
        };

        std::apply([&](auto&... unpacked) {
            (pop_any(unpacked), ...);
        }, arg_storage);

        std::apply([&](auto&&... unpacked) {
            new (raw_obj) ClassType(std::forward<decltype(unpacked)>(unpacked)...);
        }, arg_storage);
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


template<typename Method>
MethodFunc bind_static_method(Method method) {
    using Traits    = MethodTraits<Method>;
    using Ret        = typename Traits::ReturnType;
    using ArgTuple    = typename Traits::ArgTuple;

    // ArgStorage is a tuple of pointers (from ArgPtrTupleHelper)
    using ArgStorage = typename ArgPtrTupleHelper<ArgTuple>::type;

    return [method](Value* args, void*, void* ret_ptr) {
        ArgStorage arg_storage;

        // For each tuple element: point it at the stack slot
        auto pop_any = [&](auto& slot) {
            using Obj = std::remove_pointer_t<std::decay_t<decltype(*slot)>>;

            slot = reinterpret_cast<Obj*>(args);
            args += sizeof(Obj);
        };

        std::apply([&](auto&... unpacked) {
            (pop_any(unpacked), ...);
        }, arg_storage);

        if constexpr (!std::is_same_v<Ret, void>) {
            StaticMethodCaller<Method, Ret> caller(method);
            Ret result = std::apply(caller, arg_storage);

            new (ret_ptr) Ret(std::move(result));
        } else {
            StaticMethodCaller<Method, void> caller(method);
            std::apply(caller, arg_storage);
        }
    };
}

} // namespace vm

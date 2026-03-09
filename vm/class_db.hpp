#pragma once
#include <vector>
#include <functional>
#include <iostream>
#include <cstring>
#include <type_traits>
#include <tuple>
#include "value.hpp"
#include <regex>
#include "function_parser.hpp"
#include "type_traits.hpp"

namespace vm {

class Value;
using MethodFunc = std::function<void(Value* args, void* raw_obj, void* ret_ptr)>;
using StaticMethodFunc = MethodFunc; //std::function<void(Value*, void* ret_ptr)>;

struct MethodPair {
    std::string name;
	std::string signature;
    MethodFunc value_call;
    MethodFunc pointer_call;
	bool is_global = false;
	std::vector<std::string> arg_types;
	size_t arg_count;
	uint16_t class_id;
	uint16_t method_id;
	bool is_constructor;
};

struct ClassBind {
    const std::string name;
    const int id;
	const size_t size;
    std::vector<MethodPair> static_methods;
    std::vector<MethodPair> methods;

	ClassBind(const std::string& name, size_t id, size_t size)
		: name(name), id(id), size(size) {}
};

template<typename Constructor>
MethodFunc bind_constructor();

template <class Method>
MethodFunc bind_method(Method method);

template <class Method>
MethodFunc bind_method_ptr_args(Method method);

template <class Method>
StaticMethodFunc bind_static_method(Method method);

namespace {
	std::string replace_member_pointer_with_name(const std::string& input, const std::string& funcName) {
    	return std::regex_replace(input, std::regex(R"(\(\w+::\*\)|\(\*\))"), funcName);
	}
}

class ClassDB {
public:
    std::vector<ClassBind> classes;

	template<typename ClassType>
    size_t register_class(std::string name) {
        classes.push_back(ClassBind(name, classes.size(), sizeof(ClassType)));
		return classes.back().id;
    }

    template<typename Constructor>
    void register_constructor(int class_id, std::string signature) {
        MethodPair pair;
		pair.name = classes[class_id].name;
		pair.signature = signature;
		pair.is_global = true;
        pair.value_call = bind_constructor<Constructor>();
        pair.pointer_call = bind_constructor<Constructor>();

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = false;
		pair.is_constructor = true;

        classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_method(int class_id, std::string name, Method method, std::string signature) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_method(method);
        pair.pointer_call = bind_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = false;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }

    template<typename Method>
    void register_static_method(int class_id, std::string name, Method method, std::string signature, bool is_global = false) {
        MethodPair pair;
        pair.name = name;
		pair.signature = replace_member_pointer_with_name(signature, name);
        pair.value_call = bind_static_method(method);
        pair.pointer_call = bind_static_method(method); //bind_method_ptr_args(method);

		auto parsed = FunctionParser::parse(signature);
		pair.arg_types = parsed.parameter_types;
		pair.arg_count = parsed.parameters.size();

		pair.class_id = class_id;
		pair.method_id = classes[class_id].methods.size();

		pair.is_global = is_global;
		pair.is_constructor = false;

		classes[class_id].methods.push_back(std::move(pair));
    }
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

/*
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
*/

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
StaticMethodFunc bind_static_method(Method method) {
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

#pragma once
#include <tuple>

template<typename T>
struct ConstructorTraits;

template<typename ClassType_, typename... Args>
struct ConstructorTraits<ClassType_(Args...)> {
    using ClassType = ClassType_;
    using ArgTuple = std::tuple<Args...>;
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


// static
template<typename Ret, typename... Args>
struct MethodTraits<Ret (*)(Args...)> {
    using ClassType = void;  // Add this line
    using ReturnType = Ret;
    using ArgTuple = std::tuple<Args...>;
    static constexpr bool is_const = false;
};
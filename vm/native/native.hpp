#pragma once
#include <type_traits>  // For std::is_trivially_copyable

namespace auplib {

// Base template — defaults to false
template<typename T>
struct is_trivial {
    static constexpr bool value = false;
};

}

// Macro to mark a type as primitive, with a compile-time check
#define MARK_TRIVIAL(TYPE)                                   \
    static_assert(std::is_trivially_copyable<TYPE>::value,     \
                  #TYPE " must be trivially copyable to be primitive"); \
    static_assert(std::is_default_constructible_v<TYPE>, "vec2 must be default constructible");\
    template<>                                                 \
    struct is_trivial<TYPE> {                                \
        static constexpr bool value = true;                    \
    };

#define IS_OBJECT(T) (std::is_base_of_v<Object, T>)

// Macro to query primitiveness
#define IS_TRIVIAL(T) (auplib::is_trivial<T>::value)

#define DECLARE_UTILITY_CLASS(ClassName) \
    ClassName() = delete;                          \
    ~ClassName() = delete;                         \
    ClassName(const ClassName&) = delete;          \
    ClassName(ClassName&&) = delete;               \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define STATIC_ASSERT_UTILITY_CLASS(ClassName) \
    static_assert(std::is_empty<ClassName>::value, #ClassName " must only contain static members.")

#define DECLARE_OBJECT_CLASS(m_name, m_parent)

#define REGISTER_CLASS(m_name) db.register_class<m_name>(#m_name)

#define REGISTER_VARIABLE(m_id, m_type, m_name) \
	db.register_variable<m_type>(m_id, #m_name, #m_type)

#define REGISTER_CONSTRUCTOR(m_id, ...) \
    db.register_constructor<__VA_ARGS__>(m_id, #__VA_ARGS__)

#define REGISTER_METHOD(m_id, m_class, m_name, ...) \
    db.register_method<__VA_ARGS__>(m_id, #m_name, &m_class::m_name, #__VA_ARGS__)

#define REGISTER_STATIC_METHOD(m_id, m_class, m_name, ...) \
    db.register_static_method<__VA_ARGS__>(m_id, #m_name, &m_class::m_name, #__VA_ARGS__)

#define REGISTER_GLOBAL_METHOD(m_id, m_class, m_name, ...) \
    db.register_static_method<__VA_ARGS__>(m_id, #m_name, &m_class::m_name, #__VA_ARGS__, true)

#define CREATE_GENERICS(_) \
	_(vm::Value)

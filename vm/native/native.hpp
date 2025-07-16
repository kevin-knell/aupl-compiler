#pragma once
#include <type_traits>  // For std::is_trivially_copyable

// Base template — defaults to false
template<typename T>
struct is_primitive {
    static constexpr bool value = false;
};

// Macro to mark a type as primitive, with a compile-time check
#define MARK_PRIMITIVE(TYPE)                                   \
    static_assert(std::is_trivially_copyable<TYPE>::value,     \
                  #TYPE " must be trivially copyable to be primitive"); \
    static_assert(std::is_default_constructible_v<TYPE>, "vec2 must be default constructible");\
    template<>                                                 \
    struct is_primitive<TYPE> {                                \
        static constexpr bool value = true;                    \
    };

// Macro to query primitiveness
#define IS_PRIMITIVE(T) (is_primitive<T>::value)

#define DECLARE_UTILITY_CLASS(ClassName) \
    ClassName() = delete;                          \
    ~ClassName() = delete;                         \
    ClassName(const ClassName&) = delete;          \
    ClassName(ClassName&&) = delete;               \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define STATIC_ASSERT_UTILITY_CLASS(ClassName) \
    static_assert(std::is_empty<ClassName>::value, #ClassName " must only contain static members.")

#define REGISTER_CLASS(m_id, m_name) db.register_class(m_id, #m_name)

#define REGISTER_METHOD(m_id, m_class, m_name, ...) \
    db.register_method<__VA_ARGS__>(m_id, #m_name, &m_class::m_name)

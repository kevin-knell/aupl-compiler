#pragma once
#include <cstdint>
#include <memory>

#include "class.hpp"
#include "function.hpp"

class Object;

namespace vm {
    struct Value {
        union {
            int8_t i8;
            uint8_t u8;
            bool b;
        };

        template<typename T>
        T as() const {
            if constexpr (std::is_same_v<T, int8_t>)
                return i8;
            else if constexpr (std::is_same_v<T, uint8_t>)
                return u8;
            else if constexpr (std::is_same_v<T, bool>)
                return b;
            else
                static_assert(!sizeof(T), "Unsupported Value::as<T>()");
        }

        template<typename T>
        static Value from(T v) {
            if constexpr (std::is_same_v<T, int8_t>)
                return Value{.i8 = v};
            else if constexpr (std::is_same_v<T, uint8_t>)
                return Value{.u8 = v};
            else if constexpr (std::is_same_v<T, bool>)
                return Value{.b = v};
            else
                static_assert(!sizeof(T), "Unsupported Value::from<T>()");
        }
    };
    static_assert(sizeof(Value) == 1);

    struct Value2 {
        union {
            int16_t i16;
            uint16_t u16;

            Value v[2];
        };

        template<typename T>
        T as() const {
            if constexpr (std::is_same_v<T, int16_t>)
                return i16;
            else if constexpr (std::is_same_v<T, uint16_t>)
                return u16;
            else
                static_assert(!sizeof(T), "Unsupported Value2::as<T>()");
        }

        template<typename T>
        static Value2 from(T v) {
            Value2 val{};
            if constexpr (std::is_same_v<T, int16_t>)
                val.i16 = v;
            else if constexpr (std::is_same_v<T, uint16_t>)
                val.u16 = v;
            else
                static_assert(!sizeof(T), "Unsupported Value2::from<T>()");
            return val;
        }
    };
    static_assert(sizeof(Value2) == 2);

    struct Value4 {
        union {
            int32_t i32;
            uint32_t u32;
            float f32;
            
            Value v[4];
            Value2 v2[2];
        };

        template<typename T>
        T as() const {
            if constexpr (std::is_same_v<T, int32_t>)
                return i32;
            else if constexpr (std::is_same_v<T, uint32_t>)
                return u32;
            else if constexpr (std::is_same_v<T, float>)
                return f32;
            else
                static_assert(!sizeof(T), "Unsupported Value4::as<T>()");
        }

        template<typename T>
        static Value4 from(T v) {
            Value4 val{};
            if constexpr (std::is_same_v<T, int32_t>)
                val.i32 = v;
            else if constexpr (std::is_same_v<T, uint32_t>)
                val.u32 = v;
            else if constexpr (std::is_same_v<T, float>)
                val.f32 = v;
            else
                static_assert(!sizeof(T), "Unsupported Value4::from<T>()");
            return val;
        }
    };
    static_assert(sizeof(Value4) == 4);

    struct Value8 {
        union {
            int64_t i64;
            uint64_t u64;
            size_t s;
            double f64;
            void* ptr;

            Value v[8];
            Value2 v2[4];
            Value4 v4[2];
        };

        template<typename T>
        T as() const {
            if constexpr (std::is_same_v<T, int64_t>)
                return i64;
            else if constexpr (std::is_same_v<T, uint64_t>)
                return u64;
            else if constexpr (std::is_same_v<T, double>)
                return f64;
            else
                static_assert(!sizeof(T), "Unsupported Value8::as<T>()");
        }

        template<typename T>
        static Value8 from(T v) {
            Value8 val{};
            if constexpr (std::is_same_v<T, int64_t>)
                val.i64 = v;
            else if constexpr (std::is_same_v<T, uint64_t>)
                val.u64 = v;
            else if constexpr (std::is_same_v<T, double>)
                val.f64 = v;
            else
                static_assert(!sizeof(T), "Unsupported Value8::from<T>()");
            return val;
        }
    };
    static_assert(sizeof(Value8) == 8);

    struct Value16 {
        union {
            std::shared_ptr<void> shared;
            std::weak_ptr<void> weak;

            Value v[16];
            Value2 v2[8];
            Value4 v4[4];
            Value8 v8[2];
        };

        template<typename T>
        T as() const {
            static_assert(!sizeof(T), "Unsupported Value16::as<T>()");
        }

        template<typename T>
        static Value16 from(T v) {
            static_assert(!sizeof(T), "Unsupported Value16::from<T>()");
            return Value16();
        }
    };
    static_assert(sizeof(Value16) == 16);

} // namespace vm

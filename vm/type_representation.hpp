#include <tuple>
#include <string>
#include <typeinfo>
#include <cxxabi.h>
#include <vector>
#include "type_traits.hpp"
#include <memory>

std::string demangle(const char* name) {
    int status;
    char* realname = abi::__cxa_demangle(name, 0, 0, &status);
    std::string result = (status == 0) ? realname : name;
    free(realname);
    return result;
}


struct CType {
	virtual size_t size() const = 0;
	virtual bool is_const() const = 0;
	virtual std::string to_string() const = 0;
};

using CTypePtr = std::shared_ptr<CType>;

struct CBaseType : CType {
	const std::string name;
	const size_t size;

	CBaseType(std::string name, size_t size) : name(name), size(size) {}

	size_t size() const override;
	bool is_const() const override;
	std::string to_string() const override;
};

struct CPointerType : CType {
	const CTypePtr base_type;

	size_t size() const override;
	std::string to_string() const override;
};

struct CRefType : CType {
	const CTypePtr base_type;

	CRefType(CTypePtr base_type) : base_type(base_type) {}

	size_t size() const override;
	bool is_const() const override;
	std::string to_string() const override;
};

struct CFuncType : CType {
	const CTypePtr return_type;
	const CTypePtr class_type;
	const std::vector<CTypePtr> param_types;

	size_t size() const override;
	bool is_const() const override;
	std::string to_string() const override;
};


template<typename T>
CTypePtr create_ctype();

#define CREATE_BASE_TYPE(TYPE) \
template<typename T> \
CTypePtr create_ctype<TYPE>() { return std::make_shared<CBaseType>(#TYPE, sizeof(TYPE)); }

CREATE_BASE_TYPE(void)

CREATE_BASE_TYPE(int8_t)
CREATE_BASE_TYPE(int16_t)
CREATE_BASE_TYPE(int32_t)
CREATE_BASE_TYPE(int64_t)

CREATE_BASE_TYPE(uint8_t)
CREATE_BASE_TYPE(uint16_t)
CREATE_BASE_TYPE(uint32_t)
CREATE_BASE_TYPE(uint64_t)

CREATE_BASE_TYPE(char)
CREATE_BASE_TYPE(short)
CREATE_BASE_TYPE(int)
CREATE_BASE_TYPE(long)
CREATE_BASE_TYPE(float)
CREATE_BASE_TYPE(double)

template<typename T>
CTypePtr create_ctype<T>() {
	if constexpr(std::is_reference_v(T)) {
		auto result = std::make_shared<CRefType>()
	}
}
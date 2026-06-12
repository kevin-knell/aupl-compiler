#pragma once

// default
//#define CTOR_DEFAULT(m_name) m_name() = default;

#define COPY_DEFAULT(m_name)								\
	m_name(const m_name& other) = default;					\
	m_name& operator=(const m_name& other) = default;

#define MOVE_DEFAULT(m_name)								\
	m_name(m_name&& other) = default;						\
	m_name& operator=(m_name&& other) = default;

//#define DTOR_DEFAULT(m_name) ~m_name() = default;

//#define VIRTUAL_DTOR_DEFAULT(m_name) virtual ~m_name() = default;

#define ALL_FUNC_DEFAULT(m_name) m_name() = default; COPY_DEFAULT(m_name) MOVE_DEFAULT(m_name) ~m_name() = default;

#define TRIVIAL(m_name) ALL_DEFAULT(m_name)

// delete
//#define CTOR_DELETE(m_name) m_name() = delete;

#define COPY_DELETE(m_name)									\
	m_name(const m_name& other) = delete;					\
	m_name& operator=(const m_name& other) = delete;

#define MOVE_DELETE(m_name)									\
	m_name(m_name&& other) = delete;						\
	m_name& operator=(m_name&& other) = delete;


// set / get
#define SETTER(m_name) void set_##m_name(decltype(m_name) _val) { m_name = _val; }
#define GETTER(m_name) decltype(m_name) get_##m_name() const { return m_name; }

#define SETGET(m_name) SETTER(m_name) GETTER(m_name)
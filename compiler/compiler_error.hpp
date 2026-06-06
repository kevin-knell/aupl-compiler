#pragma once

#include <iostream>

#define COMPILER_ERR(m_message) std::cerr << __FILE__ << ":" << __LINE__ << ":" << m_message << std::endl; (std::abort())

#define COMPILER_WARN(m_message) std::cerr << __FILE__ << ":" << __LINE__ << ":" << m_message << std::endl

#define COMPILER_ASSERT(m_condition, m_message) if (!(m_condition)) { COMPILER_ERR(m_message); }

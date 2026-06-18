#pragma once

#include <iostream>

#define FILE_LINE std::string(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ":")

#define COMPILER_ERR(m_message) std::cerr << (FILE_LINE) << (m_message) << std::endl; (std::abort())

#define COMPILER_WARN(m_message) std::cerr << (FILE_LINE) << (m_message) << std::endl

#define COMPILER_ASSERT(m_condition, m_message) if (!(m_condition)) { COMPILER_ERR(m_message); }

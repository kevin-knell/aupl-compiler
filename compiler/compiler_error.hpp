#pragma once

#include <iostream>

#define FILE_LINE std::string(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ":")

#define COMPILER_ERR(m_message) do { std::cerr << (FILE_LINE) << (m_message) << std::endl; (std::abort()); } while (0)

#define COMPILER_WARN(m_message) do { std::cerr << (FILE_LINE) << (m_message) << std::endl; } while (0)

#define COMPILER_ASSERT(m_condition, m_message) do { if (!(m_condition)) { COMPILER_ERR(m_message); } } while (0)

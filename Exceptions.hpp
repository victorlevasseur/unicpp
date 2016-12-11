#ifndef UNICPP_EXCEPTIONS_H
#define UNICPP_EXCEPTIONS_H

#include <stdexcept>

namespace unicpp
{

class invalid_utf8_exception : public std::runtime_error
{
public: 
    explicit invalid_utf8_exception (const std::string& what_arg) : std::runtime_error(what_arg) {}
    explicit invalid_utf8_exception (const char* what_arg) : std::runtime_error(what_arg) {}
};

class bad_utf8_sequence_exception : public std::runtime_error
{
public: 
    explicit bad_utf8_sequence_exception (const std::string& what_arg) : std::runtime_error(what_arg) {}
    explicit bad_utf8_sequence_exception (const char* what_arg) : std::runtime_error(what_arg) {}
};

class bad_utf16_sequence_exception : public std::runtime_error
{
public: 
    explicit bad_utf16_sequence_exception (const std::string& what_arg) : std::runtime_error(what_arg) {}
    explicit bad_utf16_sequence_exception (const char* what_arg) : std::runtime_error(what_arg) {}
};

class invalid_codepoint_exception : public std::runtime_error
{
public: 
    explicit invalid_codepoint_exception (const std::string& what_arg) : std::runtime_error(what_arg) {}
    explicit invalid_codepoint_exception (const char* what_arg) : std::runtime_error(what_arg) {}
};

}

#endif
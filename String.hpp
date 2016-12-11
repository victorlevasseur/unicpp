#ifndef UNICPP_STRING_H
#define UNICPP_STRING_H

#include <string>

#include "Utf8Tools.hpp"

namespace unicpp
{

class string
{
public: 
    string();
    string(const char* str);
    string(const char* str, std::size_t size);
    string(std::size_t count, char32_t character);
    
    string(const std::u16string& utf16str);
    string(const std::u32string& utf32str);

    std::string& std_str();
    const std::string& std_str() const;

    std::wstring w_str();
    std::u16string utf16_str() const;
    std::u32string utf32_str() const;

    bool is_valid() const;

private:
    std::string m_content;
};

}

#endif
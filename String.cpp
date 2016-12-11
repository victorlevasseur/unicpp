#include "String.hpp"

#include <iostream>

namespace unicpp
{

string::string() :
    m_content()
{

}

string::string(const char* str) :
    m_content(str)
{
    
}

string::string(const char* str, size_t size) : 
    m_content(str, size)
{
    
}

string::string(std::size_t count, char32_t character)
{
    std::u32string utf32str(count, character);
    utf32_to_utf8(utf32str.begin(), utf32str.end(), std::back_inserter(m_content));
}

string::string(const std::u16string& utf16str)
{
    utf16_to_utf8(utf16str.begin(), utf16str.end(), std::back_inserter(m_content));
}

string::string(const std::u32string& utf32str)
{
    utf32_to_utf8(utf32str.begin(), utf32str.end(), std::back_inserter(m_content));
}

std::string& string::std_str()
{
    return m_content;
}

const std::string& string::std_str() const
{
    return m_content;
}

std::wstring string::w_str()
{
    std::wstring result;

    return result;
}

std::u16string string::utf16_str() const
{
    std::u16string result;
    utf8_to_utf16(m_content.begin(), m_content.end(), std::back_inserter(result));

    return result;
}

std::u32string string::utf32_str() const
{
    std::u32string result;
    utf8_to_utf32(m_content.begin(), m_content.end(), std::back_inserter(result));

    return result;
}

bool string::is_valid() const
{
    return is_valid_utf8(m_content.begin(), m_content.end());
}

}
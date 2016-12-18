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

string::const_iterator string::begin() const
{
    return const_iterator(m_content, m_content.begin());
}

string::const_iterator string::cbegin() const
{
    return const_iterator(m_content, m_content.begin());
}

string::const_reverse_iterator string::rbegin() const
{
    return std::reverse_iterator<string::const_iterator>(begin());
}

string::const_reverse_iterator string::crbegin() const
{
    return std::reverse_iterator<string::const_iterator>(cbegin());
}

string::iterator string::begin()
{
    return iterator(m_content, m_content.begin());
}

string::reverse_iterator string::rbegin()
{
    return std::reverse_iterator<string::iterator>(begin());
}

string::const_iterator string::end() const
{
    return const_iterator(m_content, m_content.end());
}

string::const_iterator string::cend() const
{
    return const_iterator(m_content, m_content.end());
}

string::const_reverse_iterator string::rend() const
{
    return std::reverse_iterator<string::const_iterator>(end());
}

string::const_reverse_iterator string::crend() const
{
    return std::reverse_iterator<string::const_iterator>(cend());
}

string::iterator string::end()
{
    return iterator(m_content, m_content.end());
}

string::reverse_iterator string::rend()
{
    return std::reverse_iterator<string::iterator>(end());
}

string::const_grapheme_iterator string::gbegin() const
{
    return const_grapheme_iterator(*this, cbegin());
}

string::const_grapheme_iterator string::gend() const
{
    return const_grapheme_iterator(*this, cend());
}

std::size_t string::size() const
{
    return std::distance(cbegin(), cend());
}

}

#ifndef UNICPP_STRING_H
#define UNICPP_STRING_H

#include <iterator>
#include <string>

#include "Utf8Tools.hpp"

namespace unicpp
{

class string;

template<typename StringRef, typename InternalIterator>
class string_iterator : public std::iterator<std::bidirectional_iterator_tag, char32_t>
{
    friend class string;
public:
    using iterator_type = string_iterator<StringRef, InternalIterator>;

    string_iterator() {}

private:
    string_iterator(StringRef str, InternalIterator it) : 
        internal_string(str),
        internal_it(it)
    {

    }

public:
    template<typename S, typename I>
    string_iterator(const string_iterator<S, I>& other) :
        internal_string(other.internal_string),
        internal_it(other.internal_it)
    {

    }

    iterator_type& operator++()
    {
        iterate_next(internal_it, internal_string.end());
        return *this;
    }

    iterator_type operator++(int)
    {
        iterator_type tmp(*this);
        iterate_next(internal_it, internal_string.end());
        return tmp;
    }

    bool operator==(const iterator_type& rhs)
    {
        return internal_it == rhs.internal_it;
    }
    
    bool operator!=(const iterator_type& rhs)
    {
        return internal_it != rhs.internal_it;
    }

    char32_t operator*()
    {
        auto tmp = InternalIterator(internal_it);
        return iterate_next(tmp, internal_string.end());
    }

    StringRef internal_string;
    InternalIterator internal_it;
};

class string
{
public: 

    using iterator = string_iterator<std::string&, std::string::iterator>;
    using const_iterator = string_iterator<const std::string&, std::string::const_iterator>;

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

    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator begin();

    const_iterator end() const;
    const_iterator cend() const;
    iterator end();

private:
    std::string m_content;
};

}

#endif
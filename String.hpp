#ifndef UNICPP_STRING_H
#define UNICPP_STRING_H

#include <iterator>
#include <string>

#include "utf8proc/utf8proc.h"

#include "Grapheme.hpp"
#include "Utf8Tools.hpp"

namespace unicpp
{

class string;

template<typename StringRef, typename InternalIterator>
class codepoint_iterator : public std::iterator<std::bidirectional_iterator_tag, char32_t, std::ptrdiff_t, char32_t*, char32_t>
{
    friend class string;

public:
    using iterator_type = codepoint_iterator<StringRef, InternalIterator>;

    codepoint_iterator() {}

private:
    codepoint_iterator(StringRef str, InternalIterator it) :
        internal_string(str),
        internal_it(it)
    {

    }

public:
    template<typename S, typename I>
    codepoint_iterator(const codepoint_iterator<S, I>& other) :
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

    iterator_type& operator--()
    {
        iterate_previous(internal_it, internal_string.begin());
        return *this;
    }

    iterator_type operator--(int)
    {
        iterator_type tmp(*this);
        iterate_previous(internal_it, internal_string.begin());
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

template<typename StringRef, typename CodepointIterator>
class grapheme_iterator : public std::iterator<std::forward_iterator_tag, grapheme, std::ptrdiff_t, grapheme, grapheme>
{
    friend class string;

public:
    using iterator_type = grapheme_iterator<StringRef, CodepointIterator>;

    grapheme_iterator() : state(0) {}

private:
    grapheme_iterator(StringRef str, CodepointIterator it) :
        internal_string(str),
        codepoint_it(it),
        state(0)
    {

    }

public:
    template<typename S, typename I>
    grapheme_iterator(const grapheme_iterator<S, I>& other) :
        internal_string(other.internal_string),
        codepoint_it(other.codepoint_it),
        state(other.state)
    {

    }

    iterator_type& operator++()
    {
        char32_t codepoint = *codepoint_it;
        ++codepoint_it;

        if(codepoint_it == internal_string.cend())
            return *this;

        char32_t next_codepoint = *codepoint_it;
        bool has_entered_loop = false;
        while(!utf8proc_grapheme_break_stateful(codepoint, next_codepoint, &state))
        {
            has_entered_loop = true;
            codepoint = next_codepoint;

            ++codepoint_it;
            if(codepoint_it == internal_string.cend())
                break;

            next_codepoint = *codepoint_it;
        }

        return *this;
    }

    iterator_type operator++(int)
    {
        iterator_type tmp(*this);
        operator++();
        return tmp;
    }

    bool operator==(const iterator_type& rhs)
    {
        return codepoint_it == rhs.codepoint_it;
    }

    bool operator!=(const iterator_type& rhs)
    {
        return codepoint_it != rhs.codepoint_it;
    }

    grapheme operator*()
    {
        auto tmp = CodepointIterator(codepoint_it);

        char32_t codepoint = *tmp;
        ++tmp;

        std::u32string grapheme_str{codepoint};

        if(tmp == internal_string.cend())
            return grapheme(grapheme_str, false);

        char32_t next_codepoint = *tmp;
        while(!utf8proc_grapheme_break_stateful(codepoint, next_codepoint, &state))
        {
            codepoint = next_codepoint;
            grapheme_str.push_back(codepoint);

            ++tmp;
            if(tmp == internal_string.cend())
                break;

            next_codepoint = *tmp;
        }

        return grapheme(grapheme_str, false);
    }

    StringRef internal_string;
    CodepointIterator codepoint_it;
    utf8proc_int32_t state;
};

class as_codepoints
{
public:
    using offset_type = std::size_t;
    using const_iterator = codepoint_iterator<const std::string&, std::string::const_iterator>;

    static const_iterator cbegin(const string & str);

    static const_iterator cend(const string & str);

    static bool advance_safe(const string & str, const_iterator & it, offset_type offset);

    static offset_type get_byte_distance(const string & str, const const_iterator & b, const const_iterator & it);

    static offset_type get_codepoint_distance(const string & str, const const_iterator & b, const const_iterator & it);
};

class as_graphemes
{
public:
    using offset_type = std::size_t;
    using const_iterator = grapheme_iterator<const string&, codepoint_iterator<const std::string&, std::string::const_iterator>>;

    static const_iterator cbegin(const string & str);

    static const_iterator cend(const string & str);

    static bool advance_safe(const string & str, const_iterator & it, offset_type offset);

    static offset_type get_byte_distance(const string & str, const const_iterator & b, const const_iterator & it);

    static offset_type get_codepoint_distance(const string & str, const const_iterator & b, const const_iterator & it);
};

class string
{
public:

    using iterator = codepoint_iterator<std::string&, std::string::iterator>;
    using const_iterator = codepoint_iterator<const std::string&, std::string::const_iterator>;

    using reverse_iterator = std::reverse_iterator<string::iterator>;
    using const_reverse_iterator = std::reverse_iterator<string::const_iterator>;

    using const_grapheme_iterator = grapheme_iterator<const string&, const_iterator>;

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
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    iterator begin();
    reverse_iterator rbegin();

    const_iterator end() const;
    const_iterator cend() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
    iterator end();
    reverse_iterator rend();

    const_grapheme_iterator gbegin() const;
    const_grapheme_iterator gend() const;

    template<typename Unit = as_codepoints>
    std::size_t size() const
    {
        return std::distance(Unit::cbegin(*this), Unit::cend(*this));
    }

private:
    std::string m_content;
};

}

#endif

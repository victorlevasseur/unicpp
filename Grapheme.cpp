#include "Grapheme.hpp"

#include "utf8proc/utf8proc.h"

#include "Exceptions.hpp"

namespace unicpp
{

grapheme::grapheme(const std::u32string & codepoints) :
    m_codepoints(codepoints)
{
    //Check if it's a single grapheme
    utf8proc_int32_t state = 0;
    for(std::size_t i = 0; i < m_codepoints.size() - 1; ++i)
    {
        if(utf8proc_grapheme_break_stateful(m_codepoints[i], m_codepoints[i + 1], &state))
            throw invalid_grapheme_exception("Found an grapheme break in a grapheme!");
    }
}

std::size_t grapheme::codepoints_count() const
{
    return m_codepoints.size();
}

std::u32string::const_iterator grapheme::codepoints_begin() const
{
    return m_codepoints.cbegin();
}

std::u32string::const_iterator grapheme::codepoints_end() const
{
    return m_codepoints.cend();
}

char32_t grapheme::operator[](std::size_t i) const
{
    return m_codepoints[i];
}

grapheme grapheme::get_compat() const
{
    std::u32string new_codepoints;
    for(std::size_t i = 0; i < codepoints_count(); ++i)
    {
        std::size_t count = utf8proc_decompose_char(m_codepoints[i], nullptr, 0, UTF8PROC_COMPAT, nullptr);
        utf8proc_int32_t decomposed_codepoints[count];
        utf8proc_decompose_char(m_codepoints[i], decomposed_codepoints, count, UTF8PROC_COMPAT, nullptr);

        for(std::size_t j = 0; j < count; ++j)
            new_codepoints.push_back(static_cast<char32_t>(decomposed_codepoints[j]));
    }

    return grapheme(new_codepoints, false);
}

grapheme grapheme::get_casefold() const
{
    std::u32string new_codepoints;
    for(std::size_t i = 0; i < codepoints_count(); ++i)
    {
        std::size_t count = utf8proc_decompose_char(m_codepoints[i], nullptr, 0, UTF8PROC_CASEFOLD, nullptr);
        utf8proc_int32_t decomposed_codepoints[count];
        utf8proc_decompose_char(m_codepoints[i], decomposed_codepoints, count, UTF8PROC_CASEFOLD, nullptr);

        for(std::size_t j = 0; j < count; ++j)
            new_codepoints.push_back(static_cast<char32_t>(decomposed_codepoints[j]));
    }

    return grapheme(new_codepoints, false);
}

grapheme::grapheme(const std::u32string & codepoints, bool) :
    m_codepoints(codepoints)
{
    // No checks at all in this special version
}

}

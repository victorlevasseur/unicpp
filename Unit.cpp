#include "String.hpp"

namespace unicpp
{

as_codepoints::const_iterator as_codepoints::cbegin(const string & str)
{
    return str.cbegin();
}

as_codepoints::const_iterator as_codepoints::cend(const string & str)
{
    return str.cend();
}

bool as_codepoints::advance_safe(const string & str, as_codepoints::const_iterator & it, as_codepoints::offset_type offset)
{
    while(it != cend(str) && offset > 0)
    {
        --offset;
        ++it;
    }

    if(offset == 0)
        return true;
    else
        return false;
}

as_codepoints::offset_type get_byte_distance(const string & str, const as_codepoints::const_iterator & b, const as_codepoints::const_iterator & it)
{
    return std::distance(b.internal_it, it.internal_it);
}

as_codepoints::offset_type get_codepoint_distance(const string & str, const as_codepoints::const_iterator & b, const as_codepoints::const_iterator & it)
{
    return std::distance(b, it);
}

as_graphemes::const_iterator as_graphemes::cbegin(const string & str)
{
    return str.gbegin();
}

as_graphemes::const_iterator as_graphemes::cend(const string & str)
{
    return str.gend();
}

bool as_graphemes::advance_safe(const string & str, as_graphemes::const_iterator & it, as_graphemes::offset_type offset)
{
    while(it != cend(str) && offset > 0)
    {
        --offset;
        ++it;
    }

    if(offset == 0)
        return true;
    else
        return false;
}

as_graphemes::offset_type as_graphemes::get_byte_distance(const string & str, const as_graphemes::const_iterator & b, const as_graphemes::const_iterator & it)
{
    return std::distance(b.codepoint_it.internal_it, it.codepoint_it.internal_it);
}

as_graphemes::offset_type as_graphemes::get_codepoint_distance(const string & str, const as_graphemes::const_iterator & b, const as_graphemes::const_iterator & it)
{
    return std::distance(b.codepoint_it, it.codepoint_it);
}

}

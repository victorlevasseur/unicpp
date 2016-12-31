#ifndef UNICPP_GRAPHEME_H
#define UNICPP_GRAPHEME_H

#include <string>

namespace unicpp
{

template<typename StringRef, typename CodepointIterator>
class grapheme_iterator;

class grapheme
{
    template<typename StringRef, typename CodepointIterator>
    friend class grapheme_iterator;

public:
    grapheme(const std::u32string & codepoints);

    std::size_t codepoints_count() const;

    char32_t operator[](std::size_t i) const;

    std::u32string::const_iterator codepoints_begin() const;
    std::u32string::const_iterator codepoints_end() const;

    grapheme get_compat() const;
    grapheme get_casefold() const;

private:
    // Special version of the ctor without checks, the boolean is a dummy parameter to distinguish between the two versions
    grapheme(const std::u32string & codepoints, bool);

    std::u32string m_codepoints;
};

}

#endif

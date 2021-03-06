#ifndef UNICPP_UTF8TOOLS_H
#define UNICPP_UTF8TOOLS_H

#include "Exceptions.hpp"

/**
 * \file Contains some tools to manage Utf8 directly on std::string.
 * Used internally by unicpp::string.
 */

namespace unicpp
{

const char16_t LEAD_SURROGATE_MIN  = 0xd800u;
const char16_t LEAD_SURROGATE_MAX  = 0xdbffu;
const char16_t TRAIL_SURROGATE_MIN = 0xdc00u;
const char16_t TRAIL_SURROGATE_MAX = 0xdfffu;

const char32_t CODE_POINT_MAX = 0x0010ffffu;

bool is_valid_utf8_octet(unsigned char octet);
bool is_valid_codepoint(char32_t codepoint);

/**
    * Convert a utf32 codeunit (representing a single codepoint) to
    * a sequence of utf8 code units representing the same codepoint.
    *
    * *(OutputIterator) must accept a char assignment.
    */
template<typename OutputIterator>
OutputIterator codepoint_to_utf8(char32_t codepoint, OutputIterator output)
{
    if(!is_valid_codepoint(codepoint))
        throw invalid_codepoint_exception("This codepoint is invalid: " + std::to_string(codepoint));

    if(codepoint <= 0x0000007F)
    {
        // Between 00000000 00000000 00000000 00000000 and 00000000 00000000 00000000 01111111
        // Will be encoded to 0xxxxxxx (1 byte) with
        // all the 'x' been the 7 less sign. bits in the same order
        *(output++) = codepoint & 0x7F;
    }
    else if(codepoint <= 0x000007FF)
    {
        // Between 00000000 00000000 00000000 10000000 and 00000000 00000000 00000111 11111111
        // Will be encoded to 110xxxxx 10xxxxxx (2 bytes) with
        // all the 'x' been the 11 less sign. bits in the same order
        *(output++) = 0xC0 | (codepoint >> 6) & 0x1F;
        *(output++) = 0x80 | codepoint & 0x3F;
    }
    else if(codepoint <= 0x0000FFFF)
    {
        // Between 00000000 00000000 00001000 00000000 and 00000000 00000000 11111111 11111111
        // Will be encoded to 1110xxxx 10xxxxxx 10xxxxxx (3 bytes) with
        // all the 'x' been the 11 less sign. bits in the same order

        *(output++) = 0xE0 | (codepoint >> 12) & 0xF;
        *(output++) = 0x80 | (codepoint >> 6) & 0x3F;
        *(output++) = 0x80 | codepoint & 0x3F;
    }
    else if(codepoint <= 0x0010FFFF)
    {
        // Between 00000000 00000001 00000000 00000000 and 00000000 00010000 11111111 11111111
        // Will be encoded to 1110xxxx 10xxxxxx 10xxxxxx (3 bytes) with
        // all the 'x' been the 11 less sign. bits in the same order

        *(output++) = 0xF0 | (codepoint >> 18) & 0x7;
        *(output++) = 0x80 | (codepoint >> 12) & 0x3F;
        *(output++) = 0x80 | (codepoint >> 6) & 0x3F;
        *(output++) = 0x80 | codepoint & 0x3F;
    }
    else
    {
        // Don't know how to encode this.
        throw invalid_codepoint_exception("Can't encode this codepoint: " + std::to_string(static_cast<uint32_t>(codepoint)));
    }

    return output;
}

/**
    * Convert a sequence of utf32 codeunits (codepoints) to
    * a sequence of utf8 code units representing the same codepoints.
    *
    * *(InputIterator) must return a type convertible to char32_t
    * *(OutputIterator) must accept a char assignment.
    */
template<typename InputIterator, typename OutputIterator>
OutputIterator utf32_to_utf8(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; ++it)
    {
        output = codepoint_to_utf8<OutputIterator>(*it, output);
    }

    return output;
}

bool is_utf16_lead_surrogate(char16_t codeunit);

bool is_utf16_trail_surrogate(char16_t codeunit);

bool is_utf16_surrogate(char16_t codeunit);

template<typename InputIterator, typename OutputIterator>
OutputIterator utf16_character_to_utf8(InputIterator & it, InputIterator end, OutputIterator output)
{
    if(it == end)
        throw bad_utf16_sequence_exception("Already at the end of the range!");

    char16_t codeunit = *(it++);
    if(is_utf16_surrogate(codeunit))
    {
        if(is_utf16_trail_surrogate(codeunit))
            throw bad_utf16_sequence_exception("Found a trail surrogate without previous lead surrogate!");

        // This is surely a lead surrogate
        if(it == end)
            throw bad_utf16_sequence_exception("End of range before an expected trail surrogate");

        char16_t trail = *(it++);
        if(!is_utf16_trail_surrogate(trail))
            throw bad_utf16_sequence_exception("Found a character that is not a surrogate after a lead surrogate!");

        char32_t codepoint = ((static_cast<char32_t>(codeunit & 0x3FF) << 10) | trail & 0x3FF) + 0x10000;
        output = codepoint_to_utf8(codepoint, output);
    }
    else
    {
        char32_t codepoint = static_cast<char32_t>(codeunit);
        output = codepoint_to_utf8(codepoint, output);
    }

    return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator utf16_to_utf8(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; )
    {
        output = utf16_character_to_utf8<InputIterator, OutputIterator>(it, end, output);
    }

    return output;
}

bool is_lead_octet(unsigned char octet);

std::size_t get_lead_octet_sequence_length(unsigned char octet);

bool is_trail_octet(unsigned char octet);

template<typename InputIterator>
int iterate_next_sequence(InputIterator & it, InputIterator end, unsigned char * output)
{
    if(it == end)
        throw bad_utf8_sequence_exception("Already at the end of the range!");

    unsigned char first_codeunit = *it;

    if(!is_valid_utf8_octet(first_codeunit))
        throw invalid_utf8_exception(std::to_string(first_codeunit) + " is not a valid UTF8 octet!");

    std::size_t sequence_length = get_lead_octet_sequence_length(first_codeunit);
    if(sequence_length == 0)
        throw bad_utf8_sequence_exception("Bad head of sequence octet!");

    output[0] = first_codeunit;
    for(std::size_t i = 1; i < sequence_length; ++i)
    {
        ++it;
        if(it == end || !is_trail_octet(*it))
            throw bad_utf8_sequence_exception("Not enough octets in a sequence: " + std::to_string(i) + " found but " + std::to_string(sequence_length) + " expected!");
        else if(!is_valid_utf8_octet(*it))
            throw invalid_utf8_exception(std::to_string(static_cast<unsigned char>(*it)) + " is not a valid UTF8 octet!");

        output[i] = *it;
    }

    ++it; // Last iteration to get past the last codeunit of the sequence

    return sequence_length;
}

template<typename InputIterator>
char32_t iterate_next(InputIterator & it, InputIterator end)
{
    unsigned char buffer[4];
    std::size_t sequence_length = iterate_next_sequence(it, end, buffer);

    unsigned char first_codeunit_mask;
    if(sequence_length == 1)
        first_codeunit_mask = 0x7F;
    else if(sequence_length == 2)
        first_codeunit_mask = 0x1F;
    else if(sequence_length == 3)
        first_codeunit_mask = 0xF;
    else if(sequence_length == 4)
        first_codeunit_mask = 0x7;

    char32_t codepoint = (buffer[0] & first_codeunit_mask);
    for(std::size_t i = 1; i < sequence_length; ++i)
    {
        codepoint = codepoint << 6;
        codepoint |= buffer[i] & 0x3F;
    }

    if(!is_valid_codepoint(codepoint))
        throw invalid_codepoint_exception("This is an invalid codepoint: " + std::to_string(codepoint));

    return codepoint;
}

template<typename InputIterator>
void iterate_previous(InputIterator & it, InputIterator begin)
{
    if(it == begin)
        throw bad_utf8_sequence_exception("Already at the beginning of the range!");

    --it;
    std::size_t measured_sequence_length = 1;
    while(!is_lead_octet(*it) && it != begin)
    {
        if(!is_valid_utf8_octet(*it))
            throw invalid_utf8_exception(std::to_string(static_cast<unsigned char>(*it)) + " is not a valid UTF8 octet!");
        else if(!is_trail_octet(*it))
            throw bad_utf8_sequence_exception("Found an octet that is not a lead or a trail octet!");

        ++measured_sequence_length;
        --it;
    }

    if(!is_lead_octet(*it)) // It means we stopped because we reached the beginning without finding a lead octet
        throw bad_utf8_sequence_exception("Can't find a lead octet until the beginning of the range!");
}

template<typename InputIterator, typename OutputIterator>
OutputIterator utf8_to_utf32(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; )
    {
        *(output++) = iterate_next(it, end);
    }

    return output;
}

template<typename OutputIterator>
OutputIterator codepoint_to_utf16(char32_t codepoint, OutputIterator output)
{
    if(codepoint < 0x10000)
    {
        *(output++) = static_cast<char16_t>(codepoint);
    }
    else
    {
        char32_t u = codepoint - 0x10000;
        *(output++) = 0xD800 | (static_cast<char16_t>(u >> 10) & 0x3FF);
        *(output++) = 0xDC00 | (static_cast<char16_t>(u) & 0x3FF);
    }

    return output;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator utf8_to_utf16(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; )
    {
        char32_t codepoint = iterate_next(it, end);
        output = codepoint_to_utf16(codepoint, output);
    }

    return output;
}

template<typename InputIterator>
bool is_valid_utf8(InputIterator begin, InputIterator end)
{
    unsigned char buffer[4];
    for(auto it = begin; it != end; )
    {
        try
        {
            iterate_next(it, end);
        }
        catch(...) // Some kind of exception was thrown when interpreting the UTF-8, so it's invalid
        {
            return false;
        }
    }

    return true;
}

}

#endif

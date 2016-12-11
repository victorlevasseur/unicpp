#ifndef UNICPP_UTF8TOOLS_H
#define UNICPP_UTF8TOOLS_H

#include "Exceptions.hpp"

namespace unicpp
{

bool is_valid_utf8_octet(unsigned char octet);

/**
    * Convert a utf32 codeunit (representing a single codepoint) to 
    * a sequence of utf8 code units representing the same codepoint.
    *
    * *(OutputIterator) must accept a char assignment.
    */
template<typename OutputIterator>
void utf32_to_utf8(char32_t codepoint, OutputIterator output)
{
    if(codepoint <= 0x0000007F) 
    {
        // Between 00000000 00000000 00000000 00000000 and 00000000 00000000 00000000 01111111
        // Will be encoded to 0xxxxxxx (1 byte) with 
        // all the 'x' been the 7 less sign. bits in the same order
        output++ = codepoint & 0x7F;
    }
    else if(codepoint <= 0x000007FF)
    {
        // Between 00000000 00000000 00000000 10000000 and 00000000 00000000 00000111 11111111
        // Will be encoded to 110xxxxx 10xxxxxx (2 bytes) with 
        // all the 'x' been the 11 less sign. bits in the same order
        output++ = 0xC0 | (codepoint >> 6) & 0x1F;
        output++ = 0x80 | codepoint & 0x3F;
    }
    else if(codepoint <= 0x0000FFFF)
    {
        // Between 00000000 00000000 00001000 00000000 and 00000000 00000000 11111111 11111111
        // Will be encoded to 1110xxxx 10xxxxxx 10xxxxxx (3 bytes) with 
        // all the 'x' been the 11 less sign. bits in the same order

        output++ = 0xE0 | (codepoint >> 12) & 0xF;
        output++ = 0x80 | (codepoint >> 6) & 0x3F;
        output++ = 0x80 | codepoint & 0x3F;
    }
    else if(codepoint <= 0x0010FFFF)
    {
        // Between 00000000 00000001 00000000 00000000 and 00000000 00010000 11111111 11111111
        // Will be encoded to 1110xxxx 10xxxxxx 10xxxxxx (3 bytes) with 
        // all the 'x' been the 11 less sign. bits in the same order

        output++ = 0xF0 | (codepoint >> 18) & 0x7;
        output++ = 0x80 | (codepoint >> 12) & 0x3F;
        output++ = 0x80 | (codepoint >> 6) & 0x3F;
        output++ = 0x80 | codepoint & 0x3F;
    }
    else
    {
        // Don't know how to encode this.
        throw not_encodable_codepoint_exception("Can't encode this codepoint: " + std::to_string(static_cast<uint32_t>(codepoint)));
    }
}

/**
    * Convert a sequence of utf32 codeunits (codepoints) to
    * a sequence of utf8 code units representing the same codepoints.
    *
    * *(InputIterator) must return a type convertible to char32_t
    * *(OutputIterator) must accept a char assignment.
    */
template<typename InputIterator, typename OutputIterator>
void utf32_to_utf8(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; ++it)
    {
        utf32_to_utf8<OutputIterator>(*it, output);
    }
}

template<typename OutputIterator>
void utf16_to_utf8(char16_t codepoint, OutputIterator output)
{

}

template<typename InputIterator, typename OutputIterator>
void utf16_to_utf8(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; ++it)
    {
        utf16_to_utf8<OutputIterator>(*it, output);
    }
}

template<typename InputIterator>
int iterate_next_sequence(InputIterator & it, InputIterator end, unsigned char* output)
{
    if(it == end)
        throw bad_utf8_sequence_exception("Already at the end of the range!");
    
    unsigned char first_codeunit = *it;

    if(!is_valid_utf8_octet(first_codeunit))
        throw invalid_utf8_exception(std::to_string(first_codeunit) + " is not a valid UTF8 octet!");

    std::size_t sequence_length;
    if(first_codeunit <= 127) // This is a 1 byte sequence
        sequence_length = 1;
    else if((first_codeunit >> 5) == 0x6) // This is a 2 bytes sequence
        sequence_length = 2;
    else if((first_codeunit >> 4) == 0xE) // This is a 3 bytes sequence
        sequence_length = 3;
    else if((first_codeunit >> 3) == 0x1E) // This is a 4 bytes sequence
        sequence_length = 4;
    else
        throw bad_utf8_sequence_exception("Bad head of sequence octet!");

    output[0] = first_codeunit;
    for(std::size_t i = 1; i < sequence_length; ++i)
    {
        ++it;
        if(it == end || (static_cast<unsigned char>(*it) >> 6) != 0x2)
            throw bad_utf8_sequence_exception("Not enough octets in a sequence: " + std::to_string(i) + " found but " + std::to_string(sequence_length) + " expected!");
        else if(!is_valid_utf8_octet(*it))
            throw invalid_utf8_exception(std::to_string(static_cast<unsigned char>(*it)) + " is not a valid UTF8 octet!");

        output[i] = *it;
    }

    ++it; // Last iteration to get past the last codeunit of the sequence

    return sequence_length;
}

template<typename InputIterator>
char32_t iterate_next(InputIterator& it, InputIterator end)
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

    return codepoint;
}

template<typename InputIterator>
char32_t get_next(InputIterator it, InputIterator end)
{
    return iterate_next(it, end);
}

template<typename InputIterator, typename OutputIterator>
bool utf8_to_utf32(InputIterator begin, InputIterator end, OutputIterator output)
{
    for(auto it = begin; it != end; )
    {
        output++ = iterate_next(it, end);
    }
}

template<typename InputIterator>
bool is_valid_utf8(InputIterator begin, InputIterator end)
{
    unsigned char buffer[4];
    for(auto it = begin; it != end; )
    {
        try
        {
            std::size_t sequence_length = iterate_next_sequence(it, end, buffer);

            for(std::size_t i = 0; i < sequence_length; ++i)
            {
                if(!is_valid_utf8_octet(buffer[i]))
                    return false;
            }
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
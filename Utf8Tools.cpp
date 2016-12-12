#include "Utf8Tools.hpp"

namespace unicpp
{

bool is_valid_utf8_octet(unsigned char octet)
{
    return octet != 0xC0 && octet != 0xC1 && octet != 0xF5 && octet != 0xFF;
}

bool is_valid_codepoint(char32_t codepoint)
{
    return codepoint <= CODE_POINT_MAX &&
        !is_utf16_surrogate(static_cast<char16_t>(codepoint)) &&
        codepoint != 0xffff &&
        codepoint != 0xfffe;
}

bool is_utf16_lead_surrogate(char16_t codeunit)
{
    return codeunit >= LEAD_SURROGATE_MIN && codeunit <= LEAD_SURROGATE_MAX;
}

bool is_utf16_trail_surrogate(char16_t codeunit)
{
    return codeunit >= TRAIL_SURROGATE_MIN && codeunit <= TRAIL_SURROGATE_MAX;
}

bool is_utf16_surrogate(char16_t codeunit)
{
    return is_utf16_lead_surrogate(codeunit) || is_utf16_trail_surrogate(codeunit);
}

bool is_lead_octet(unsigned char octet)
{
    return get_lead_octet_sequence_length(octet) != 0;
}

std::size_t get_lead_octet_sequence_length(unsigned char octet)
{
    if(octet <= 127) // This is a 1 byte sequence
        return 1;
    else if((octet >> 5) == 0x6) // This is a 2 bytes sequence
        return 2;
    else if((octet >> 4) == 0xE) // This is a 3 bytes sequence
        return 3;
    else if((octet >> 3) == 0x1E) // This is a 4 bytes sequence
        return 4;
    else
        return 0;
}

bool is_trail_octet(unsigned char octet)
{
    return ((octet >> 6) == 0x2);
}

}
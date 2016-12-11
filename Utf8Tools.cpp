#include "Utf8Tools.hpp"

namespace unicpp
{

bool is_valid_utf8_octet(unsigned char octet)
{
    return octet != 0xC0 && octet != 0xC1 && octet != 0xF5 && octet != 0xFF;
}

}
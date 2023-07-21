#pragma once
#include <cstdint>

union AccessType
{
    uint8_t value;
    struct
    {
        uint8_t r : 1;
        uint8_t w : 1;
        uint8_t constant : 1;
    } bits;
};

union TPDOCobId
{
    uint32_t value;
    struct
    {
        uint32_t canId : 29;
        uint32_t frame : 1;
        uint32_t rtr : 1;
        uint32_t valid : 1;
    } bits;
};
#pragma once
#include "frame.hpp"

union SDO_CommandByte
{
    uint8_t value;
    struct
    {
        uint8_t s : 1;
        uint8_t e : 1;
        uint8_t n : 2;
        uint8_t reserved : 1;
        uint8_t ccs : 3;
    } bits_initiate;
    struct
    {
        uint8_t c : 1;
        uint8_t n : 3;
        uint8_t t : 1;
        uint8_t ccs : 3;
    } bits_segment;
};

enum SDOServerStates
{
    SDOServerStates_Ready,
    SDOServerStates_Uploading,
    SDOServerStates_Downloading
};

class CANopen_SDO
{
private:
    class CANopen_Node &node;
    SDOServerStates serverState;
    struct
    {
        uint16_t index;
        uint8_t subIndex;
        uint32_t objectSize;
        uint32_t remainingBytes;
        uint8_t *dataSrc;
        uint8_t buffer[64];
    } transferData;

    void sendAbort(uint16_t index, uint8_t subIndex, uint32_t error);
    void uploadInitiate(CANopen_Frame request);
    void uploadSegment(CANopen_Frame request);
    void downloadInitiate(CANopen_Frame request);
    void downloadSegment(CANopen_Frame request);

public:
    CANopen_SDO(class CANopen_Node &node);
    void receiveFrame(CANopen_Frame frame);
};

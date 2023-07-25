#pragma once
#include <cstdint>
#include "enums.hpp"
#include "frame.hpp"
#define HB_OBJECT_1017 0x1017

class CANopen_HB
{
private:
    class CANopen_Node &node;
    uint32_t lastPublish;
    uint16_t *producerHeartbeatTime;

public:
    CANopen_HB(class CANopen_Node &node);
    void receiveFrame(CANopen_Frame frame);
    void publishState(NMTStates state);
    void update(uint32_t timestamp_us);
};

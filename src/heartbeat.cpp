/******************************************************************************
 * [Filename]:      heartbeat.cpp
 * [Project]:       CANopen
 * [Author]:        Tristan Lieberherr
 * [Date]:          August 2023
 * [Description]:   Contains the definitions of the HB class.
 *****************************************************************************/
#include "heartbeat.hpp"
#include "node.hpp"
#include "frame.hpp"
using namespace CANopen;

HB::HB(Node &node) : node(node) {}

void HB::publishState(NMTStates state)
{
    HeartbeatFrame frame(node.nodeId, state);
    node.sendFrame(frame);
    lastPublish = node.getTime_us();
}

void HB::update(uint32_t timestamp_us)
{
#ifdef OD_OBJECT_1017
    uint16_t heartbeatTime_ms = 0;
    node._od.at(OD_OBJECT_1017)->getValue(0, &heartbeatTime_ms);
    uint32_t heartbeatTime_us = (uint32_t)heartbeatTime_ms * 1000;
    if (heartbeatTime_ms > 0 && timestamp_us - lastPublish >= heartbeatTime_us)
        publishState(node._nmt.getState());
#endif
}

#include "node.hpp"

CANopen_Node::CANopen_Node(uint8_t id) : nodeId(id), receptionTimestamp(0), nmt(*this), hb(*this), sdo(*this)
{
}

void CANopen_Node::receiveFrame(CANopen_Frame frame, uint64_t timestamp_us)
{
    receptionTimestamp = timestamp_us;
    switch (frame.functionCode)
    {
    case FunctionCode_NMT:
        nmt.receiveFrame(frame);
        break;
    case FunctionCode_SYNC: // Also FunctionCode_EMCY
        break;
    case FunctionCode_TIME:
        break;
    case FunctionCode_TPDO1:
    case FunctionCode_TPDO2:
    case FunctionCode_TPDO3:
    case FunctionCode_TPDO4:
        break;
    case FunctionCode_RPDO1:
    case FunctionCode_RPDO2:
    case FunctionCode_RPDO3:
    case FunctionCode_RPDO4:
        break;
    case FunctionCode_TSDO:
    case FunctionCode_RSDO:
        sdo.receiveFrame(frame);
        break;
    case FunctionCode_HEARTBEAT:
        break;
    }
}

// void CANopen_Node::sendFrame(CANopen_Frame frame)
// {
// }

void CANopen_Node::update(uint64_t timer_us)
{
    nmt.update();
    sdo.update(timer_us);
}

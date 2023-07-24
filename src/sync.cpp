#include "sync.hpp"
#include "node.hpp"
#include <cstdio>

CANopen_SYNC::CANopen_SYNC(CANopen_Node &node) : node(node)
{
}

void CANopen_SYNC::receiveFrame(CANopen_Frame frame, uint32_t timestamp_us)
{
    uint8_t counter = frame.dlc > 0 ? frame.data[0] : 0;
    node.pdo.onSync(counter, timestamp_us);
}

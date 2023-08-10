#include "sync.hpp"
#include "node.hpp"
#include "frame.hpp"
#include "enums.hpp"
#include "objects/object_1019.hpp"
using namespace CANopen;

SYNC::SYNC(Node &node) : node(node)
{
#ifdef OD_OBJECT_1019
    uint8_t tmp;
    node.at(OD_OBJECT_1019)->getValue(0, &tmp);
    maxCounter = tmp < X1019_MIN_COUNTER ? X1019_MAX_COUNTER : tmp;
#else
    maxCounter = X1019_MAX_COUNTER;
#endif
}

void SYNC::enable() { enabled = true; }

void SYNC::disable() { enabled = false; }

void SYNC::receiveFrame(Frame frame, uint32_t timestamp_us)
{
    if (!enabled || frame.nodeId != 0)
        return;
    internalCounter = frame.dlc > 0 ? frame.data[0] : internalCounter % maxCounter + 1;
    node.pdo.onSync(internalCounter, timestamp_us);
}

#include "pdo.hpp"
#include "node.hpp"
#include "enums.hpp"
#include "objects/object_1800.hpp"
#include <cstring>
#include <cstdio>

CANopen_PDO::CANopen_PDO(CANopen_Node &node) : node(node)
{
    for (unsigned i = 0; i < OD_TPDO_COUNT; i++)
        initTPDO(i);
}

void CANopen_PDO::initTPDO(unsigned index)
{
    tpdos[index].commObject = (TPDOCommunicationObject *)node.od.findObject(TPDO_COMMUNICATION_INDEX + index);
    tpdos[index].mapObject = (TPDOMappingObject *)node.od.findObject(TPDO_MAPPING_INDEX + index);
    remapTPDO(index);
}

void CANopen_PDO::remapTPDO(unsigned index)
{
    TPDO *tpdo = tpdos + index;
    unsigned count = tpdo->mapObject->getCount();
    tpdo->count = 0;
    if (tpdo->mappedEntries != NULL)
        delete[] tpdo->mappedEntries;
    tpdo->mappedEntries = new ObjectEntry *[count]; // TODO: check for NULL?
    printf("[PDO] TPDO[%d] was remapped\n", index + 1);
    unsigned sizeSum = 0;
    for (unsigned i = 0; i < count; i++)
    {
        TPDOMapEntry content = {tpdo->mapObject->getMappedValue(i)};
        ObjectEntry *entry = (ObjectEntry *)node.od.findObject(content.bits.index)->entries + content.bits.subindex;
        sizeSum += entry->size;
        if (sizeSum > 8) // TODO
        {
            printf("Exceeded PDO size! Skipping any other mapping (%d/%d)\n", tpdo->count, count);
            break;
        }
        tpdo->mappedEntries[i] = entry;
        tpdo->count++;
        printf("index: 0x%04X, sub-index: %d, size(bits): %d\n", content.bits.index, content.bits.subindex, content.bits.length);
    }
    puts("");
}

void CANopen_PDO::bufferizeTPDO(unsigned index, uint8_t *buffer)
{
    TPDO *tpdo = tpdos + index;
    unsigned bytesTransferred = 0;
    for (unsigned i = 0; i < tpdo->count; i++)
    {
        ObjectEntry *entry = tpdo->mappedEntries[i];
        entry->readBytes(buffer, entry->size, bytesTransferred);
        bytesTransferred += entry->size;
    }
}

void CANopen_PDO::sendTPDO(unsigned index, uint32_t timestamp_us)
{
    TPDOCobidEntry cobid = {tpdos[index].commObject->getCobId()};
    CANopen_Frame frame;
    frame.functionCode = (cobid.bits.canId >> 7) & 0x0F;
    frame.nodeId = cobid.bits.canId & 0x7F;
    frame.dlc = 8;
    bufferizeTPDO(index, frame.data);
    node.sendFrame(frame);
    tpdos[index].timestamp_us = timestamp_us;
}

void CANopen_PDO::update(uint32_t timestamp_us)
{
    NMTStates state = node.nmt.getState();
    for (unsigned i = 0; i < OD_TPDO_COUNT; i++)
    {
        TPDO *tpdo = tpdos + i;
        if (state == NMTState_PreOperational && tpdo->commObject->getEnableFlag())
        {
            tpdo->commObject->clearEnableFlag();
            remapTPDO(i);
        }
        uint8_t transmission = tpdo->commObject->getTransmissionType();
        uint16_t timer_ms = tpdo->commObject->getEventTimer();
        // Only event-driven PDOs can be sent periodically
        // SKip if not in NMT Operational OR PDO is disabled OR transmission type not event-driven OR event-timer unsupported OR event timer disabled OR event timer not reached yet
        if (state != NMTState_Operational || !tpdo->commObject->isEnabled() || (transmission != X1800_EVENT1 && transmission != X1800_EVENT2) || !tpdo->commObject->isTimerSupported() || timer_ms == 0 || timestamp_us - tpdo->timestamp_us < timer_ms * 1000)
            continue;
        sendTPDO(i, timestamp_us);
    }
}

void CANopen_PDO::receiveFrame(CANopen_Frame frame)
{
    if (node.nmt.getState() != NMTState_Operational)
        return;
}

void CANopen_PDO::onSync(uint8_t counter, uint32_t timestamp_us)
{
    if (node.nmt.getState() != NMTState_Operational)
        return;
    for (unsigned i = 0; i < OD_TPDO_COUNT; i++)
    {
        TPDO *tpdo = tpdos + i;
        uint8_t transmission = tpdo->commObject->getTransmissionType();
        if (transmission == 0) // acyclic
        {
            // TODO: code correct behaviour
            sendTPDO(i, timestamp_us);
        }
        else if (transmission <= X1800_SYNC_MAX && counter && !(counter % transmission)) // cyclic
        {
            sendTPDO(i, timestamp_us);
        }
    }
}

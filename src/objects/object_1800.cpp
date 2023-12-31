/******************************************************************************
 * [Filename]:      object_1800.cpp
 * [Project]:       CANopen
 * [Author]:        Tristan Lieberherr
 * [Date]:          August 2023
 * [Description]:   Contains the definitions of the "RPDO mapping parameter" Object1800 class.
 *****************************************************************************/
#include "object_1800.hpp"
#include "../frame.hpp"
#include "../node.hpp"
#include <cstring>
using namespace CANopen;

SDOAbortCodes Object1800::preReadBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, uint32_t offset)
{
    if (subindex == X1800_INDEX_RESERVED)
        return SDOAbortCode_SubindexNonExistent;
    return SDOAbortCode_OK;
}

SDOAbortCodes Object1800::preWriteBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, Node &node)
{
    bool enabled = isEnabled();
    switch (subindex)
    {
    case X1800_INDEX_COBID:
    {
        PDOCobidEntry current = {getCobId()};
        PDOCobidEntry recent = {*(uint32_t *)bytes};
        // Check if bits 0 to 30 are modified
        if (enabled && (current.value ^ recent.value) & ~X1800_COBID_VALID_MASK)
            return SDOAbortCode_InvalidDownloadParameterValue;
        // If a PDO was enabled
        if (current.bits.valid && !recent.bits.valid)
            remap = true;
        break;
    }
    case X1800_INDEX_TRANSMISSION:
    {
        uint8_t value = bytes[0];
        if (X1800_SYNC_MAX < value && value < X1800_RTR_SYNC)
            return SDOAbortCode_InvalidDownloadParameterValue;
        break;
    }
    case X1800_INDEX_INHIBIT:
    {
        if (enabled)
            return SDOAbortCode_UnsupportedObjectAccess;
        break;
    }
    case X1800_INDEX_RESERVED:
        return SDOAbortCode_SubindexNonExistent;
    case X1800_INDEX_EVENT:
        break;
    case X1800_INDEX_SYNC:
    {
        if (enabled)
            return SDOAbortCode_UnsupportedObjectAccess;
        break;
    }
    }
    return SDOAbortCode_OK;
}

void Object1800::postWriteBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, Node &node)
{
    if (remap)
    {
        node.pdo().reloadTPDO();
        remap = false;
    }
}

uint32_t Object1800::getCobId()
{
    uint32_t value;
    getValue(X1800_INDEX_COBID, &value);
    return value;
}

uint16_t Object1800::getActualCobId() { return getCobId() & COBID_MASK; }

uint8_t Object1800::getTransmissionType()
{
    uint8_t value;
    getValue(X1800_INDEX_TRANSMISSION, &value);
    return value;
}

uint32_t Object1800::getInhibitTime_us()
{
    uint16_t value;
    getValue(X1800_INDEX_INHIBIT, &value);
    return (uint32_t)value * 100;
}

uint16_t Object1800::getEventTimer_ms()
{
    uint16_t value;
    getValue(X1800_INDEX_EVENT, &value);
    return value;
}

uint32_t Object1800::getEventTimer_us() { return (uint32_t)getEventTimer_ms() * 1000; }

uint8_t Object1800::getSyncStart()
{
    uint8_t value;
    getValue(X1800_INDEX_SYNC, &value);
    return value;
}

bool Object1800::isEnabled() { return ~getCobId() & X1800_COBID_VALID_MASK; }

bool Object1800::isInhibitSupported() { return getCount() >= X1800_INDEX_INHIBIT; }

bool Object1800::isTimerSupported() { return getCount() >= X1800_INDEX_EVENT; }

bool Object1800::isSynchronous()
{
    uint8_t t = getTransmissionType();
    return t <= X1800_SYNC_MAX || t == X1800_RTR_SYNC;
}

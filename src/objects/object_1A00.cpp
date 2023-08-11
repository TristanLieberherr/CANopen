#include "object_1A00.hpp"
#include "../node.hpp"
using namespace CANopen;

SDOAbortCodes Object1A00::preWriteBytes(uint8_t subindex, uint8_t *bytes, unsigned size, Node &node)
{
    if (subindex == X1A00_INDEX_COUNT)
    {
        uint8_t value = bytes[0];
        if (value > X1A00_MAX_ENTRIES)
            return SDOAbortCode_DownloadValueTooHigh;
        if (value > 0)
        {
            unsigned sizeSum = 0;
            for (unsigned i = 0; i < value; i++)
            {
                TPDOMapEntry entry = {getMappedValue(i)};
                sizeSum += node.findObject(entry.bits.index)->getSize(entry.bits.subindex);
            }
            if (sizeSum > PDO_DLC)
                return SDOAbortCode_MappedPDOLengthExceeded;
        }
    }
    else
    {
        TPDOMapEntry entry = {*(uint32_t *)bytes};
        Object *object = node.findObject(entry.bits.index);
        if (!object || !object->isSubValid(entry.bits.subindex))
            return SDOAbortCode_ObjectNonExistent;
        AccessType access = object->getAccessType(entry.bits.subindex);
        if (entry.bits.length != ((uint32_t)object->getSize(entry.bits.subindex)) * 8 || !access.bits.mappable || !access.bits.readable)
            return SDOAbortCode_CannotMapToPDO;
    }
    return SDOAbortCode_OK;
}

uint8_t Object1A00::getCount() { return *(uint8_t *)entries[X1A00_INDEX_COUNT].dataSrc; }

uint32_t Object1A00::getMappedValue(uint8_t index) { return *(uint32_t *)entries[index + 1].dataSrc; }

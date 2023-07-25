#include "object_1A00.hpp"
#include "node.hpp"
#include <cstring>
#include <cstdio>

uint8_t TPDOMappingObject::getCount() { return *(uint8_t *)entries[X1A00_INDEX_COUNT].dataSrc; }

uint32_t TPDOMappingObject::getMappedValue(uint8_t index) { return *(uint32_t *)entries[index + 1].dataSrc; }

SDOAbortCodes TPDOMappingObject::writeBytes(uint8_t subindex, uint8_t *bytes, unsigned size, CANopen_Node &node)
{
    if (!isSubValid(subindex))
        return SDOAbortCode_SubindexNonExistent;
    ObjectEntry entry = entries[subindex];
    if (!entry.accessType.bits.w)
        return SDOAbortCode_AttemptWriteOnReadOnly;
    if (size != entry.size)
        return SDOAbortCode_DataTypeMismatch_LengthParameterMismatch;
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
            if (sizeSum > PDO_DATA_LENGTH)
                return SDOAbortCode_MappedPDOLengthExceeded;
        }
    }
    else
    {
        TPDOMapEntry entry = {*(uint32_t *)bytes};
        Object *object = node.findObject(entry.bits.index);
        if (object == NULL || !object->isSubValid(entry.bits.subindex))
            return SDOAbortCode_ObjectNonExistent;
        if (entry.bits.length != ((uint32_t)object->getSize(entry.bits.subindex)) * 8)
            return SDOAbortCode_CannotMapToPDO;
    }
    memcpy((void *)entry.dataSrc, bytes, size);
    return SDOAbortCode_OK;
}

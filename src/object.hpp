#pragma once
#include "unions.hpp"
#include "enums.hpp"
#include <cstdint>
#define OBJECT_INDEX_COUNT 0

namespace CANopen
{
    struct ObjectEntry
    {
        const void *dataSrc;
        const AccessType accessType;
        const uint32_t size;

        ObjectEntry(void *dataSrc, uint8_t accessType, uint32_t size) : dataSrc(dataSrc), accessType{accessType}, size(size) {}
    };

    class Object
    {
    protected:
        const ObjectEntry *entries;

        virtual SDOAbortCodes preReadBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, uint32_t offset);
        virtual void postReadBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, uint32_t offset);
        virtual SDOAbortCodes preWriteBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, class Node &node);
        virtual void postWriteBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, class Node &node);

    public:
        const uint16_t index;
        const uint8_t subNumber;

        Object(uint16_t index, uint8_t subNumber, ObjectEntry *entries) : entries(entries), index(index), subNumber(subNumber) {}
        virtual ~Object() {}
        bool isSubValid(uint8_t subindex);
        uint32_t getSize(uint8_t subindex);
        AccessType getAccessType(uint8_t subindex);
        // Methods called mainly by SDO
        SDOAbortCodes readBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, uint32_t offset);
        SDOAbortCodes writeBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, class Node &node);
        // Methods called mainly by application
        uint8_t getCount();
        template <typename T>
        bool getValue(uint8_t subindex, T *value)
        {
            if (!isSubValid(subindex) || sizeof(T) != entries[subindex].size)
                return false;
            *value = *(T *)entries[subindex].dataSrc;
            return true;
        }
        template <typename T>
        bool setValue(uint8_t subindex, T value)
        {
            if (!isSubValid(subindex) || sizeof(T) != entries[subindex].size)
                return false;
            *(T *)entries[subindex].dataSrc = value;
            return true;
        }
    };
}

#pragma once
#include <cstdint>

struct ObjectEntry
{
    const void *dataSrc;
    const uint8_t accessType;
    const uint8_t dataType;
    const uint8_t size;

    ObjectEntry(void *dataSrc, uint8_t accessType, uint8_t dataType, uint8_t size) : dataSrc(dataSrc), accessType(accessType), dataType(dataType), size(size) {}
};

struct Object
{
    const uint16_t index;
    const uint8_t subNumber;
    const uint16_t objectType;
    const ObjectEntry *entries;

    Object(uint16_t index, uint8_t subNumber, uint16_t objectType, ObjectEntry entries[]) : index(index), subNumber(subNumber), objectType(objectType), entries(entries) {}
    virtual bool writeBytes(uint8_t subindex, uint8_t bytes[], unsigned size);
};

class IObjectDictionnary
{
public:
    virtual Object *findObject(uint16_t index) = 0;
    virtual void saveData() = 0;
    virtual void loadData() = 0;
};

struct TPDOCommunicationObject : public Object
{
};

struct TPDOMappingObject : public Object
{
};

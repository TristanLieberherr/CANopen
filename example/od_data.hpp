#pragma once
#include <cstdint>

#define OD_OBJECTS_COUNT 1

struct OD_Data // Auto generated
{
    uint32_t x1005;
    uint32_t x1006;
    uint32_t x1007[2];
    struct
    {
        uint8_t sub0;
        uint32_t sub1;
        uint32_t sub2;
    } x1008;

    OD_Data() : x1005(80),
                x1006(6),
                x1007{10, 11},
                x1008{.sub0 = 3, .sub1 = 1, .sub2 = 2} {}

    void print();
    void save();
    void load();
};

struct OD_Object
{
    const int accessType;
    const int dataType;
    const void *valueSrc;

    OD_Object(int accessType, int dataType, void *valueSrc) : accessType(accessType), dataType(dataType), valueSrc(valueSrc) {}
};

struct OD_Objects // Auto generated
{
    OD_Object obj_x1005[1];
    OD_Object obj_x1006[1];
    OD_Object obj_x1007[2];
    OD_Object obj_x1008[2];

    OD_Objects(OD_Data &data) : obj_x1005{OD_Object(1, 1, &data.x1005)},
                                obj_x1006{OD_Object(1, 1, &data.x1006)},
                                obj_x1007{OD_Object(1, 2, &data.x1007[0]), OD_Object(1, 2, &data.x1007[1])},
                                obj_x1008{OD_Object(1, 2, &data.x1008.sub0), OD_Object(1, 2, &data.x1008.sub1)}
    {
    }
};

struct OD_ObjectEntry
{
    const int index;
    const int objectType;
    const int subNumber;
    const OD_Object *objects;

    OD_ObjectEntry(int index, int objectType, int subNumber, OD_Object *objects) : index(index), objectType(objectType), subNumber(subNumber), objects(objects) {}
};

class OD_ObjectDictionnary // Auto generated
{
private:
    const int size;
    OD_Data data;
    OD_Objects objects;
    OD_ObjectEntry entries[OD_OBJECTS_COUNT];

public:
    OD_ObjectDictionnary() : size(OD_OBJECTS_COUNT), objects(data), entries{OD_ObjectEntry(1, 2, 3, objects.obj_x1005)}
    {
    }
    inline void save() { return data.save(); }
    inline void load() { return data.load(); }
};

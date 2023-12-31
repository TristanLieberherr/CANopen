/******************************************************************************
 * [Filename]:      object_1011.cpp
 * [Project]:       CANopen
 * [Author]:        Tristan Lieberherr
 * [Date]:          August 2023
 * [Description]:   Contains the definitions of the "Restore default parameters" Object1011 class.
 *****************************************************************************/
#include "object_1011.hpp"
#include "../node.hpp"
using namespace CANopen;

SDOAbortCodes CANopen::Object1011::preWriteBytes(uint8_t subindex, uint8_t *bytes, uint32_t size, Node &node)
{
    if (subindex > OBJECT_INDEX_COUNT)
    {
        uint32_t value = *(uint32_t *)bytes;
        if (value == X1011_LOAD_SIGNATURE)
            return node.od().restoreData(subindex) ? SDOAbortCode_CancelWrite : SDOAbortCode_AccessFailedHardwareError;
        else
            return SDOAbortCode_CannotStoreOrTransfer;
    }
    return SDOAbortCode_OK;
}

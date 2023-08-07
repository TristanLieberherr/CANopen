#include "object_1010.hpp"
#include "../node.hpp"
using namespace CANopen;

SDOAbortCodes CANopen::Object1010::preWriteBytes(uint8_t subindex, uint8_t *bytes, unsigned size, Node &node)
{
    if (subindex > X1010_INDEX_COUNT)
    {
        uint32_t value = *(uint32_t *)bytes;
        if (value == X1010_SAVE_SIGNATURE)
            return node.saveOD(subindex) ? SDOAbortCode_CancelWrite : SDOAbortCode_AccessFailedHardwareError;
        else
            return SDOAbortCode_CannotStoreOrTransfer;
    }
    return SDOAbortCode_OK;
}
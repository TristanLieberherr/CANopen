#pragma once
#include "frame.hpp"
#include "nmt.hpp"
#include "heartbeat.hpp"
#include "sdo.hpp"
#include "od.hpp"

class CANopen_Node
{
private:
    int nodeId;
    OD_ObjectDictionnary od;
    CANopen_NMT nmt;
    CANopen_HB hb;
    CANopen_SDO sdo;

public:
    friend CANopen_NMT;
    friend CANopen_HB;
    friend CANopen_SDO;
    CANopen_Node(int id);
    void receiveFrame(CANopen_Frame frame);
    void sendFrame(CANopen_Frame frame);
    void update();
};

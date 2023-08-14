#include "node.hpp"
#include "frame.hpp"
using namespace CANopen;

Node::Node(uint8_t id) : _nmt(*this), _hb(*this), _sdo(*this), _pdo(*this), _sync(*this), _emcy(*this), nodeId(id)
{
    _nmt.initSM();
}

ObjectDictionnary &Node::od() { return _od; }

NMT &CANopen::Node::nmt() { return _nmt; }

HB &CANopen::Node::hb() { return _hb; }

SDO &CANopen::Node::sdo() { return _sdo; }

PDO &CANopen::Node::pdo() { return _pdo; }

SYNC &CANopen::Node::sync() { return _sync; }

EMCY &CANopen::Node::emcy() { return _emcy; }

void Node::receiveFrame(Frame &frame)
{
    uint32_t timestamp = getTime_us();
    switch (frame.functionCode)
    {
    case FunctionCode_NMT:
        _nmt.receiveFrame((NMTFrame &)frame);
        break;
    case FunctionCode_SYNC:
        // Also FunctionCode_EMCY
        _sync.receiveFrame((SYNCFrame &)frame, timestamp);
        break;
    case FunctionCode_TPDO1:
    case FunctionCode_TPDO2:
    case FunctionCode_TPDO3:
    case FunctionCode_TPDO4:
        _pdo.receiveTPDO(frame, timestamp);
        break;
    case FunctionCode_RPDO1:
    case FunctionCode_RPDO2:
    case FunctionCode_RPDO3:
    case FunctionCode_RPDO4:
        _pdo.receiveRPDO(frame, timestamp);
        break;
    case FunctionCode_RSDO:
        _sdo.receiveFrame((SDOFrame &)frame, timestamp);
        break;
    }
}

void Node::update()
{
    uint32_t timestamp = getTime_us();
    _hb.update(timestamp);
    _sdo.update(timestamp);
    _pdo.update(timestamp);
}

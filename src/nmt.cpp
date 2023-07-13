#include "nmt.hpp"
#include "node.hpp"
#include <cstdio>

CANopen_NMT::CANopen_NMT(CANopen_Node &node) : currentState(NMTState_Initialisation), node(node)
{
}

void CANopen_NMT::receiveFrame(CANopen_Frame frame)
{
    if (frame.functionCode != FunctionCode_NMT || (frame.data[1] != node.nodeId && frame.data[1] != 0))
        return;
    setTransition((NMTServiceCommands)frame.data[0]);
}

void CANopen_NMT::setTransition(NMTServiceCommands command)
{
    NMTStates nextState = currentState;
    if (currentState == NMTState_Initialisation)
    {
        puts("entered Initialisation state, soing to Pre Op");
        nextState = NMTState_PreOperational;
    }
    else if (currentState == NMTState_PreOperational)
    {
        if (command == NMTServiceCommands_Start)
        {
            nextState = NMTState_Operational;
        }
        else if (command == NMTServiceCommands_Stop)
        {
            nextState = NMTState_Stopped;
        }
        else if (command == NMTServiceCommands_ResetNode || command == NMTServiceCommands_ResetCommunication)
        {
            nextState = NMTState_Initialisation;
        }
    }
    else if (currentState == NMTState_Operational)
    {
        if (command == NMTServiceCommands_EnterPreOperational)
        {
            nextState = NMTState_PreOperational;
        }
        else if (command == NMTServiceCommands_Stop)
        {
            nextState = NMTState_Stopped;
        }
        else if (command == NMTServiceCommands_ResetNode || command == NMTServiceCommands_ResetCommunication)
        {
            currentState = NMTState_Initialisation;
        }
    }
    else if (currentState == NMTState_Stopped)
    {
        if (command == NMTServiceCommands_EnterPreOperational)
        {
            nextState = NMTState_PreOperational;
        }
        else if (command == NMTServiceCommands_Start)
        {
            nextState = NMTState_Operational;
        }
        else if (command == NMTServiceCommands_ResetNode || command == NMTServiceCommands_ResetCommunication)
        {
            nextState = NMTState_Initialisation;
        }
    }
    // if (currentState == NMTState_Initialisation && nextState == NMTState_PreOperational)
    // {
    //     // TODO: boot-up protocol (p.77)
    //     CANopen_Frame frame;
    //     frame.nodeId = node.nodeId;
    //     frame.dlc = 1;
    //     frame.data[0] = NMTState_Initialisation;
    //     frame.functionCode = FunctionCode_HEARTBEAT;
    //     node.sendFrame(frame);
    // }
    if (currentState != nextState)
    {
        printf("[NMT] Node %d entered state 0x%02X\n\n", node.nodeId, nextState);
    }
    currentState = nextState;
}

void CANopen_NMT::update()
{
    setTransition((NMTServiceCommands)0);
}
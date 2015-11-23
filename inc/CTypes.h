//------------------------------------------------------------------------------
//  File        :   CTypes.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file containing application required types
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CTYPES_H
#define CTYPES_H

//Headers
#include <iostream>
#include <string>
#include <vector>
#include <queue>

//Defines
#define DEBUGPRINT 0

//TypeDefs
//Structure to capture the config file data
typedef struct 
{
    //Node Identification
    int m_iNodeNo;

    //Host Name of the Node
    std::string m_strHostName;

    //Port number
    int m_iPortNo;

    //The neighbour nodes of this particular node 
    std::vector<int> m_veciNodeNghbrs;

    //Total No of Nodes
    int m_iTotalNodes;

    //Root Node
    bool m_bRootNode;
    
} NODE_INFO_T;

//Structure to capture the config file data
typedef struct 
{
    //The neighbour nodes of this particular node 
    std::queue<int> m_qiBroadcastSource;

    //Total No of Nodes
    int m_iTotalBroadcastingNodes;
    
} BROADCAST_INFO_T;

//Message Type Enum
enum MSG_TYPE_E
{
    eMSG_INITIATE = 0,
    eMSG_REQ = 1,
    eMSG_RESP = 2,
    eMSG_POS_ACK = 3,
    eMSG_NEG_ACK = 4,
    eMSG_BROADCAST = 5,
    eMSG_BROADCAST_ACK = 6,
    eMSG_BROADCAST_INITIATE = 7,
    //-------------------------Project 2----------------------------------------
    eMSG_TRIGGER_APP_CS_REQ = 8,
    eMSG_TOKEN_REQ = 9,
    eMSG_TOKEN = 10,
};

//Structure to send message across the system
typedef struct 
{
    //The type of message
    MSG_TYPE_E m_eMsgType;

    //Source Node Identification
    int m_iSrcNodeNo;

    //Destination Node Identification
    int m_iDestNodeNo;

    //Host Name of the Node
    char m_arrchSrcHostName[10];

    //Port number
    int m_iSrcPortNo;

    //Total No of Nodes
    int m_iTotalNodes;

    //The neighbour nodes of this particular node 
    int m_arriNodeVisited[45];

    //Node Visited Counter
    int m_iVisitedNodes;
    
} MSG_INFO_T;

//------------------------------Project 2---------------------------------------
//Application Message Type Enum
enum APP_MSG_TYPE_E
{
    eAPP_MSG_INIT_CS_REQ = 0,
    eAPP_MSG_BROADCAST = 1,
    eAPP_MSG_BROADCAST_ACK = 2,
};

//struct to send application message across the system
typedef struct
{
    //The type of message
    APP_MSG_TYPE_E m_eMsgType;

    //Source Node Identification
    int m_iSrcNodeNo;

    //Destination Node Identification
    int m_iDestNodeNo;

    //Vector Clock Information to be piggybacked
    int m_arriNodeVectorClock[45];

    //Total No of Nodes
    int m_iTotalNodes;

} APP_MSG_INFO_T;
#endif
//----------------------------End of File---------------------------------------

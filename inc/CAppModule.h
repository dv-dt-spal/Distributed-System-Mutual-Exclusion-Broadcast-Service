//------------------------------------------------------------------------------
//  File        :   CAppModule.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for Node in the Distributed System
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef C_APP_MODULE_H
#define C_APP_MODULE_H

//Headers
#include "CParser.h"
#include "CSystemAPI.h"
#include "CAppMessageQ.h"
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

//Class Description
class CAppModule
{    
    public:
        
        //Constructor
        CAppModule();

        //Destructor
        ~CAppModule();

        //Initialize the Server
        bool Init(int p_iNodeIndex,CSystemAPI* p_pobjSystemAPI);

        //Close the Server
        bool Close();

        //App Module Thread Which keeps generating CS Request
        void AppCSReqGenerator();

        //App Message Handler
        void AppMessageHandler();

        //Process Message
        void ProcessMessage(APP_MSG_INFO_T p_stMsg);

        //Msg Handler Initiate
        void MsgHndlrAppMsgInitCSReq(APP_MSG_INFO_T p_stMsg);

        //Msg Handler App Message Broadcast
        void MsgHndlrAppMsgBroadcast(APP_MSG_INFO_T p_stMsg);

        //Msg Handler App Message Broadcast Ack
        void MsgHndlrAppMsgBroadcastAck(APP_MSG_INFO_T p_stMsg);

        //ClearBroadcast
        void ClearBroadcast();

        //GetTreeNighbours
        void GetTreeNeighbours();
               
    private:

        //Status
        bool m_bInitStatus;

        //System API Pointer
        CSystemAPI * m_pobjSystemAPI;
    
        //Vector Clock of this node
        std::vector<int> m_veciVectorClockNode;

        //Vector to Store the Tree Neighbours
        std::vector<int> m_veciTreeNeighbours;

        //Thread for creating the App message handler
        CThread m_objThreadAppMsg;

        //Thread for creating the node message handler
        CThread m_objThreadAppGenCSReq;

        //Parser Class object
        CParser m_objParser;

        //Node Index
        int m_iNodeIndex;

        //Total Nodes
        int m_iTotalNodes;

        //Total Number of CS Requests per Node
        int m_iTotalCSRequests;

        //Mean delay between two CS execution
        float m_fMeanDelayBetnCS;

        //Mean Duration of CS
        float m_fMeanDurationCS;

        //Vector to keep the all the nodes which this app node will be sending 
        //broadcast messages
        std::vector<int> m_veciBroadcastedNghbrs;

        //Node Parent Index
        int m_iBroacastParentNodeIndex;

        //Count of Broadcast ACks
        int m_iNoofBroadcastAcks;

        //bool for sending broadcast source
        volatile bool m_bBroadcastSource;

        //bool for showing app cs task completion
        volatile bool m_bIsCSTaskFinished;
};
#endif
//----------------------------End of File---------------------------------------

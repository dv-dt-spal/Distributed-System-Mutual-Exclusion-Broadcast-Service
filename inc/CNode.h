//------------------------------------------------------------------------------
//  File        :   CNode.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for Node in the Distributed System
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CNODE_H
#define CNODE_H

//Headers
#include "CParser.h"
#include "CNodeMessageQ.h"
#include "CTokenRequestQ.h"
#include "CClient.h"

//Class Description
class CNode
{    
    public:
        
        //Constructor
        CNode();

        //Destructor
        ~CNode();

        //Initialize the Server
        bool Init(int p_iNodeIndex);

        //Close the Server
        bool Close();

        //Check if root node
        bool CheckIfRoot();
            
        //Check if spanning tree created
        void CheckSpanningTreeCreated();

        //PrintTreeNeighbours
        void PrintTreeNeighbours();

        //Node Message Handler
        void NodeMessageHandler();

        //Process Message
        void ProcessMessage(MSG_INFO_T p_stMsg);

        //Msg Handler Initiate
        void MsgHndlrInitiate(MSG_INFO_T p_stMsg);

        //Msg Handler Request
        void MsgHndlrRequest(MSG_INFO_T p_stMsg);

        //Msg Handler Response
        void MsgHndlrResponse(MSG_INFO_T p_stMsg);

        //Msg Handler Positive Acknowledgement
        void MsgHndlrPositiveAck(MSG_INFO_T p_stMsg);

        //Msg Handler Negative Acknowledgement
        void MsgHndlrNegativeAck(MSG_INFO_T p_stMsg);

        //Msg Handler Broadcast
        void MsgHndlrBroadcast(MSG_INFO_T p_stMsg);

        //Msg Handler Broadcast Acknowledgement
        void MsgHndlrBroadcastAck(MSG_INFO_T p_stMsg);

        //Msg Handler Broadcast Initiate
        void MsgHndlrBroadcastInitiate(MSG_INFO_T p_stMsg);

        //InitiateBroadcast
        void InitiateBroadcast();

        //Set Broadcast
        void SetAsBroadcastSource();

        //ClearBroadcast
        void ClearBroadcast();

        //-------------------------Project 2------------------------------------

        //Msg Handler Trigger App CS Req
        void MsgHndlrTriggerAppCSReq(MSG_INFO_T p_stMsg);

        //Msg Handler Token Request
        void MsgHndlrTokenReq(MSG_INFO_T p_stMsg);

        //Msg Handler Token
        void MsgHndlrToken(MSG_INFO_T p_stMsg);

        //Process Mutual Exclusion Req
        void ProcessMutualExecReq();

        //API for System API to Request CS
        void CS_Enter();

        //API for System API to Notify CS Exit
        void CS_Exit();

        //Initiate Broadcast for app tp generate CS Requests
        void InitiateTriggerAppCSReq();

        //Node Message Handler
        void ProcessMutualExecReqHndlr();
               
    private:

        //Store the Node info of self
        NODE_INFO_T m_stnodeinfoSelf;

        //Store the Node info of self
        BROADCAST_INFO_T m_stBroadcastInfo;

        //Node Index
        int m_iNodeIndex;

        //Node Index
        int m_iDefaultNghbrsCount;
        
        //Count of ACks
        int m_iNoofAcks;

        //Count of Broadcast ACks
        int m_iNoofBroadcastAcks;

        //The new neighbour nodes of this particular node 
        //Updated when sending and receiving a positive acknowledgement message
        std::vector<int> m_veciNewNodeNghbrs;

        //Neigbours which sent negative acknowledgement
        std::vector<int> m_veciNotNodeNghbrs;

        //Vector to keep the all the nodes which this node will be sending 
        //request messages
        std::vector<int> m_veciRequestedNghbrs;

        //Vector to keep the all the nodes which this node will be sending 
        //broadcast messages
        std::vector<int> m_veciBroadcastedNghbrs;

        //Node Parent Index
        int m_iParentNodeIndex;

        //Node Parent Index
        int m_iBroacastParentNodeIndex;
        
        //Boolean variable to mark the visited nodes
        bool m_bNodeVisited;

        //Boolean variable to check if spanning tree created
        volatile bool m_bSpanningTreeCreated;

        //Status
        bool m_bInitStatus;

        //Thread for creating the node message handler
        CThread m_objThread;

        //Thread for creating the Process Mutual Exclusion handler
        CThread m_objThrdPrcssMtlExclsn;

        //Parser Class object
        CParser m_objParser;

        //bool for sending broadcast source
        bool m_bBroadcastSource;

        //-------------------------Project 2------------------------------------
        //Token Request Accepted for the Node
        volatile bool m_bNodeTokenReqAccepted;

        //Node executing CS status
        volatile bool m_bNodeInCS;

        //Node has token status
        volatile bool m_bNodeHasToken;

        //Node has requested token status
        volatile bool m_bNodeHasRequestedToken;

        //Pointer Node pointing towards the node having token
        int m_iPointerNode;

        //Bool variable to show status if request being processed
        volatile bool m_bIsNodeProcessingReq;

        //Node Being Processed
        volatile int m_iNodeBeingProcessed;
        
};
#endif
//----------------------------End of File---------------------------------------

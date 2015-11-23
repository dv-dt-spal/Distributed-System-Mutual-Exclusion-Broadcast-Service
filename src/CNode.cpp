//------------------------------------------------------------------------------
//  File        :   CNode.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for Node in the Distributed System
//------------------------------------------------------------------------------

//Headers
#include "CNode.h" 
        
//------------------------------------------------------------------------------
//Thread Handler for Node Message handler
static void* ThreadHandlerNodeMessage(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
        
    //Get the arguement and reinterpret as CNode
    CNode * l_pobjNodeThrdHndlrNodeMsg = reinterpret_cast<CNode*>(p_pvParam);

    //Call the in class node message handler
    l_pobjNodeThrdHndlrNodeMsg->NodeMessageHandler();

    //Return
    return NULL;
}

//------------------------------------------------------------------------------
//Thread Handler for Node Message handler
static void* ThreadHandlerProcessMutualExclusion(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
    
    //Get the arguement and reinterpret as CNode
    CNode * l_pobjNodeThrdHndlrMutualEx = reinterpret_cast<CNode*>(p_pvParam);

    //Call the in class node message handler
    l_pobjNodeThrdHndlrMutualEx->ProcessMutualExecReqHndlr();

    //Return
    return NULL;
}

//------------------------------------------------------------------------------
//Constructor
CNode :: CNode()
{
    //Initialize the member attributes
    //New Neeighbour vector
    m_veciNewNodeNghbrs.clear();

    //Requested neighbours
    m_veciRequestedNghbrs.clear();

    //Neigbours which sent negative acknowledgement
    m_veciNotNodeNghbrs.clear();

    //To be broadcasted    
    m_veciBroadcastedNghbrs.clear();

    //Boolean variable to check if already attached to a node while
    //creating the spanning tree
    m_bNodeVisited = false;

    //Status
    m_bInitStatus = false;

    //Boolean variable to check if spanning tree created
    m_bSpanningTreeCreated = false;

    //Node Index
    m_iNodeIndex = -1;

    //Node Index
    m_iDefaultNghbrsCount = 0;

    //Count of ACks
    m_iNoofAcks = 0;
    
    //Broadcast Acks
    m_iNoofBroadcastAcks = 0;

    //Node Parent Index
    m_iParentNodeIndex = -1;

    //Node Parent Index
    m_iBroacastParentNodeIndex = -1;

    //bool for sending broadcast source
    m_bBroadcastSource = false;

    //-----------------------------Project 2------------------------------------
    //Token Request Accepted for the Node
    m_bNodeTokenReqAccepted = false;

    //Node executing CS status
    m_bNodeInCS = false;

    //Node has requested token status
    m_bNodeHasRequestedToken = false;

    //Node has token status
    m_bNodeHasToken = false;

    //Bool variable to show status if request being processed
    m_bIsNodeProcessingReq = false;

    //Node Being Processed
    m_iNodeBeingProcessed = -1;

    //Pointer Node pointing towards the node having token
    m_iPointerNode = -1;
}

//------------------------------------------------------------------------------
//Destructor
CNode :: ~CNode()
{
    //If still Init close
    if(m_bInitStatus)
    {
        //Close
        Close();
    }
}

//------------------------------------------------------------------------------
//Initialize the Server
bool CNode :: Init(int p_iNodeIndex)
{
    //If already Init return false
    if(m_bInitStatus)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node Already Initialized"<<std::endl;
        return false;
    }

    //Validate Node index
    if(1 > p_iNodeIndex)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Invalid Node Index"<<std::endl;
        return false;
    }

    //Store the node index
    m_iNodeIndex = p_iNodeIndex;

    //Init the parser
    m_objParser.Init();

    //Get the Node information of the node
    m_stnodeinfoSelf = m_objParser.GetNodeInfo(m_iNodeIndex);

    //Get the Broadcast Node information
    m_stBroadcastInfo = m_objParser.GetBroadcastNodeInfo();

    //Update the Default neighbours count
    m_iDefaultNghbrsCount = m_stnodeinfoSelf.m_veciNodeNghbrs.size();

    //Create thread for message handler
    if(!m_objThread.Create(&ThreadHandlerNodeMessage,this))
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "
                 <<"Unable to create node message handler thread"<<std::endl;
        return false;
    }

    //Create thread for Process Mutual Exclusion 
    if(!m_objThrdPrcssMtlExclsn.Create(&ThreadHandlerProcessMutualExclusion,this))
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "
                 <<"Unable to create Process Mutual Exclusion handler thread"<<std::endl;
        return false;
    }

    //Set the Init Status as true
    m_bInitStatus = true;

    //Check if root node
    CheckIfRoot();

    //Wait for execution to be over
    //m_objThread.ThreadWait();

    //Return Success
    return true;
}

//------------------------------------------------------------------------------
//Close the Server
bool CNode :: Close()
{
    //If not init return false
    if(!m_bInitStatus)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node not Initialized"<<std::endl;
        return false;
    }

    //Stop the thread
    m_objThread.Stop();
    m_objThrdPrcssMtlExclsn.Stop();

    //Close the parser
    m_objParser.Close();
    
    //ReInitialize the member attributes
    //New Neeighbour vector
    m_veciNewNodeNghbrs.clear();

    //Requested neighbours
    m_veciRequestedNghbrs.clear();

    //Neigbours which sent negative acknowledgement
    m_veciNotNodeNghbrs.clear();

    //To be broadcasted    
    m_veciBroadcastedNghbrs.clear();
    
    //Boolean variable to check if already attached to a node while
    //creating the spanning tree
    m_bNodeVisited = false;

    //Status
    m_bInitStatus = false;

    //Boolean variable to check if spanning tree created
    m_bSpanningTreeCreated = false;

    //Node Index
    m_iNodeIndex = -1;

    //Node Index
    m_iDefaultNghbrsCount = 0;

    //Count of ACks
    m_iNoofAcks = 0;

    //Node Parent Index
    m_iParentNodeIndex = -1;

    //Node Parent Index
    m_iBroacastParentNodeIndex = -1;

    //-----------------------------Project 2------------------------------------
    //Token Request Accepted for the Node
    m_bNodeTokenReqAccepted = false;

    //Node executing CS status
    m_bNodeInCS = false;

    //Node has token status
    m_bNodeHasToken = false;

    //Node has requested token status
    m_bNodeHasRequestedToken = false;

    //Pointer Node pointing towards the node having token
    m_iPointerNode = -1;

    //bool for sending broadcast source
    m_bBroadcastSource = false;

    //Bool variable to show status if request being processed
    m_bIsNodeProcessingReq = false;

    //Node Being Processed
    m_iNodeBeingProcessed = -1;

    //return 
    return true;
}

//------------------------------------------------------------------------------
//Check if root node
bool CNode :: CheckIfRoot()
{
    //If not init return false
    if(!m_bInitStatus)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node not Initialized"<<std::endl;
        return false;
    } 

    //If spanning tree created no need of root check as all nodes are same
    if(m_bSpanningTreeCreated)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Spanning Tree already created"<<std::endl;
        return false;
    }

    //Check if not root node
    if(!m_stnodeinfoSelf.m_bRootNode)
    {
        return false;
    }
    
    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"In Root Node"<<std::endl;
    //Create the message
    MSG_INFO_T l_stmsginfoMsgInitiate;
    
    //The type of message
    l_stmsginfoMsgInitiate.m_eMsgType = MSG_TYPE_E::eMSG_INITIATE;

    //Node Identification
    l_stmsginfoMsgInitiate.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsgInitiate.m_iDestNodeNo = m_iNodeIndex;

    //Host Name of the Node
    std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
    std::size_t length = l_strHostName.
                                copy(l_stmsginfoMsgInitiate.m_arrchSrcHostName,
                                     l_strHostName.length(),
                                     0);
    l_stmsginfoMsgInitiate.m_arrchSrcHostName[length]='\0';

    //Port number
    l_stmsginfoMsgInitiate.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

    //Total No of Nodes
    l_stmsginfoMsgInitiate.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Dont update array as no nodes visited

    //Node Visited Counter
    l_stmsginfoMsgInitiate.m_iVisitedNodes = 0;

    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Adding Message to Queue"<<std::endl;
    //Add the message to the queue
    //CNodeMessageQ::GetInstance()->AddMessage(l_stmsginfoMsgInitiate);

    //Client object
    CClient l_objClient;

    //Init the client
    l_objClient.Init(m_stnodeinfoSelf.m_iPortNo,
                       m_stnodeinfoSelf.m_strHostName);    

    //Send Message to the source
    l_objClient.Send(l_stmsginfoMsgInitiate); 
      
}

//------------------------------------------------------------------------------
//Node Message Handler
void CNode :: NodeMessageHandler()
{
    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node Message Handler"<<std::endl;
    while(1)
    {
        //Message Structure
        MSG_INFO_T * l_pstmsginfoProcessMsg = new MSG_INFO_T;
        if(CNodeMessageQ::GetInstance()->RemoveMessage(*l_pstmsginfoProcessMsg))
        {
            //Process Message
            ProcessMessage(*l_pstmsginfoProcessMsg);
        }
        
        //If spanning tree not created
        if(!m_bSpanningTreeCreated)
        {
            //Check if spanning tree created
            CheckSpanningTreeCreated();   
        } 
    }
}

//Print Spanning Tree
void CNode :: PrintTreeNeighbours()
{
    //If spanning tree created print tree neigbours
    if(m_bSpanningTreeCreated)
    {
        //GenerateNodeIndex
        std::stringstream l_ssNodeIndex;
        l_ssNodeIndex << m_iNodeIndex;
        std::string l_strNodeIndex = l_ssNodeIndex.str();

        //Local File Object
        std::ofstream l_objFile ("TreeNeigbourNode" + l_strNodeIndex);
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n";    
        #endif
        for(int l_iPos = 0 ; l_iPos < m_veciNewNodeNghbrs.size() ; l_iPos++)
        {
            #if DEBUGPRINT
            std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "
                     <<"The Tree Neighbour of node "<<m_iNodeIndex<<" is "
                     <<m_veciNewNodeNghbrs[l_iPos]<<std::endl;
            #endif

            if (l_objFile.is_open())
            {
                l_objFile << m_veciNewNodeNghbrs[l_iPos]<<std::endl;
            }
            else 
            {            
                std::cout << "Unable to open Tree neighbour file\n";
            }
        }         
        #if DEBUGPRINT    
        std::cout<<"======================================================================\n\n\n";      
        #endif
        //Close the file
        l_objFile.close();        
    }
}


//------------------------------------------------------------------------------
//Process Message
void CNode :: ProcessMessage(MSG_INFO_T p_stMsg)
{
    if( MSG_TYPE_E::eMSG_INITIATE == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Handler Msg Initiate"<<std::endl;
	    MsgHndlrInitiate(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_REQ == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Handler Msg Req"<<std::endl;
	    MsgHndlrRequest(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_RESP == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrResponse(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_POS_ACK == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Handler Msg Pos Ack"<<std::endl;
	    MsgHndlrPositiveAck(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_NEG_ACK == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Handler Msg Neg Ack"<<std::endl;
	    MsgHndlrNegativeAck(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_BROADCAST == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrBroadcast(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_BROADCAST_ACK == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrBroadcastAck(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_BROADCAST_INITIATE == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrBroadcastInitiate(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_TRIGGER_APP_CS_REQ == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrTriggerAppCSReq(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_TOKEN_REQ == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrTokenReq(p_stMsg);
    }
    else if( MSG_TYPE_E::eMSG_TOKEN == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrToken(p_stMsg);
    }

}

//------------------------------------------------------------------------------
//Msg Handler Initiate
void CNode :: MsgHndlrInitiate(MSG_INFO_T p_stMsg)
{
    //Mark node as visited
    m_bNodeVisited = true;


    //This Function will initiate & send request messages to the roots neighbour
    for(int l_iPos = 0; l_iPos < m_iDefaultNghbrsCount; l_iPos++)
    {
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_REQ;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo =m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos];

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        //Node Visited Counter
        l_stmsginfoMsg.m_iVisitedNodes = 1;

        //Update array of nodes visited
        l_stmsginfoMsg.m_arriNodeVisited[l_stmsginfoMsg.m_iVisitedNodes-1] 
                                                                = m_iNodeIndex;        

        //Client object
        CClient l_objClient;

        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Root Node "<<m_iNodeIndex<<" is "
                    // <<"Sending Request to "
                   // <<m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos]<<std::endl;

        //Init the client
        l_objClient.Init(m_objParser.
               GetNodeInfo(m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos]).m_iPortNo,
               m_objParser.GetNodeInfo
               (m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos]).m_strHostName);    

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg); 
    }
}

//------------------------------------------------------------------------------
//Msg Handler Request
void CNode :: MsgHndlrRequest(MSG_INFO_T p_stMsg)
{
    //if the node already visited send negative acknowledment to the sender
    if(m_bNodeVisited)
    {
        //Send NACK to the sender   
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_NEG_ACK;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = p_stMsg.m_iSrcNodeNo;

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        //Node Visited Counter not reqd        

        //Update array of nodes visited not reqd

        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "<<"Sending Neg ACK to "
                        //                    <<p_stMsg.m_iSrcNodeNo<<std::endl;

        //Client object
        CClient l_objClient;

        //Init the client
        l_objClient.Init(p_stMsg.m_iSrcPortNo,
                         std::string(p_stMsg.m_arrchSrcHostName));    

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg); 

    }
    else
    {
        //Mark the node as visited
        m_bNodeVisited = true;

        //The sender node will be the parent of the current node in the spanning
        //tree. Store the sender node as parent node 
        m_iParentNodeIndex = p_stMsg.m_iSrcNodeNo;

        //The parent will also be its neighbour when spanning tree is completed
        //and we are creating just a broadcast service so we add it to the new
        //neighbour vector
        m_veciNewNodeNghbrs.push_back(p_stMsg.m_iSrcNodeNo);

        //boolean varibale to check if node visited
        bool m_bNeighbourVisted = false;        

        //Out of all the neighbours it has the node has to send request messages
        //to those which are not already visited.
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Visited Node Size "<<p_stMsg.m_iVisitedNodes<<std::endl;
        for(int l_iPos = 0 ; l_iPos < m_iDefaultNghbrsCount ; l_iPos++)
        {

            for(int l_iPos1 = 0 ; l_iPos1 < p_stMsg.m_iVisitedNodes ; l_iPos1++)
            {
                //If the neogbour node index is already in the visited list                
                if(m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos] == 
                        p_stMsg.m_arriNodeVisited[l_iPos1] )
                {
                    //make the neighbour visited variable as true
                    m_bNeighbourVisted = true;
                }                            
            }

            //If not visited add to the list to which request will be sent
            if(!m_bNeighbourVisted)                
            {
                //Add to the vector
                m_veciRequestedNghbrs.
                           push_back(m_stnodeinfoSelf.m_veciNodeNghbrs[l_iPos]);
            }

            //Reset value of the negihbour visited bool
            m_bNeighbourVisted = false;
        }
        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Requested Node Size "<<m_veciRequestedNghbrs.size()<<std::endl;
        //If the size of the requested neigbour vector is zero it implies
        //either the node was just connected to the sender node or the neigbours 
        //have already been visited,in this case send positive acknowledgement
        //to the sender node. This also helps us define the termination 
        if(0 == m_veciRequestedNghbrs.size())
        {
            //Send Positive Acknowledgement to the sender
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_POS_ACK;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = p_stMsg.m_iSrcNodeNo;

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                        copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                             l_strHostName.length(),
                                             0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

            //Node Visited Counter not reqd        

            //Update array of nodes visited not reqd

            //Set the bool spanning tree created as true
            m_bSpanningTreeCreated = true;           

            //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                 //               <<"Sending Pos ACK to "<<p_stMsg.m_iSrcNodeNo
                      //                                          <<std::endl;

            //Client object
            CClient l_objClient;

            //Init the client
            l_objClient.Init(p_stMsg.m_iSrcPortNo,
                             std::string(p_stMsg.m_arrchSrcHostName));    

            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg); 

            //PrintTreeNeighbours
            PrintTreeNeighbours();
        }
        else
        {
            //Send Request to all the Requested Neighbour List
            for(int l_iPos = 0; l_iPos < m_veciRequestedNghbrs.size(); l_iPos++)
            {
                //Send Request to the sender   
                //Create Request Message
                MSG_INFO_T l_stmsginfoMsg;

                //The type of message
                l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_REQ;

                //Node Identification
                l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

                //Node Identification
                l_stmsginfoMsg.m_iDestNodeNo = m_veciRequestedNghbrs[l_iPos];

                //Host Name of the Node
                std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
                std::size_t length = l_strHostName.
                                        copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                             l_strHostName.length(),
                                             0);
                l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

                //Port number
                l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

                //Total No of Nodes
                l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

                //Node Visited Counter
                l_stmsginfoMsg.m_iVisitedNodes = p_stMsg.m_iVisitedNodes + 1;

                //Update array of nodes visited
                for(int l_iPos = 0 ; l_iPos < p_stMsg.m_iVisitedNodes; l_iPos++)
                {
                    l_stmsginfoMsg.m_arriNodeVisited[l_iPos] = 
                                              p_stMsg.m_arriNodeVisited[l_iPos];
                }
                l_stmsginfoMsg.
                           m_arriNodeVisited[l_stmsginfoMsg.m_iVisitedNodes-1] 
                                                                = m_iNodeIndex;

                //Client object
                CClient l_objClient;

                //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                    // <<"Sending Request to "<<m_veciRequestedNghbrs[l_iPos]
                       //                                         <<std::endl;

                //Init the client
                l_objClient.Init(m_objParser.
                           GetNodeInfo(m_veciRequestedNghbrs[l_iPos]).m_iPortNo,
                           m_objParser.GetNodeInfo
                           (m_veciRequestedNghbrs[l_iPos]).m_strHostName);  

                //Send Message to the source
                l_objClient.Send(l_stmsginfoMsg); 
            }
        }
    }
}

//------------------------------------------------------------------------------
//Msg Handler Positive Acknowledgement
void CNode :: MsgHndlrPositiveAck(MSG_INFO_T p_stMsg)
{
    //Add the node to the New Node neighbour list
    m_veciNewNodeNghbrs.push_back(p_stMsg.m_iSrcNodeNo);

    //Increment the count of acknowledgements
    m_iNoofAcks++;    
}

//------------------------------------------------------------------------------
//Msg Handler Negative Acknowledgement
void CNode :: MsgHndlrNegativeAck(MSG_INFO_T p_stMsg)
{
    //Neigbours which sent negative acknowledgement
    m_veciNotNodeNghbrs.push_back(p_stMsg.m_iSrcNodeNo);

    //Increment the count of acknowledgements
    m_iNoofAcks++;
}

//------------------------------------------------------------------------------
//Initiate Broadcast for app tp generate CS Requests
void CNode :: InitiateTriggerAppCSReq()
{
    //This function is only Triggered from the Root Node where the spanning tree
    //execution was completed. This Function will do the following

    //1. Update its pointer to itself since that will have the first token
    m_iPointerNode = m_iNodeIndex;

    //2. Update that it has token
    m_bNodeHasToken = true;

    //3. Send a message Application Server for starting the CS Requests
    //Create Request Message
    APP_MSG_INFO_T l_stmsginfoMsg;

    //The type of message
    l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_INIT_CS_REQ;

    //Node Identification
    l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsg.m_iDestNodeNo = m_iNodeIndex;

    //Total No of Nodes
    l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Client object
    CClient l_objClient;

    /*std::cout<<"\n\n\n======================================================================\n"; 
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Source Node "<<m_iNodeIndex<<" is "
                 <<"Sending APP Message InitCS Request to its App"<<std::endl;
    std::cout<<"======================================================================\n\n\n";
    */

    //Init the client
    l_objClient.Init(m_objParser.GetAppPortNoByIndex(m_iNodeIndex-1),
                     m_stnodeinfoSelf.m_strHostName); 

    //Send Message to the App Servers
    l_objClient.SendAppMsg(l_stmsginfoMsg); 

    //4. Send Message of Trigger APP CS Req to all its tree neighbour
    for(int l_iPos = 0; l_iPos <  m_veciNewNodeNghbrs.size(); l_iPos++)
    {
        //Send App CS Req Trigger to all the tree neighbours
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_TRIGGER_APP_CS_REQ;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = m_veciNewNodeNghbrs[l_iPos];

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                     l_strHostName.length(),
                                     0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        //Client object
        CClient l_objClient;

        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
            // <<"Sending App Trigger CS Req to "<<m_veciNewNodeNghbrs[l_iPos]
               //                                         <<std::endl;

        //Init the client
        l_objClient.Init(m_objParser.
                   GetNodeInfo(m_veciNewNodeNghbrs[l_iPos]).m_iPortNo,
                   m_objParser.GetNodeInfo
                   (m_veciNewNodeNghbrs[l_iPos]).m_strHostName);  

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg);
    }
}

//------------------------------------------------------------------------------
//InitiateBroadcast
void CNode :: InitiateBroadcast()
{
    if(m_stBroadcastInfo.m_qiBroadcastSource.empty())
    {
        return;
    }

    //When root gets the spanning tree  
    int l_iBroadcastInitNode = m_stBroadcastInfo.m_qiBroadcastSource.front();

    //Send Request to the sender   
    //Create Request Message
    MSG_INFO_T l_stmsginfoMsg;

    //The type of message
    l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST_INITIATE;

    //Node Identification
    l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsg.m_iDestNodeNo = l_iBroadcastInitNode;

    //Host Name of the Node
    std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
    std::size_t length = l_strHostName.
                            copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                 l_strHostName.length(),
                                 0);
    l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

    //Port number
    l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

    //Total No of Nodes
    l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Client object
    CClient l_objClient;    

    //Init the client
    l_objClient.Init(m_objParser.GetNodeInfo(l_iBroadcastInitNode).m_iPortNo,
                   m_objParser.GetNodeInfo(l_iBroadcastInitNode).m_strHostName);  

    //Send Message to the source
    l_objClient.Send(l_stmsginfoMsg); 
}

//------------------------------------------------------------------------------
void CNode :: CheckSpanningTreeCreated()
{
    //If the No of responses received is equal to the no of requests sent then 
    // if it is root node change the spanning tree created to true
    // if it is non root node send back pack to the parent
    if(m_iDefaultNghbrsCount == m_iNoofAcks && m_stnodeinfoSelf.m_bRootNode)
    {   
        //Set the bool spanning tree created as true
        m_bSpanningTreeCreated = true;

        //PrintTreeNeighbours
        PrintTreeNeighbours();

        //InitiateBroadcast
        InitiateBroadcast();       

        //Initiate Broadcast for app tp generate CS Requests
        InitiateTriggerAppCSReq();
    }
    else if(m_veciRequestedNghbrs.size() == m_iNoofAcks && 
            !m_stnodeinfoSelf.m_bRootNode && 0 != m_veciRequestedNghbrs.size())
    {
        //Set the bool spanning tree created as true
        m_bSpanningTreeCreated = true;

        //Send Positive Acknowledgement to the sender
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_POS_ACK;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = m_iParentNodeIndex;

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        //Node Visited Counter not reqd        

        //Update array of nodes visited not reqd

        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                   //    <<"Sending Pos ACK to "<<m_iParentNodeIndex<<std::endl;
        //Client object
        CClient l_objClient;

        //Init the client
        l_objClient.Init(m_objParser.GetNodeInfo(m_iParentNodeIndex).m_iPortNo,
                    m_objParser.GetNodeInfo(m_iParentNodeIndex).m_strHostName);   

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg);

        //PrintTreeNeighbours
        PrintTreeNeighbours();
    }
}

//------------------------------------------------------------------------------
//SetBroadcast
void CNode :: SetAsBroadcastSource()
{
    //Set Broadcast as true
    m_bBroadcastSource = true;

    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"In Root Node"<<std::endl;
    //Create the message
    MSG_INFO_T l_stmsginfoMsgInitiate;
    
    //The type of message
    l_stmsginfoMsgInitiate.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST;

    //Node Identification
    l_stmsginfoMsgInitiate.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsgInitiate.m_iDestNodeNo = m_iNodeIndex;

    //Host Name of the Node
    std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
    std::size_t length = l_strHostName.
                                copy(l_stmsginfoMsgInitiate.m_arrchSrcHostName,
                                     l_strHostName.length(),
                                     0);
    l_stmsginfoMsgInitiate.m_arrchSrcHostName[length]='\0';

    //Port number
    l_stmsginfoMsgInitiate.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

    //Total No of Nodes
    l_stmsginfoMsgInitiate.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Dont update array as no nodes visited

    //Node Visited Counter
    l_stmsginfoMsgInitiate.m_iVisitedNodes = 0;

    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Adding Message to Queue"<<std::endl;
    //Add the message to the queue
    //CNodeMessageQ::GetInstance()->AddMessage(l_stmsginfoMsgInitiate);

    //Client object
    CClient l_objClient;

    //Init the client
    l_objClient.Init(m_stnodeinfoSelf.m_iPortNo,
                       m_stnodeinfoSelf.m_strHostName);    

    //Send Message to the source
    l_objClient.Send(l_stmsginfoMsgInitiate);
}

//------------------------------------------------------------------------------
//Msg Handler Response
void CNode :: MsgHndlrBroadcastInitiate(MSG_INFO_T p_stMsg)
{
    //SetAsBroadcastSource
    SetAsBroadcastSource();
}

//------------------------------------------------------------------------------
//Msg Handler Broadcast
void CNode :: MsgHndlrBroadcast(MSG_INFO_T p_stMsg)
{
    //Check if spanning tree created
    if(!m_bSpanningTreeCreated)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Cannot Broadcast as Spanning tree not created"<<std::endl;
        return;
    }

    //Check if broadcast source
    if(m_bBroadcastSource)
    {        
        //Remove the first element in the queue
        m_stBroadcastInfo.m_qiBroadcastSource.pop();
    
        //This block will inititate broadcast messages
        for(int l_iPos = 0; l_iPos < m_veciNewNodeNghbrs.size(); l_iPos++)
        {
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_veciNewNodeNghbrs[l_iPos];

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                        copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                             l_strHostName.length(),
                                             0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

            //Node Visited Counter
            l_stmsginfoMsg.m_iVisitedNodes = 1;

            //Update array of nodes visited
            l_stmsginfoMsg.m_arriNodeVisited[l_stmsginfoMsg.m_iVisitedNodes-1] 
                                                                = m_iNodeIndex;        

            //Client object
            CClient l_objClient;
            #if DEBUGPRINT
            std::cout<<"\n\n\n======================================================================\n"; 
            std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Broacast Source Node "<<m_iNodeIndex<<" is "
                         <<"Sending Request to "
                        <<m_veciNewNodeNghbrs[l_iPos]<<std::endl;
            std::cout<<"======================================================================\n\n\n";
            #endif
            //Init the client
            l_objClient.Init(m_objParser.
               GetNodeInfo(m_veciNewNodeNghbrs[l_iPos]).m_iPortNo,
               m_objParser.GetNodeInfo
               (m_veciNewNodeNghbrs[l_iPos]).m_strHostName);    

            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg); 
        }
    }
    else
    {
        //Remove the first element in the queue
        m_stBroadcastInfo.m_qiBroadcastSource.pop();
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n";
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                <<" Received Broadcast from "<<p_stMsg.m_iSrcNodeNo<<std::endl;
        std::cout<<"======================================================================\n\n\n";        
        #endif
        //Broadcst Node Parent Index
        m_iBroacastParentNodeIndex = p_stMsg.m_iSrcNodeNo;

        //boolean varibale to check if node visited
        bool m_bBroadcastedFrom = false;        

        //Out of all the neighbours it has the node has to send broadcast msgs
        //to those which are not already visited.
        for(int l_iPos = 0 ; l_iPos < m_veciNewNodeNghbrs.size() ; l_iPos++)
        {

            for(int l_iPos1 = 0 ; l_iPos1 < p_stMsg.m_iVisitedNodes ; l_iPos1++)
            {
                //If the new neigbour node index is already in the visited list                
                if(m_veciNewNodeNghbrs[l_iPos] == 
                        p_stMsg.m_arriNodeVisited[l_iPos1] )
                {
                    //make the neighbour visited variable as true
                    m_bBroadcastedFrom = true;
                }                            
            }

            //If not visited add to the list to which request will be sent
            if(!m_bBroadcastedFrom)                
            {
                //Add to the vector
                m_veciBroadcastedNghbrs.
                           push_back(m_veciNewNodeNghbrs[l_iPos]);
            }

            //Reset value of the negihbour visited bool
            m_bBroadcastedFrom = false;
        }
        
        //Last Node
        if(0 == m_veciBroadcastedNghbrs.size())
        {
            //Send Positive Acknowledgement to the sender
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST_ACK;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = p_stMsg.m_iSrcNodeNo;

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                        copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                             l_strHostName.length(),
                                             0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

            //Node Visited Counter not reqd        

            //Update array of nodes visited not reqd
         
            //std::cout<<"======================================================================\n\n\n";
            //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                       // <<"Sending Broadcast ACK to "<<p_stMsg.m_iSrcNodeNo
                                       //                         <<std::endl;
            //std::cout<<"======================================================================\n\n\n";

            //Client object
            CClient l_objClient;

            //Init the client
            l_objClient.Init(p_stMsg.m_iSrcPortNo,
                             std::string(p_stMsg.m_arrchSrcHostName));    

            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg); 

            //ClearBroadcast
            ClearBroadcast();           
        }
        else
        {
            //Send Request to all the Requested Neighbour List
            for(int l_iPos = 0; 
                l_iPos < m_veciBroadcastedNghbrs.size(); 
                l_iPos++)
            {
                //Send Request to the sender   
                //Create Request Message
                MSG_INFO_T l_stmsginfoMsg;

                //The type of message
                l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST;

                //Node Identification
                l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

                //Node Identification
                l_stmsginfoMsg.m_iDestNodeNo = m_veciBroadcastedNghbrs[l_iPos];

                //Host Name of the Node
                std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
                std::size_t length = l_strHostName.
                                        copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                             l_strHostName.length(),
                                             0);
                l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

                //Port number
                l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

                //Total No of Nodes
                l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

                //Node Visited Counter
                l_stmsginfoMsg.m_iVisitedNodes = p_stMsg.m_iVisitedNodes + 1;

                //Update array of nodes visited
                for(int l_iPos = 0 ; l_iPos < p_stMsg.m_iVisitedNodes; l_iPos++)
                {
                    l_stmsginfoMsg.m_arriNodeVisited[l_iPos] = 
                                              p_stMsg.m_arriNodeVisited[l_iPos];
                }
                l_stmsginfoMsg.
                           m_arriNodeVisited[l_stmsginfoMsg.m_iVisitedNodes-1] 
                                                                = m_iNodeIndex;

                //Client object
                CClient l_objClient;
            #if DEBUGPRINT
          std::cout<<"\n\n\n======================================================================\n";
                std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                     <<"Sending Broadcast to "<<m_veciBroadcastedNghbrs[l_iPos]
                                                                <<std::endl;
            std::cout<<"======================================================================\n\n\n";
                #endif
                //Init the client
                l_objClient.Init(m_objParser.
                       GetNodeInfo(m_veciBroadcastedNghbrs[l_iPos]).m_iPortNo,
                       m_objParser.GetNodeInfo
                       (m_veciBroadcastedNghbrs[l_iPos]).m_strHostName);  

                //Send Message to the source
                l_objClient.Send(l_stmsginfoMsg); 
            }
        }
    }
}

//------------------------------------------------------------------------------
//Msg Handler Broadcast Acknowledgement
void CNode :: MsgHndlrBroadcastAck(MSG_INFO_T p_stMsg)
{
    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                   //     <<"Received Broadcast ACK from "<<p_stMsg.m_iSrcNodeNo
                      //                                          <<std::endl;

    //Increment the count of acknowledgements
    m_iNoofBroadcastAcks++;

    //If the No of responses received is equal to the no of requests sent then 
    // if it is root node change the spanning tree created to true
    // if it is non root node send back pack to the parent
    if(m_veciNewNodeNghbrs.size() == m_iNoofBroadcastAcks && m_bBroadcastSource)
    {   
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n";
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Broadcast completed which started at node "
                                                <<m_iNodeIndex<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
        //ClearBroadcast
        ClearBroadcast();    

        //InitiateBroadcast
        InitiateBroadcast();             
    }
    else if(m_veciBroadcastedNghbrs.size() == m_iNoofBroadcastAcks && 
            !m_bBroadcastSource && 0 != m_veciBroadcastedNghbrs.size())
    {
        

        //Send Positive Acknowledgement to the sender
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_BROADCAST_ACK;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = m_iBroacastParentNodeIndex;

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        //Node Visited Counter not reqd        

        //Update array of nodes visited not reqd

        //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
             //    <<"Sending Broadcast ACK to "
                               //     <<m_iBroacastParentNodeIndex<<std::endl;
        //Client object
        CClient l_objClient;

        //Init the client
        l_objClient.Init(
            m_objParser.GetNodeInfo(m_iBroacastParentNodeIndex).m_iPortNo,
            m_objParser.GetNodeInfo(m_iBroacastParentNodeIndex).m_strHostName);   

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg);
        
        //ClearBroadcast
        ClearBroadcast();  
    }
}

//------------------------------------------------------------------------------
void CNode:: ClearBroadcast()
{
    //Reinit all braodcast related info
    m_bBroadcastSource = false;
    m_veciBroadcastedNghbrs.clear();
    m_iBroacastParentNodeIndex = -1;
    m_iNoofBroadcastAcks = 0;
}

//------------------------------------------------------------------------------
//Msg Handler Response
void CNode :: MsgHndlrResponse(MSG_INFO_T p_stMsg)
{

}

//------------------------------------------------------------------------------
//API for System API to Request CS
void CNode :: CS_Enter()
{
    //Send Token Request to self to get added to the queue
    //Create the message
    MSG_INFO_T l_stmsginfoMsgSelfTokenReq;
    
    //The type of message
    l_stmsginfoMsgSelfTokenReq.m_eMsgType = MSG_TYPE_E::eMSG_TOKEN_REQ;

    //Node Identification
    l_stmsginfoMsgSelfTokenReq.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsgSelfTokenReq.m_iDestNodeNo = m_iNodeIndex;

    //Host Name of the Node
    std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
    std::size_t length = l_strHostName.
                                copy(l_stmsginfoMsgSelfTokenReq.m_arrchSrcHostName,
                                     l_strHostName.length(),
                                     0);
    l_stmsginfoMsgSelfTokenReq.m_arrchSrcHostName[length]='\0';

    //Port number
    l_stmsginfoMsgSelfTokenReq.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

    //Total No of Nodes
    l_stmsginfoMsgSelfTokenReq.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Dont update array as no nodes visited

    //Node Visited Counter
    l_stmsginfoMsgSelfTokenReq.m_iVisitedNodes = 0;

    //Client object
    CClient l_objClient;

    //Init the client
    l_objClient.Init(m_stnodeinfoSelf.m_iPortNo,
                       m_stnodeinfoSelf.m_strHostName);    

    /*std::cout<<"\n\n\n======================================================================\n"; 
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Source Node "<<m_iNodeIndex<<" is "
                 <<"Sending Token Request to itself"<<std::endl;
    std::cout<<"======================================================================\n\n\n";
    */

    //Send Message to the source
    l_objClient.Send(l_stmsginfoMsgSelfTokenReq);   

    //Safety Property block if it receives a request before spanning tree     
    while(!m_bSpanningTreeCreated){}

    //General Blocking scenario till Node's Token Req Accepted
    while(!m_bNodeTokenReqAccepted){}

    //Return when the loop breaks due to spanning tree creation
    return;
}

//------------------------------------------------------------------------------
//API for System API to Notify CS Exit
void CNode :: CS_Exit()
{
    //Update the Node In CS as false
    m_bNodeInCS = false;

    //Update Node Token Request Accepted as false
    m_bNodeTokenReqAccepted = false;

    //Bool variable to show status if request being processed
    m_bIsNodeProcessingReq = false;
}

//------------------------------------------------------------------------------
//Msg Handler Trigger App CS Req
void CNode :: MsgHndlrTriggerAppCSReq(MSG_INFO_T p_stMsg)
{
    //This Trigger is received only by system nodes other than source node which
    //starts the first trigger.On receiving the message the steps are :

    //1.Update its pointer node to the source node of the message received
    m_iPointerNode = p_stMsg.m_iSrcNodeNo;

    //2.Send Message to its own applicaton node to start generating CS Requests
    //Create Request Message
    APP_MSG_INFO_T l_stmsginfoMsg;

    //The type of message
    l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_INIT_CS_REQ;

    //Node Identification
    l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

    //Node Identification
    l_stmsginfoMsg.m_iDestNodeNo = m_iNodeIndex;

    //Total No of Nodes
    l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

    //Client object
    CClient l_objClient;

    /*std::cout<<"\n\n\n======================================================================\n"; 
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Source Node "<<m_iNodeIndex<<" is "
                 <<"Sending APP Message InitCS Request to its App"<<std::endl;
    std::cout<<"======================================================================\n\n\n";
    */

    //Init the client
    l_objClient.Init(m_objParser.GetAppPortNoByIndex(m_iNodeIndex-1),
                     m_stnodeinfoSelf.m_strHostName); 

    //Send Message to the App Servers
    l_objClient.SendAppMsg(l_stmsginfoMsg); 



    //3.Send Broadcast to all the neighbour nodes except the source node of msg
    for(int l_iPos = 0; l_iPos <  m_veciNewNodeNghbrs.size(); l_iPos++)
    {

        if(m_veciNewNodeNghbrs[l_iPos] != p_stMsg.m_iSrcNodeNo)
        {
            //Send App CS Req Trigger to all the tree neighbours
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_TRIGGER_APP_CS_REQ;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_veciNewNodeNghbrs[l_iPos];

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

            //Client object
            CClient l_objClient;

            //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex<<" is "
                // <<"Sending App Trigger CS Req to "<<m_veciNewNodeNghbrs[l_iPos]
                   //                                         <<std::endl;

            //Init the client
            l_objClient.Init(m_objParser.
                       GetNodeInfo(m_veciNewNodeNghbrs[l_iPos]).m_iPortNo,
                       m_objParser.GetNodeInfo
                       (m_veciNewNodeNghbrs[l_iPos]).m_strHostName);  

            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg);
        }
    }
}

//------------------------------------------------------------------------------
//Msg Handler Token Request
void CNode :: MsgHndlrTokenReq(MSG_INFO_T p_stMsg)
{
    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n"; 
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" Received Token Request From "<<p_stMsg.m_iSrcNodeNo<<std::endl;
    std::cout<<"======================================================================\n\n\n";
    #endif    
    //Node Requesting for Token is added to the queue    
    CTokenRequestQ::GetInstance()->AddMessage(p_stMsg.m_iSrcNodeNo);
}

//------------------------------------------------------------------------------
//Msg Handler Token
void CNode :: MsgHndlrToken(MSG_INFO_T p_stMsg)
{
    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n"; 
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" Received Token From "<<p_stMsg.m_iSrcNodeNo<<std::endl;
    std::cout<<"======================================================================\n\n\n";
    #endif
    //Update Node has token equal to true
    m_bNodeHasToken = true;
}

//------------------------------------------------------------------------------
//Node Message Handler
void CNode :: ProcessMutualExecReqHndlr()
{
    while(1)
    {
        //If Node is Not processing any request get the token request for next
        //wait till there is one
        if(!m_bIsNodeProcessingReq)
        {
     
            //Node Being Processed
            int * l_iNodeBeingProcessed = new int; 

            if(CTokenRequestQ::GetInstance()->RemoveMessage(*l_iNodeBeingProcessed))
            {
                //Node Being Processed
                m_iNodeBeingProcessed = *l_iNodeBeingProcessed;
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" is processing token request for node :: "<<m_iNodeBeingProcessed<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
                //Bool variable to show status if request being processed
                m_bIsNodeProcessingReq = true;
            }
        }

        //If Node is processing any request then call Process Mutual Req
        if(m_bIsNodeProcessingReq)
        {         
            //ProcessMutualExecReq
            ProcessMutualExecReq();
        }

    }
}

//------------------------------------------------------------------------------
//Process Mutual Exclusion Req
void CNode :: ProcessMutualExecReq()
{

    //Check if Node in CS
    if(m_bNodeInCS)
    {
        /*std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" Node in CS"<<std::endl;
        std::cout<<"======================================================================\n\n\n";*/
        return;
    }   

    if(1 > m_iNodeBeingProcessed)
    {
        return;
    }
    
    //CASE 1: Node is requesting token.Node has token
    if(m_iNodeBeingProcessed == m_iNodeIndex && m_bNodeHasToken)
    {
        //Invalidate the Node being processed
        m_iNodeBeingProcessed = -1;            

        //Node in CS
        m_bNodeInCS = true;

        //Token Request Accepted - This will return the CS_Enter Call
        m_bNodeTokenReqAccepted = true;

        //Node has requested token status
        m_bNodeHasRequestedToken = false;
    }

    //CASE 2: Node is requesting token.Node doesn't have token
    else if(m_iNodeBeingProcessed == m_iNodeIndex && !m_bNodeHasToken)
    {
        if(!m_bNodeHasRequestedToken)
        {
            //Send Token Request to Pointer Node
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_TOKEN_REQ;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_iPointerNode;

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" is Sending Token Request for self i.e node "<<m_iNodeBeingProcessed<<" to "<<m_iPointerNode<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
            //Client object
            CClient l_objClient;

            //Init the client
            l_objClient.Init(m_objParser.GetNodeInfo(m_iPointerNode).m_iPortNo,
                             m_objParser.GetNodeInfo(m_iPointerNode).m_strHostName);  
        

            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg);

            //Node has requested token status
            m_bNodeHasRequestedToken = true;
        }
    }

    //CASE 3: Some other Node is requesting token.Node doesn't have token
    else if(m_iNodeBeingProcessed != m_iNodeIndex && !m_bNodeHasToken)
    {
        if(!m_bNodeHasRequestedToken)
        {
            //Send Token Request to Pointer Node
            //Create Request Message
            MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_TOKEN_REQ;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_iPointerNode;

            //Host Name of the Node
            std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
            std::size_t length = l_strHostName.
                                    copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                         l_strHostName.length(),
                                         0);
            l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

            //Port number
            l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;

        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" is Sending Token Request for the node "<<m_iNodeBeingProcessed<<" to "<<m_iPointerNode<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
            //Client object
            CClient l_objClient;

            //Init the client
            l_objClient.Init(m_objParser.GetNodeInfo(m_iPointerNode).m_iPortNo,
                             m_objParser.GetNodeInfo(m_iPointerNode).m_strHostName);  

        
            //Send Message to the source
            l_objClient.Send(l_stmsginfoMsg);

            //Node has requested token status
            m_bNodeHasRequestedToken = true;
        }
    }

    //CASE 4: Some other Node is requesting token.Node has token
    else if(m_iNodeBeingProcessed != m_iNodeIndex && m_bNodeHasToken)
    {

        //Update pointer to the front of the queue
        m_iPointerNode = m_iNodeBeingProcessed;

        //Send Token to Front of the CS Req Queue
        //Create Request Message
        MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = MSG_TYPE_E::eMSG_TOKEN;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = m_iNodeBeingProcessed;

        //Host Name of the Node
        std::string l_strHostName = m_stnodeinfoSelf.m_strHostName;
        std::size_t length = l_strHostName.
                                copy(l_stmsginfoMsg.m_arrchSrcHostName,
                                     l_strHostName.length(),
                                     0);
        l_stmsginfoMsg.m_arrchSrcHostName[length]='\0';

        //Port number
        l_stmsginfoMsg.m_iSrcPortNo = m_stnodeinfoSelf.m_iPortNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_stnodeinfoSelf.m_iTotalNodes;
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                 <<" is Sending Token to "<<m_iNodeBeingProcessed<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
        //Client object
        CClient l_objClient;

        //Init the client
        l_objClient.Init(m_objParser.
                              GetNodeInfo(m_iNodeBeingProcessed).m_iPortNo,
                         m_objParser.
                             GetNodeInfo(m_iNodeBeingProcessed).m_strHostName);     

        //Send Message to the source
        l_objClient.Send(l_stmsginfoMsg);

        //Update Node has token equal to false
        m_bNodeHasToken = false;

        //Invalidate the node being processed
        m_iNodeBeingProcessed = -1;

        //Node has requested token status
        m_bNodeHasRequestedToken = false;

        //Bool variable to show status if request being processed
        m_bIsNodeProcessingReq = false;
    }
}
//----------------------------End of File---------------------------------------

//------------------------------------------------------------------------------
//  File        :   CAppModule.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for Node in the Distributed System
//------------------------------------------------------------------------------

//Headers
#include "CAppModule.h"
#include "CClient.h"
#include <unistd.h>

//------------------------------------------------------------------------------
//Thread Handler for App Message handler
static void* ThreadHandlerAppMessage(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
    
    //Get the arguement and reinterpret as CNode
    CAppModule * l_pobjAppMsgHndlr = reinterpret_cast<CAppModule*>(p_pvParam);

    //Call the in class App message handler
    l_pobjAppMsgHndlr->AppMessageHandler();

    //Return
    return NULL;
}

//------------------------------------------------------------------------------
//Thread Handler for App Module CS Req Generator
static void* ThreadHandlerAppCSReqGenerator(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
    
    //Get the arguement and reinterpret as CNode
    CAppModule * l_pobjAppModule = reinterpret_cast<CAppModule*>(p_pvParam);

    //Call the in class App CS Req Generator
    l_pobjAppModule->AppCSReqGenerator();

    //Return
    return NULL;
}

//------------------------------------------------------------------------------        
//Constructor
CAppModule :: CAppModule()
{
    //Init the Member attributes
    //Status
    m_bInitStatus = false;

    //Total Number of CS Requests per Node
    m_iTotalCSRequests = 0;

    //Mean delay between two CS execution
    m_fMeanDelayBetnCS  = 0;

    //Mean Duration of CS
    m_fMeanDurationCS = 0;

    //Node Index
    m_iNodeIndex = 0;
    
    //Total Nodes
    m_iTotalNodes = 0;

    //System API Pointer
    m_pobjSystemAPI = NULL;

    //Clear the vector clock
    m_veciVectorClockNode.clear();

    //Vector to Store the Tree Neighbours
    m_veciTreeNeighbours.clear();

    //Vector to keep the all the nodes which this app node will be sending 
    //broadcast messages
    m_veciBroadcastedNghbrs.clear();

    //Node Parent Index
    m_iBroacastParentNodeIndex = -1;

    //Count of Broadcast ACks
    m_iNoofBroadcastAcks = 0;

    //bool for sending broadcast source
    m_bBroadcastSource = false;

    //bool for showing app cs task completion
    m_bIsCSTaskFinished = false;
}

//------------------------------------------------------------------------------
//Destructor
CAppModule :: ~CAppModule()
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
bool CAppModule :: Init(int p_iNodeIndex,CSystemAPI* p_pobjSystemAPI)
{
//If already Init return false
    if(m_bInitStatus)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Module :: Already Initialized"<<std::endl;
        return false;
    }

    //Validate Node index
    if(1 > p_iNodeIndex)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Module :: Invalid Node Index"<<std::endl;
        return false;
    }
    
    //Validate SystemAPI Pointer object
    if(NULL == p_pobjSystemAPI)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Module :: System API Null Pointer"<<std::endl;
        return false;
    }

    //System API Pointer
    m_pobjSystemAPI = p_pobjSystemAPI;

    //Store the node index
    m_iNodeIndex = p_iNodeIndex;

    //Init the parser
    m_objParser.Init();

    //Total Number of CS Requests per Node
    m_iTotalCSRequests = m_objParser.GetTotalCSRequests();

    //Mean delay between two CS execution
    m_fMeanDelayBetnCS  = m_objParser.GetMeanDelayBetnCS();

    //Mean Duration of CS
    m_fMeanDurationCS = m_objParser.GetMeanDurationCS();

    //Total Nodes
    m_iTotalNodes = m_objParser.GetTotalNodes();

    //Set the Probability Distribution Class with the Mean Values

    //Init the Vector Clock
    for(int l_iPos = 0 ; l_iPos < m_iTotalNodes ; l_iPos++)
    {
        //Init each processes count of mutual exec request to be 0
        m_veciVectorClockNode.push_back(0);
    }

    //Create thread for message handler
    if(!m_objThreadAppMsg.Create(&ThreadHandlerAppMessage,this))
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "
            <<"App Module :: Unable to create App message handler thread"
            <<std::endl;
        return false;
    }

    //Set the Init Status as true
    m_bInitStatus = true;

    //Return Success
    return true;
}

//------------------------------------------------------------------------------
//Close the Server
bool CAppModule :: Close()
{
    //If not init return false
    if(!m_bInitStatus)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Module :: Node not Initialized"<<std::endl;
        return false;
    }

    //ReInit the Member attributes 
    //Stop Thread for App message handler
    m_objThreadAppMsg.Stop();

    //Stop Thread for App CS Generator handler
    m_objThreadAppGenCSReq.Stop();

    //Close the parser
    m_objParser.Close();

    //Total Number of CS Requests per Node
    m_iTotalCSRequests = 0;

    //Mean delay between two CS execution
    m_fMeanDelayBetnCS  = 0;

    //Mean Duration of CS
    m_fMeanDurationCS = 0;

    //Node Index
    m_iNodeIndex = 0;

    //Total Nodes
    m_iTotalNodes = 0;

    //Clear the vector clock
    m_veciVectorClockNode.clear();

    //Vector to Store the Tree Neighbours
    m_veciTreeNeighbours.clear();

    //System API Pointer
    m_pobjSystemAPI = NULL;

    //Vector to keep the all the nodes which this app node will be sending 
    //broadcast messages
    m_veciBroadcastedNghbrs.clear();

    //Node Parent Index
    m_iBroacastParentNodeIndex = -1;

    //Count of Broadcast ACks
    m_iNoofBroadcastAcks = 0;

    //bool for sending broadcast source
    m_bBroadcastSource = false;

    //bool for showing app cs task completion
    m_bIsCSTaskFinished = false;

    //Status
    m_bInitStatus = false;

    //return 
    return true;
}

//------------------------------------------------------------------------------
//App Message Handler
void CAppModule :: AppMessageHandler()
{
    while(1)
    {
        //Message Structure
        APP_MSG_INFO_T * l_pstmsginfoProcessMsg = new APP_MSG_INFO_T;
        if(CAppMessageQ::GetInstance()->RemoveMessage(*l_pstmsginfoProcessMsg))
        {
            //Process Message
            ProcessMessage(*l_pstmsginfoProcessMsg);
        }               
    }
}

//------------------------------------------------------------------------------
//Process Message
void CAppModule :: ProcessMessage(APP_MSG_INFO_T p_stMsg)
{
    if( APP_MSG_TYPE_E::eAPP_MSG_INIT_CS_REQ == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrAppMsgInitCSReq(p_stMsg);
    }
    else if( APP_MSG_TYPE_E::eAPP_MSG_BROADCAST == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrAppMsgBroadcast(p_stMsg);
    }
    else if( APP_MSG_TYPE_E::eAPP_MSG_BROADCAST_ACK == p_stMsg.m_eMsgType)
    {
	    //Call the respective handler message
	    MsgHndlrAppMsgBroadcastAck(p_stMsg);
    }

}

//------------------------------------------------------------------------------
//Msg Handler Initiate
void CAppModule :: MsgHndlrAppMsgInitCSReq(APP_MSG_INFO_T p_stMsg)
{
    //Create thread for message handler
    if(!m_objThreadAppGenCSReq.Create(&ThreadHandlerAppCSReqGenerator,this))
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex
                <<" "<<"App Module :: Unable to create App CS Req Generator"
                <<std::endl;
        return;
    }
}

//------------------------------------------------------------------------------
//App Module Thread Which keeps generating CS Request
void CAppModule :: AppCSReqGenerator()
{
    //Get the Tree Neighbours
    GetTreeNeighbours();

    //Local Count of CS Requests
    int l_iCSReqCount = 0;
    
    srand(time(0));    

    //Run the Generator Till the Total CS Requests allowed per Node
    while(l_iCSReqCount < m_iTotalCSRequests)
    {
        //Check if the System API Pointer is NULL if it is then continue
        if(NULL == m_pobjSystemAPI)
        {
            continue;
        }


        //Random Value        
        double l_dRandomValue1 = rand() / ((double) RAND_MAX);

        //Delay between two CS execution
        float l_fDurationCS = -log(l_dRandomValue1) * m_fMeanDurationCS;

        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<" Duration of CS :: "<<l_fDurationCS<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif

        //Random Value        
        double l_dRandomValue2 = rand() / ((double) RAND_MAX);

        //Delay between two CS execution
        float l_fDelayBetnCS = -log(l_dRandomValue2) * m_fMeanDelayBetnCS;

        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<" Delay Between CS :: "<<l_fDelayBetnCS<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif

        //Call the Blocking CS_Enter Function in the system module
        m_pobjSystemAPI->CS_Enter();

        //Update the vector clock entry for this node
        m_veciVectorClockNode[m_iNodeIndex-1] += 1; 

        //bool for sending broadcast source
        m_bBroadcastSource = true;

        //This is the Source of the broadcast from the Application Module
        for(int l_iPos = 0 ; l_iPos < m_veciTreeNeighbours.size() ; l_iPos++)
        {
            //Create Request Message
            APP_MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_BROADCAST;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_veciTreeNeighbours[l_iPos];

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_iTotalNodes;

            //Update the Vector Clock to be piggybacked
            for(int l_iPos1 = 0; l_iPos1 < m_iTotalNodes; l_iPos1++)
            {
                //Update the individual values of the vector clock 
                l_stmsginfoMsg.m_arriNodeVectorClock[l_iPos1] = 
                                        m_veciVectorClockNode[l_iPos1];    
            }

            //Client object
            CClient l_objClient;
            #if DEBUGPRINT
            std::cout<<"\n\n\n======================================================================\n"; 
            std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Executing CS at node "<<m_iNodeIndex<<" is "
                         <<"Sending App Broadcast Message to "
                        <<m_veciTreeNeighbours[l_iPos]<<std::endl;
            std::cout<<"======================================================================\n\n\n";
            #endif
            //Init the client
            l_objClient.Init(m_objParser.GetAppPortNoByIndex(m_veciTreeNeighbours[l_iPos]-1),
                             m_objParser.GetNodeInfo
                             (m_veciTreeNeighbours[l_iPos]).m_strHostName);    

            //Send Message to the App Servers
            l_objClient.SendAppMsg(l_stmsginfoMsg); 

        }

        //Write Data to File
        //File Object to perform the operation
        std::ofstream l_objFileLog;

        //Open the file in the append mode
        l_objFileLog.open("Project2LogFile.txt", std::ios_base::app);

        //Write to the File
        l_objFileLog << "The Node Entering CS is "<<m_iNodeIndex<<" With Time Stamp ";

        //Add the Time stamp to the Log File
        for(int l_iPos1 = 0; l_iPos1 < m_iTotalNodes; l_iPos1++)
        {
            //Update the individual values of the vector clock 
            l_objFileLog << m_veciVectorClockNode[l_iPos1]<< " ";
        }

        //New Line
        l_objFileLog << "\n";
        

        //Wait until source received broadcast acks from nodes
        while(!m_bIsCSTaskFinished){}

        //Set Task Finished is False
        m_bIsCSTaskFinished = false;   

        //Wait for time time equal to current CS Execution Time       
        sleep(l_fDurationCS/1000);


        //Write to the File
        l_objFileLog << "The Node Exiting CS is "<<m_iNodeIndex<<"\n";

        //Close the File
        l_objFileLog.close(); 

        //Call the CS_Exit Function in system module to notify App exiting CS
        m_pobjSystemAPI->CS_Exit();

        //Wait for time time equal to current delay between next CS Req
        sleep(l_fDelayBetnCS/1000);
       
        //Increment the CS Request Count
        l_iCSReqCount++;
    }
}

//------------------------------------------------------------------------------
//GetTreeNighbours
void CAppModule :: GetTreeNeighbours()
{
    //GenerateNodeIndex
    std::stringstream l_ssNodeIndex;
    l_ssNodeIndex << m_iNodeIndex;
    std::string l_strNodeIndex = l_ssNodeIndex.str();
 
    //Local File Object
    std::fstream l_objFileRead;

    //Open the config file for parsing
    l_objFileRead.open(("TreeNeigbourNode" + l_strNodeIndex), 
                    std::fstream::in);
    //Local String
    std::string l_strLine;
    
    //Loop to extract Node information from the file
    while (std::getline(l_objFileRead, l_strLine))
    {
        //Add the Node to the vector
        m_veciTreeNeighbours.push_back(atoi(l_strLine.c_str()));
    }

    /*std::cout<<"\n\n\n======================================================================\n";    
    for(int l_iPos = 0 ; l_iPos < m_veciTreeNeighbours.size(); l_iPos++)
    {
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "
                       <<"App Module :: The Tree Neighbour read from the file :: "
                       <<"TreeNeigbourNode" + l_strNodeIndex
                       <<" is :: "<<m_veciTreeNeighbours[l_iPos]<<std::endl;
    }
    std::cout<<"======================================================================\n\n\n";  */    
    l_objFileRead.close();
}

//------------------------------------------------------------------------------
//Msg Handler Request
void CAppModule :: MsgHndlrAppMsgBroadcast(APP_MSG_INFO_T p_stMsg)
{
    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n";
    std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App Module :: Node "<<m_iNodeIndex
            <<" Received APP CS Broadcast from "<<p_stMsg.m_iSrcNodeNo<<std::endl;
    std::cout<<"======================================================================\n\n\n";     
    #endif
    //Broadcst Node Parent Index
    m_iBroacastParentNodeIndex = p_stMsg.m_iSrcNodeNo;

    //Update its own vector clock
    for(int l_iPos = 0 ; l_iPos < m_iTotalNodes ; l_iPos++)
    {
        //Set the Vector Clock Value to the received ones
        m_veciVectorClockNode[l_iPos] = p_stMsg.m_arriNodeVectorClock[l_iPos];
    }            

    //This is the Source of the broadcast from the Application Module
    for(int l_iPos = 0 ; l_iPos < m_veciTreeNeighbours.size() ; l_iPos++)
    {
        if(m_veciTreeNeighbours[l_iPos] != p_stMsg.m_iSrcNodeNo)
        {
            //Create Request Message
            APP_MSG_INFO_T l_stmsginfoMsg;

            //The type of message
            l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_BROADCAST;

            //Node Identification
            l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

            //Node Identification
            l_stmsginfoMsg.m_iDestNodeNo = m_veciTreeNeighbours[l_iPos];

            //Total No of Nodes
            l_stmsginfoMsg.m_iTotalNodes = m_iTotalNodes;

            //Update the individual values of the vector clock 
            for(int l_iPos1 = 0 ; l_iPos1 < m_iTotalNodes ; l_iPos1++)
            {
                //Get the Vector Values
                l_stmsginfoMsg.m_arriNodeVectorClock[l_iPos1] = 
                                        p_stMsg.m_arriNodeVectorClock[l_iPos1];
            } 
            
            //Client object
            CClient l_objClient;
            #if DEBUGPRINT
            std::cout<<"\n\n\n======================================================================\n"; 
            std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App at node "<<m_iNodeIndex<<" is "
                         <<"Sending APP Broadcast Message to "
                        <<m_veciTreeNeighbours[l_iPos]<<std::endl;
            std::cout<<"======================================================================\n\n\n";
            #endif
            //Add to the Broadcasted Node Vector
            m_veciBroadcastedNghbrs.push_back(m_veciTreeNeighbours[l_iPos]);

            //Init the client
            l_objClient.Init(m_objParser.GetAppPortNoByIndex(m_veciTreeNeighbours[l_iPos]-1),
                             m_objParser.GetNodeInfo
                             (m_veciTreeNeighbours[l_iPos]).m_strHostName);    

            //Send Message to the App Servers
            l_objClient.SendAppMsg(l_stmsginfoMsg); 
        }
    }

    //Last Node
    if(0 == m_veciBroadcastedNghbrs.size())
    {
        //Create Request Message
        APP_MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_BROADCAST_ACK;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = p_stMsg.m_iSrcNodeNo;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_iTotalNodes;
        
        //Client object
        CClient l_objClient;
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App at node "<<m_iNodeIndex<<" is "
                     <<"Sending APP Broadcast Ack to "
                    <<p_stMsg.m_iSrcNodeNo<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
        //Init the client
        l_objClient.Init(
            m_objParser.GetAppPortNoByIndex(m_iBroacastParentNodeIndex-1),
            m_objParser.GetNodeInfo(m_iBroacastParentNodeIndex).m_strHostName);    

        //Send Message to the App Servers
        l_objClient.SendAppMsg(l_stmsginfoMsg); 

        //ClearBroadcast
        ClearBroadcast();
    }
}

//------------------------------------------------------------------------------
//Msg Handler Broadcast Acknowledgement
void CAppModule :: MsgHndlrAppMsgBroadcastAck(APP_MSG_INFO_T p_stMsg)
{
    //std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Node "<<m_iNodeIndex
                   //     <<"Received Broadcast ACK from "<<p_stMsg.m_iSrcNodeNo
                      //                                          <<std::endl;

    //Increment the count of acknowledgements
    m_iNoofBroadcastAcks++;

    //If the No of responses received is equal to the no of requests sent then 
    // if it is root node finish the critical section activity
    if(m_veciTreeNeighbours.size() == m_iNoofBroadcastAcks && m_bBroadcastSource)
    {   
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n";
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"Broadcast completed which started at node "
                                                <<m_iNodeIndex<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
        //CS Task Finished True
        m_bIsCSTaskFinished = true;

        //ClearBroadcast
        ClearBroadcast();    
    
    }
    else if(m_veciBroadcastedNghbrs.size() == m_iNoofBroadcastAcks && 
            !m_bBroadcastSource && 0 != m_veciBroadcastedNghbrs.size())
    {
        
        //Create Request Message
        APP_MSG_INFO_T l_stmsginfoMsg;

        //The type of message
        l_stmsginfoMsg.m_eMsgType = APP_MSG_TYPE_E::eAPP_MSG_BROADCAST_ACK;

        //Node Identification
        l_stmsginfoMsg.m_iSrcNodeNo = m_iNodeIndex;

        //Node Identification
        l_stmsginfoMsg.m_iDestNodeNo = m_iBroacastParentNodeIndex;

        //Total No of Nodes
        l_stmsginfoMsg.m_iTotalNodes = m_iTotalNodes;
        
        //Client object
        CClient l_objClient;
        #if DEBUGPRINT
        std::cout<<"\n\n\n======================================================================\n"; 
        std::cout<<"DEBUG PRINT NODE :: "<<m_iNodeIndex<<" "<<"App at node "<<m_iNodeIndex<<" is "
                     <<"Sending APP Broadcast Ack to "
                    <<m_iBroacastParentNodeIndex<<std::endl;
        std::cout<<"======================================================================\n\n\n";
        #endif
        //Init the client
        l_objClient.Init(
            m_objParser.GetAppPortNoByIndex(m_iBroacastParentNodeIndex-1),
            m_objParser.GetNodeInfo(m_iBroacastParentNodeIndex).m_strHostName);

        //Send Message to the App Servers
        l_objClient.SendAppMsg(l_stmsginfoMsg); 

        //ClearBroadcast
        ClearBroadcast();
    }
}

//------------------------------------------------------------------------------
void CAppModule :: ClearBroadcast()
{
    //Reinit all braodcast related info
    m_bBroadcastSource = false;
    m_veciBroadcastedNghbrs.clear();
    m_iBroacastParentNodeIndex = -1;
    m_iNoofBroadcastAcks = 0;
}
//----------------------------End of File---------------------------------------

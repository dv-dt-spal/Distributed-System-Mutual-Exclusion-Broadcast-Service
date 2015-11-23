//------------------------------------------------------------------------------
//  File        :   CAppServer.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for Application Server using POSIX Socket
//------------------------------------------------------------------------------

//Headers
#include "CAppServer.h"
#include "CAppMessageQ.h"

//------------------------------------------------------------------------------
//Connection Thread handler
static void* AppConnectionThreadHndlr(void *p_pvParam)
{
    //Validdate argument
    if(NULL == p_pvParam)
    {
        std::cout<<"Invalid Argument in Connection Thread Hndlr"<<std::endl;
        return NULL;
    }

    //Get the socket descriptor
    int l_iSocketFileDesc = *(int*)(p_pvParam);
    //std::string l_strMessage = "Reply from Application Server";    

    //Reply to the client if needed
    //write(l_iSocketFileDesc ,l_strMessage.c_str(), l_strMessage.size()+1);     
     
    //Size of received message
    int l_iRcvdMsgReadSize;

    //Application Message to be Read from the Client
    APP_MSG_INFO_T l_stAppMsgInfo;

    l_iRcvdMsgReadSize = recv(l_iSocketFileDesc, 
                              &l_stAppMsgInfo , 
                              sizeof(l_stAppMsgInfo),0);

    //Print The Data Received
    //std::cout<<"################# Message Received ##################\n";
    //The type of message
    //std::cout<<"Message Type :: "<<l_stAppMsgInfo.m_eMsgType<<std::endl;

    //Node Identification
    //std::cout<<"Sending Source :: "<<l_stAppMsgInfo.m_iSrcNodeNo<<std::endl;

    //Node Identification
    //std::cout<<"Receiving Node :: "<<l_stAppMsgInfo.m_iDestNodeNo<<std::endl;

    //Host Name of the Node
    //for(int l_iPos = 0 ; l_iPos < l_stAppMsgInfo.m_iTotalNodes ; l_iPos++)
    //{
      //  std::cout<<"The Vector Clock of Process on Node :: "
      //              <<l_stAppMsgInfo.m_iSrcNodeNo<<" is :: "
      //              <<l_stAppMsgInfo.m_arriNodeVectorClock[l_iPos]<<std::endl;
    //}

    //Total Nodes
    //std::cout<<"Total Nodes :: "<<l_stAppMsgInfo.m_iTotalNodes<<std::endl;  

    //std::cout<<"###################################################\n";

    //Add the Message to the Node Message Queue
    CAppMessageQ::GetInstance()->AddMessage(l_stAppMsgInfo);

    //Receive a message from client
    //while(l_iRcvdMsgReadSize > 0)
    {
        //Process the Message Received
    }

    if(0 == l_iRcvdMsgReadSize)
    {
        //std::cout<<"App Client Disconnected"<<std::endl;
    }
    else if(-1 == l_iRcvdMsgReadSize)
    {
        //std::cout<<"App Receive Failed"<<std::endl;
    }

    //Thread Handler Return
    return NULL;
}

//------------------------------------------------------------------------------
//Constructor
CAppServer :: CAppServer()
{
    //Initialize the Member Variables
    //Port No for the Server
    m_iPortNo = -1;

    //Port No for the Server
    m_iSocketFileDesc = -1;

    //Host Name
    m_strHostName = "";    

    //Status
    m_bInitStatus = false;        
}

//------------------------------------------------------------------------------
//Destructor
CAppServer :: ~CAppServer()
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
bool CAppServer :: Init(int p_iPortNo,
                     std::string p_strHostName)
{
    //If already Init return false
    if(m_bInitStatus)
    {
        std::cout<<"App Server Init!!App Server Already Initialized"<<std::endl;
        return false;
    }

    //Store the parameters in the member variables
    //Port No for the Server
    m_iPortNo = p_iPortNo;

    //Host Name
    m_strHostName = p_strHostName;      

    //Create Socket
    m_iSocketFileDesc = socket(AF_INET, SOCK_STREAM, 0);

    //Check if Socket was Created    
    if (0 > m_iSocketFileDesc)
    {    
        std::cout<<"App Server ERROR opening socket"<<std::endl;
        return false;
    }

    //Socket Address Structure for Server
    struct sockaddr_in l_stsockaddrServAddr;

    //Prepare the Socket Address to bind with the socket created
    bzero((char *) &l_stsockaddrServAddr, sizeof(l_stsockaddrServAddr));

    //Prepare the structure for binding to the socket
    l_stsockaddrServAddr.sin_family = AF_INET;
    l_stsockaddrServAddr.sin_addr.s_addr = INADDR_ANY;
    l_stsockaddrServAddr.sin_port = htons(m_iPortNo);

    //Bind the Server Address to the Socket Created
    if (bind(m_iSocketFileDesc, (struct sockaddr *) &l_stsockaddrServAddr,
                                        sizeof(l_stsockaddrServAddr)) < 0) 
    {
        std::cout<<"App Server ERROR on binding"<<std::endl;
        return false;
    }

    //Listem to the incoming connection
    listen(m_iSocketFileDesc,5);
    
    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n";    
    std::cout<<"Application Server Initalized Node with Port No :: "<<m_iPortNo<<std::endl;   
    std::cout<<"======================================================================\n\n\n";
    #endif
    //Status
    m_bInitStatus = true; 

    //Socket Address Structure for Server
    struct sockaddr_in l_stsockaddrClientAddr;

    //Size of sockaddr_in
    int l_iSize = sizeof(l_stsockaddrClientAddr);

    //Socket Descriptor 
    int l_iSocketFileDescLoop;

    while(l_iSocketFileDescLoop = accept(m_iSocketFileDesc, 
                               (struct sockaddr *)&l_stsockaddrClientAddr, 
                               (socklen_t*)&l_iSize))
    {
        //Reply to the client if needed
        //write(new_socket , message , strlen(message));
        
        //Socket Descriptor 
        int l_iSocketFileDesc = l_iSocketFileDescLoop;
        
        //Thread Creation for the new connection
        CThread l_objThread;

        //Create a thread to handle the accept connection
        if(!(l_objThread.Create(&AppConnectionThreadHndlr,
                                                (void*)&l_iSocketFileDescLoop)))
        {
            std::cout<<"Connection Thread Failed"<<std::endl;
            return false;            
        }
        l_objThread.ThreadWait();
    }

    return true;
}

//------------------------------------------------------------------------------
//Close the Server
bool CAppServer :: Close()
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"App Server Close!! App Server Not yet Initialized"<<std::endl;
        return false;
    }

    //Close the Socket
    close(m_iSocketFileDesc);

    //ReInitialize the Member Variables
    //Port No for the Server
    m_iPortNo = -1;

    //Port No for the Server
    m_iSocketFileDesc = -1;

    //Host Name
    m_strHostName = "";    

    //Status
    m_bInitStatus = false;   
}

//------------------------------------------------------------------------------       
//Initialize the Server
void CAppServer :: Start()
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"App Server Start!!!Server Not yet Initialized"<<std::endl;
        return;
    }
}
//----------------------------End of File---------------------------------------

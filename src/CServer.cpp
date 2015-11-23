//------------------------------------------------------------------------------
//  File        :   CServer.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for Server using POSIX Socket
//------------------------------------------------------------------------------

//Headers
#include "CServer.h"
#include "CNodeMessageQ.h"

//------------------------------------------------------------------------------
//Connection Thread handler
static void* ConnectionThreadHndlr(void *p_pvParam)
{
    //Validdate argument
    if(NULL == p_pvParam)
    {
        std::cout<<"Invalid Argument in Connection Thread Hndlr"<<std::endl;
        return NULL;
    }

    //Get the socket descriptor
    int l_iSocketFileDesc = *(int*)(p_pvParam);
    //std::string l_strMessage = "Reply from Server";    

    //Reply to the client if needed
    //write(l_iSocketFileDesc ,l_strMessage.c_str(), l_strMessage.size()+1);     
     
    //Size of received message
    int l_iRcvdMsgReadSize;

    //Message to be Read from the Client
    MSG_INFO_T l_stMsgInfo;

    l_iRcvdMsgReadSize = recv(l_iSocketFileDesc, 
                              &l_stMsgInfo , 
                              sizeof(l_stMsgInfo),0);

    //Print The Data Received
    //std::cout<<"################# Message Received ##################\n";
    //The type of message
    //std::cout<<"Message Type :: "<<l_stMsgInfo.m_eMsgType<<std::endl;

    //Node Identification
    //std::cout<<"Sending Source :: "<<l_stMsgInfo.m_iSrcNodeNo<<std::endl;

    //Node Identification
    //std::cout<<"Receiving Node :: "<<l_stMsgInfo.m_iDestNodeNo<<std::endl;

    //Host Name of the Node
    //std::cout<<"Host Name :: "<<l_stMsgInfo.m_arrchSrcHostName<<std::endl;

    //Port number
    //std::cout<<"Port Number :: "<<l_stMsgInfo.m_iSrcPortNo<<std::endl;  

    //std::cout<<"###################################################\n";

    //Add the Message to the Node Message Queue
    CNodeMessageQ::GetInstance()->AddMessage(l_stMsgInfo);

    //Receive a message from client
    //while(l_iRcvdMsgReadSize > 0)
    {
        //Process the Message Received
    }

    if(0 == l_iRcvdMsgReadSize)
    {
        std::cout<<"System Client Disconnected"<<std::endl;
    }
    else if(-1 == l_iRcvdMsgReadSize)
    {
        std::cout<<"System Receive Failed"<<std::endl;
    }

    //Thread Handler Return
    return NULL;
}

//------------------------------------------------------------------------------
//Constructor
CServer :: CServer()
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
CServer :: ~CServer()
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
bool CServer :: Init(int p_iPortNo,
                     std::string p_strHostName)
{
    //If already Init return false
    if(m_bInitStatus)
    {
        std::cout<<"System Server Init!! System Server Already Initialized"<<std::endl;
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
        std::cout<<"System Server ERROR opening socket"<<std::endl;
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
        std::cout<<"System Server ERROR on binding"<<std::endl;
        return false;
    }

    //Listem to the incoming connection
    listen(m_iSocketFileDesc,5);

    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n";    
    std::cout<<"Server Initalized Node with Port No :: "<<m_iPortNo<<std::endl;   
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
        if(!(l_objThread.Create(&ConnectionThreadHndlr,
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
bool CServer :: Close()
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"System Server Close!! System Server Not yet Initialized"<<std::endl;
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
void CServer :: Start()
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"System Server Start!!Server Not yet Initialized"<<std::endl;
        return;
    }
}
//----------------------------End of File---------------------------------------

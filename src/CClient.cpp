//------------------------------------------------------------------------------
//  File        :   CClient.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for Client using POSIX Socket
//------------------------------------------------------------------------------

//Headers
#include "CClient.h"

//------------------------------------------------------------------------------
//Constructor
CClient :: CClient()
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
CClient :: ~CClient()
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
bool CClient :: Init(int p_iPortNo,
                     std::string p_strHostName)
{
    //If already Init return false
    if(m_bInitStatus)
    {
        std::cout<<"Client Already Initialized"<<std::endl;
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
        std::cout<<"Client ERROR opening socket for port "<<p_iPortNo<<std::endl;
        return false;
    }

    //Server Hostnet
    struct hostent * l_pobjServer;

    //Get the hostnet using the hostname
    l_pobjServer = gethostbyname(m_strHostName.c_str());

    //Check if server exists
    if (NULL == l_pobjServer)
    {
        std::cout<<"Host Does Not Exist"<<std::endl;
        return false;
    }

    //Socket Address Structure for Server
    struct sockaddr_in l_stsockaddrServAddr;

    //Prepare the Socket Address to bind with the socket created
    bzero((char *) &l_stsockaddrServAddr, sizeof(l_stsockaddrServAddr));

    //Prepare the structure for binding to the socket
    l_stsockaddrServAddr.sin_family = AF_INET;    
    bcopy((char *)l_pobjServer->h_addr, 
         (char *)&l_stsockaddrServAddr.sin_addr.s_addr,
         l_pobjServer->h_length);
    l_stsockaddrServAddr.sin_port = htons(m_iPortNo);

    //Connect to the Server
    if (connect(m_iSocketFileDesc,(struct sockaddr *) &l_stsockaddrServAddr,
                                        sizeof(l_stsockaddrServAddr)) < 0) 
    {
        std::cout<<"ERROR Connecting Server from Client of Port No"<<p_iPortNo<<std::endl;
        return false;
    }
    
    //Status
    m_bInitStatus = true; 
}

//------------------------------------------------------------------------------
//Close the Server
bool CClient :: Close()
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"Client Not yet Initialized"<<std::endl;
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
//Send Message to Server
bool CClient :: Send(MSG_INFO_T p_stMsgInfo)
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"Client System Message Send!!!Client Not yet Initialized "<<m_iPortNo<<std::endl;
        return false;
    }

    //Write to the Server
    if(write(m_iSocketFileDesc,&p_stMsgInfo,sizeof(p_stMsgInfo)) < 0)
    {
        std::cout<<"Error sending System message to server"<<std::endl;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------       
//Send Message to Server
bool CClient :: SendAppMsg(APP_MSG_INFO_T p_stMsgInfo)
{
    //If not Init return false
    if(!m_bInitStatus)
    {
        std::cout<<"Client App Message Send!!!Client Not yet Initialized "<<m_iPortNo<<std::endl;
        return false;
    }

    //Write to the Server
    if(write(m_iSocketFileDesc,&p_stMsgInfo,sizeof(p_stMsgInfo)) < 0)
    {
        std::cout<<"Error sending App message to App server"<<std::endl;
        return false;
    }
    return true;
}
//----------------------------End of File---------------------------------------

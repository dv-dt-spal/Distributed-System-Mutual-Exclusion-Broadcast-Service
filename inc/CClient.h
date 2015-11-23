//------------------------------------------------------------------------------
//  File        :   CClient.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for Client using POSIX Socket
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CCLIENT_H
#define CCLIENT_H

//Headers
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>
#include "CTypes.h"
#include "CThread.h"

//Class Description
class CClient
{    
    public:
        
        //Constructor
        CClient();

        //Destructor
        ~CClient();

        //Initialize the Server
        bool Init(int p_iPortNo,
                  std::string p_strHostName);

        //Close the Server
        bool Close();

        //Send
        bool Send(MSG_INFO_T p_stMsgInfo);

        //Send App Message
        bool SendAppMsg(APP_MSG_INFO_T p_stMsgInfo);
       
    private:   

        //Port No for the Server
        int m_iPortNo;

        //Port No for the Server
        int m_iSocketFileDesc;

        //Host Name
        std::string m_strHostName;    

        //Status
        bool m_bInitStatus;
};
#endif
//----------------------------End of File---------------------------------------

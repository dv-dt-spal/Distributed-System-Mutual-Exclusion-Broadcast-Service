//------------------------------------------------------------------------------
//  File        :   CServer.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for Server using POSIX Socket
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CSERVER_H
#define CSERVER_H

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
class CServer
{    
    public:
        
        //Constructor
        CServer();

        //Destructor
        ~CServer();

        //Initialize the Server
        bool Init(int p_iPortNo,
                  std::string p_strHostName);

        //Close the Server
        bool Close();
       
    private:

        //Initialize the Server
        void Start(); 

        //Thread Object to Listen on a New Thread
        CThread m_objThread;           

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

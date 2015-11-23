//------------------------------------------------------------------------------
//  File        :   CNodeMessageQ.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX Node Message Queue Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CNODE_MESSAGEQ_H
#define CNODE_MESSAGEQ_H

//Headers
#include <queue>
#include "CMutex.h"
#include "CSemaphore.h"
#include "CTypes.h"

//Class Description
class CNodeMessageQ
{
    public:

        //Getinstance method which returns CNodeMessageQ implements singleton
        //pattern. Since this class is going to be used by many threads
        //call this at the very earliest. Other solution would have been
        //Double checked locking which checks null twice and in between has a 
        //Critical section
        static CNodeMessageQ * GetInstance();

        //Destructor
        ~CNodeMessageQ(){}

        //Init the Node Message Queue
        void Init();

        //Close the Node Message Queue
        void Close();

        //Add Received Message to the Node Message Queue
        void AddMessage(MSG_INFO_T p_stMsgInfo);

        //Clear the Node Message Queue
        void Clear();

        //Pop Received Message to the Node Message Queue
        bool RemoveMessage(MSG_INFO_T& p_rstMsgInfo);

    private:

        //Constructor singleton class
        CNodeMessageQ(){}

        //Singleton object
        static CNodeMessageQ * m_pobjNodeMessageQ;

        //Queue to store the Message
        std::queue<MSG_INFO_T> m_qMessageQueue;

        //Mute Object to implement a thread safe queue
        CMutex m_objMutex;

        //Semaphore Object to enable multiple server threads to add message
        CSemaphore m_objSem;
        
        //Conditional Variable
        pthread_cond_t m_pthCond;
};
#endif
//----------------------------End of File---------------------------------------

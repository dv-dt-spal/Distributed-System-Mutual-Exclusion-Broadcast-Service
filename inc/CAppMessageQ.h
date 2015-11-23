//------------------------------------------------------------------------------
//  File        :   CAppMessageQ.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX App Message Queue Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef C_APP_MESSAGEQ_H
#define C_APP_MESSAGEQ_H

//Headers
#include <queue>
#include "CMutex.h"
#include "CSemaphore.h"
#include "CTypes.h"

//Class Description
class CAppMessageQ
{
    public:

        //Getinstance method which returns CAppMessageQ implements singleton
        //pattern. Since this class is going to be used by many threads
        //call this at the very earliest. Other solution would have been
        //Double checked locking which checks null twice and in between has a 
        //Critical section
        static CAppMessageQ * GetInstance();

        //Destructor
        ~CAppMessageQ(){}

        //Init the Node Message Queue
        void Init();

        //Close the Node Message Queue
        void Close();

        //Add Received Message to the Node Message Queue
        void AddMessage(APP_MSG_INFO_T p_stAppMsgInfo);

        //Clear the Node Message Queue
        void Clear();

        //Pop Received Message to the Node Message Queue
        bool RemoveMessage(APP_MSG_INFO_T& p_rstAppMsgInfo);

    private:

        //Constructor singleton class
        CAppMessageQ(){}

        //Singleton object
        static CAppMessageQ * m_pobjAppMessageQ;

        //Queue to store the Message
        std::queue<APP_MSG_INFO_T> m_qMessageQueue;

        //Mute Object to implement a thread safe queue
        CMutex m_objMutex;

        //Semaphore Object to enable multiple server threads to add message
        CSemaphore m_objSem;
        
        //Conditional Variable
        pthread_cond_t m_pthCond;
};
#endif
//----------------------------End of File---------------------------------------

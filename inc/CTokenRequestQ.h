//------------------------------------------------------------------------------
//  File        :   CTokenRequestQ.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX Token Request Queue Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CTOKEN_MESSAGEQ_H
#define CTOKEN_MESSAGEQ_H

//Headers
#include <queue>
#include "CMutex.h"
#include "CSemaphore.h"
#include "CTypes.h"

//Class Description
class CTokenRequestQ
{
    public:

        //Getinstance method which returns CTokenRequestQ implements singleton
        //pattern. Since this class is going to be used by many threads
        //call this at the very earliest. Other solution would have been
        //Double checked locking which checks null twice and in between has a 
        //Critical section
        static CTokenRequestQ * GetInstance();

        //Destructor
        ~CTokenRequestQ(){}

        //Init the Node Message Queue
        void Init();

        //Close the Node Message Queue
        void Close();

        //Add Received Message to the Node Message Queue
        void AddMessage(int p_iNodeRequestingToken);

        //Clear the Node Message Queue
        void Clear();

        //Pop Received Message to the Node Message Queue
        bool RemoveMessage(int& p_riNodeRequestingToken);

    private:

        //Constructor singleton class
        CTokenRequestQ(){}

        //Singleton object
        static CTokenRequestQ * m_pobjNodeMessageQ;

        //Queue to store the Message
        std::queue<int> m_qMessageQueue;

        //Mute Object to implement a thread safe queue
        CMutex m_objMutex;

        //Semaphore Object to enable multiple server threads to add message
        CSemaphore m_objSem;
        
        //Conditional Variable
        pthread_cond_t m_pthCond;
};
#endif
//----------------------------End of File---------------------------------------

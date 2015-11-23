//------------------------------------------------------------------------------
//  File        :   CThread.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX Thread Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CTHREAD_H
#define CTHREAD_H

//Headers
#include <pthread.h>
#include <iostream>

//Typedefs
//Thread Handler
typedef void *(*THREAD_HANDLER_T)(void*); 

//Class Description
class CThread
{    
    public:
        
        //Constructor
        CThread();

        //Destructor
        ~CThread();

        //Create a Thread
        bool Create(THREAD_HANDLER_T p_pthThreadHandler, void* p_pvParam);

        //Wait till thread finishes the task
        bool ThreadWait();

        //Stop the Thread
        bool Stop();       

    private:

        //Thread ID returned by POSIX Layer
		pthread_t m_pthThreadID;   

        //Status
        bool m_bInitStatus; 
};
#endif
//----------------------------End of File---------------------------------------

//------------------------------------------------------------------------------
//  File        :   CMutex.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX Mutex Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CMUTEX_H
#define CMUTEX_H

//Headers
#include <pthread.h>
#include <iostream>

//Class Description
class CMutex
{    
    public:
        
        //Constructor
        CMutex();

        //Destructor
        ~CMutex();

        //Initialize the mutex as PTHREAD_MUTEX_NORMAL
        bool Init();

        //Lock Mutex
        bool Lock();

        //Unlock the mutex
        bool UnLock();       

        //Destroy the mutex created
        bool Close();    
    
        //Mutex
	    pthread_mutex_t m_pmtxtMutex;

    private:
	    
	    //Mutex Name
	    std::string m_strMutexName;

        //Status
        bool m_bInitStatus; 
};
#endif
//----------------------------End of File---------------------------------------

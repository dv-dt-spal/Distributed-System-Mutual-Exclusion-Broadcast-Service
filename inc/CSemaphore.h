//------------------------------------------------------------------------------
//  File        :   CSemaphore.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for POSIX Semaphore Implementation
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H

//Headers
#include <semaphore.h>
#include <iostream>

//Class Description
class CSemaphore
{    
    public:
        
        //Constructor
        CSemaphore();

        //Destructor
        ~CSemaphore();

        //Init the semaphore
        bool Init(int p_iValue);

        //Wait till Critical section is available
        bool Wait();

        //Post will allow the next calling thread
        bool Post();

        //Destroy the Semaphore created
        bool Close();

    private:

        //Semaphore object
		sem_t m_semtSemaphore; 

        //Status
        bool m_bInitStatus; 
};
#endif
//----------------------------End of File---------------------------------------

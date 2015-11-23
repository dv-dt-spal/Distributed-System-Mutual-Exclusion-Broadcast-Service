//------------------------------------------------------------------------------
//  File        :   CSemaphore.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for POSIX Semaphore Implementation
//------------------------------------------------------------------------------

//Headers
#include "CSemaphore.h"

//------------------------------------------------------------------------------
//Constructor
CSemaphore :: CSemaphore()
{
	//Init Status
	m_bInitStatus = false;
}

//------------------------------------------------------------------------------
//Destructor
CSemaphore :: ~CSemaphore()
{
    //Check if still init close
    if(m_bInitStatus)
    {
        Close();
    }
}

//------------------------------------------------------------------------------
//Init the semaphore
bool CSemaphore :: Init(int p_iValue)
{
    //Check if already init
	if(m_bInitStatus)
	{
		//Return
        std::cout<<"Semaphore already initialized"<<std::endl;
		return false;
	}
	
	//Initsemaphore
    if(0 != sem_init(&m_semtSemaphore, 0, p_iValue))
	{
		//Return
        std::cout<<"Semaphore failed to initialize"<<std::endl;
		return false;
	}	
	
	//semaphore initialised
	m_bInitStatus = true;

    //Success
    return true;
}

//------------------------------------------------------------------------------
//Wait till Critical section is available
bool CSemaphore :: Wait()
{
	//Check if semaphore init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Semaphore not yet initialized"<<std::endl;
		return false;
	}
	
    //Wait till Critical Section is available
	if(0 != sem_wait(&m_semtSemaphore))
	{
		//Return
        std::cout<<"Semaphore failed to wait"<<std::endl;
		return false;
	}
	
	//Success
    return true;
}

//------------------------------------------------------------------------------
//Post will allow the next calling thread
bool CSemaphore :: Post()
{
	//Check if semaphore init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Semaphore not yet initialized"<<std::endl;
		return false;
	}
	
	//Post that critical section is now available
	if(0 != sem_post(&m_semtSemaphore))
	{
		//Return
        std::cout<<"Semaphore failed to post"<<std::endl;
		return false;
	}
	
	//Success
    return true;
}

//------------------------------------------------------------------------------
//Destroy the Semaphore created
bool CSemaphore :: Close()
{
	//Check if semaphore init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Semaphore not yet initialized"<<std::endl;
		return false;
	}
	
	//semaphore destroy
	if(0 != sem_destroy(&m_semtSemaphore))
	{
		//Return
        std::cout<<"Semaphore failed to destroy"<<std::endl;
		return false;
	}
	
	//Semaphore destroyed
	m_bInitStatus = false;

	//Success
    return true;;  
}
//----------------------------End of File---------------------------------------

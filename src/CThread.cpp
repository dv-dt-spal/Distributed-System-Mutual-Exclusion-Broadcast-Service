//------------------------------------------------------------------------------
//  File        :   CThread.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for POSIX Thread Implementation
//------------------------------------------------------------------------------

//Headers
#include "CThread.h"

//------------------------------------------------------------------------------
//Constructor
CThread :: CThread()
{
    //Initialise thread ID and status
	m_pthThreadID = -1;
    m_bInitStatus = false;
}

//------------------------------------------------------------------------------
//Destructor
CThread :: ~CThread()
{
    //Reinitialise thread ID and status
	m_pthThreadID = -1;
    m_bInitStatus = false;
    //Check if still init close
    if(m_bInitStatus)
    {
        Stop();
    }
}

//------------------------------------------------------------------------------
//Create a Thread
bool CThread :: Create(THREAD_HANDLER_T p_pthThreadHandler, void* p_pvParam)
{
    //Validate parameters
	if(NULL == p_pthThreadHandler)
	{	
		//Return
        std::cout<<"Invalid Thread Handler"<<std::endl;
		return false;
	}
	
	//Check if already init
	if(m_bInitStatus)
	{
		//Return
        std::cout<<"Thread is already created"<<std::endl;
		return false;        
	}
	
    //Create Thread
	int l_iRetValue = pthread_create(&m_pthThreadID,
									  NULL,
									  p_pthThreadHandler, 
									  p_pvParam);
									  
    //Check the Value returned by POSIX API
	if(0 != l_iRetValue)
	{
		//Return
        std::cout<<"Thread creation failed"<<std::endl;
		return false;
	}

	//Create Thread
	return true;  
}

//------------------------------------------------------------------------------
//Wait till thread finishes the task
bool CThread :: ThreadWait()
{
	//Check if already init
	if(m_bInitStatus)
	{
		//Return
        std::cout<<"Thread is already created"<<std::endl;
		return false;        
	}
	
	//Thread Wait
	int l_iRetValue = pthread_join(m_pthThreadID, NULL);
	if(0 != l_iRetValue)
	{
		//Return
        std::cout<<"Thread wait failed"<<std::endl;
		return false;
	}
	
	//Thread completed the task
	return true; 
}

//------------------------------------------------------------------------------
//Stop the Thread
bool CThread :: Stop()
{
	//Check if thread is created
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Thread not initialized"<<std::endl;
		return false;
	}
	
	//Thread Stop
	int l_iRetValue = pthread_cancel(m_pthThreadID);
	if(0 != l_iRetValue)
	{
		//Return
        std::cout<<"Thread stopping failed"<<std::endl;
		return false;
	}
	
	//Wait for the thread to complete
	if(!ThreadWait())
	{
		//ReInitialise ThreadID
    	m_pthThreadID = -1;
        std::cout<<"Thread stopping failed1"<<std::endl;
        return false;
	}
	
	//ReInitialise ThreadID
	m_pthThreadID = -1;

	//Thread Stopped
	return true; 
}
//----------------------------End of File---------------------------------------

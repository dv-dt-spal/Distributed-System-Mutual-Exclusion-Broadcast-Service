//------------------------------------------------------------------------------
//  File        :   CMutex.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for POSIX Mutex Implementation
//------------------------------------------------------------------------------

//Headers
#include "CMutex.h"

//------------------------------------------------------------------------------   
//Constructor
CMutex :: CMutex()
{
	//Init Status
	m_bInitStatus = false;
}

//------------------------------------------------------------------------------
//Destructor
CMutex :: ~CMutex()
{
    //Check if still init close
    if(m_bInitStatus)
    {
        Close();
    }
}

//------------------------------------------------------------------------------
//Initialize the mutex as PTHREAD_MUTEX_NORMAL
bool CMutex :: Init()
{
    //Check if already init
	if(m_bInitStatus)
	{
		//Return
        std::cout<<"Mutex already initialized"<<std::endl;
		return false;
	}
	
	//Local Mutex attribute
	pthread_mutexattr_t l_pthmtxtAttribute;

	//Init attribute
	if(0 != pthread_mutexattr_init(&l_pthmtxtAttribute))
	{
		//Return
        std::cout<<"Mutex failed to initialize"<<std::endl;
		return false;
	}	
	
	//Type of mutex
	if(0 != pthread_mutexattr_settype(&l_pthmtxtAttribute,
    								  PTHREAD_MUTEX_NORMAL))
	{
		//Return
        std::cout<<"Mutex Type Setting Failed"<<std::endl;
		pthread_mutexattr_destroy(&l_pthmtxtAttribute);	
    	return false;
	}
	
    //Init Mutex
	if(0 != pthread_mutex_init(&m_pmtxtMutex, &l_pthmtxtAttribute))
	{
		//Return
        std::cout<<"Mutex Init Failed"<<std::endl;
		pthread_mutexattr_destroy(&l_pthmtxtAttribute);	
    	return false;
	}
	
    //Set the init to true
	m_bInitStatus = true;

	//Destroy the local attribute variable
	pthread_mutexattr_destroy(&l_pthmtxtAttribute);

	//Success
	return true;
}  

//------------------------------------------------------------------------------
//Lock Mutex
bool CMutex :: Lock()
{
	//Check if mutex init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Mutex not initialized"<<std::endl;
		return false;
	}
	
    //Lock the mutex
	if(0 != pthread_mutex_lock(&m_pmtxtMutex))
	{
		//Return
        std::cout<<"Mutex lock failed"<<std::endl;
		return false;
	}

    //Success
    return true; 
}

//------------------------------------------------------------------------------
//Unlock the mutex
bool CMutex :: UnLock()
{
	//Check if mutex init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Mutex not initialized"<<std::endl;
		return false;
	}

	//Unlock the mutex
	if(0 != pthread_mutex_unlock(&m_pmtxtMutex))
	{
		//Return
        std::cout<<"Mutex unlock failed"<<std::endl;
		return false;
	}
	
    //Success
    return true; 
}       

//------------------------------------------------------------------------------
//Destroy the mutex created
bool CMutex :: Close()
{
	//Check if mutex init
	if(!m_bInitStatus)
	{
		//Return
        std::cout<<"Mutex not initialized"<<std::endl;
		return false;
	}
	
	//Safe destruction of mutex
	pthread_mutex_lock(&m_pmtxtMutex);
	pthread_mutex_unlock(&m_pmtxtMutex);

									  
	//Destroy Mutex
	if(0 != pthread_mutex_destroy(&m_pmtxtMutex))
	{
		//Return
        std::cout<<"Mutex destroy failed"<<std::endl;
		return false;
	}
	
	//Mutex destroyed
	m_bInitStatus = false;

    //Success
    return true; 
}    
//----------------------------End of File---------------------------------------

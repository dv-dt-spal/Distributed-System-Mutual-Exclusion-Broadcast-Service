//------------------------------------------------------------------------------
//  File        :   CTokenRequestQ.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   source file for POSIX Token Request Queue Implementation
//------------------------------------------------------------------------------

//Headers
#include "CTokenRequestQ.h"

//Init the static varibale
CTokenRequestQ* CTokenRequestQ::m_pobjNodeMessageQ = NULL;

//------------------------------------------------------------------------------
//Get single instance of CTokenRequestQ
CTokenRequestQ * CTokenRequestQ::GetInstance()
{
    //Check if not already created
    if (m_pobjNodeMessageQ == NULL)
    {
        //create one and only object
        m_pobjNodeMessageQ = new CTokenRequestQ();
    }

    //Return the object pointer
    return m_pobjNodeMessageQ;
}

//------------------------------------------------------------------------------
//Init the Node Message Queue
void CTokenRequestQ :: Init()
{
    //Init Mutex
    if(!m_objMutex.Init())
    {
        std::cout<<"Mutex Init Failed"<<std::endl;
    }

    //Init Semaphore
    if(!m_objSem.Init(1))
    {
        std::cout<<"Sem init Failed"<<std::endl;
    }

    //Init the conditional variable
    pthread_cond_init(&m_pthCond, 0);
}

//------------------------------------------------------------------------------
//Close the Node Message Queue
void CTokenRequestQ :: Close()
{
    //Destroy the conditional variable
    pthread_cond_destroy(&m_pthCond);
    
    //Close Semaphore
    m_objSem.Close();

    //Close Mutex
    m_objMutex.Close();            
}

//------------------------------------------------------------------------------
//Add Received Message to the Node Message Queue
void CTokenRequestQ :: AddMessage(int p_iNodeRequestingToken)
{
    //Critical Section by calling sem wait
    if(!m_objSem.Wait())
    {   
        std::cout<<"Sem Wait Failed"<<std::endl;
    }   

    //Guard the Queue
    if(!m_objMutex.Lock())
    {
        std::cout<<"Mutex Lock Failed"<<std::endl;    
    }

    //Add Data to the Queue
    m_qMessageQueue.push(p_iNodeRequestingToken);

    //Signal waiting thread
    pthread_cond_signal(&m_pthCond);

    //Unlock the queue or remove guard
    if(!m_objMutex.UnLock())
    {
        std::cout<<"Mutex UnLock Failed"<<std::endl;    
    }

    //Notify others
    if(!m_objSem.Post())
    {
        std::cout<<"Sem Post Failed"<<std::endl;
    }
}

//------------------------------------------------------------------------------
//Clear the Node Message Queue
void CTokenRequestQ :: Clear()
{
    //Guard the Queue
    m_objMutex.Lock();

    //Clear the Queue
    m_qMessageQueue.empty();

    //Unlock the queue or remove guard
    m_objMutex.UnLock();
}

//------------------------------------------------------------------------------
//Pop Received Message to the Node Message Queue
bool CTokenRequestQ :: RemoveMessage(int& p_riNodeRequestingToken)
{
    //Guard the Queue
    if(!m_objMutex.Lock())
    {
        std::cout<<"Mutex Lock Failed"<<std::endl;    
        return false;
    }

    //Check if Queue is empty
    if(m_qMessageQueue.empty())
    {
        //Wait till queue has item
        pthread_cond_wait(&m_pthCond, &(m_objMutex.m_pmtxtMutex));
    }
    
    //Add data to the reference
    p_riNodeRequestingToken = m_qMessageQueue.front();
    
    //Delete from Queue
    m_qMessageQueue.pop();

    //Unlock the queue or remove guard
    if(!m_objMutex.UnLock())
    {
        std::cout<<"Mutex UnLock Failed"<<std::endl;    
    }

    //Success
    return true;
}
//----------------------------End of File---------------------------------------

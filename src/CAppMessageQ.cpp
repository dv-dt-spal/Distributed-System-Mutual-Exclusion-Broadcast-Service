//------------------------------------------------------------------------------
//  File        :   CAppMessageQ.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   source file for POSIX App Message Queue Implementation
//------------------------------------------------------------------------------

//Headers
#include "CAppMessageQ.h"

//Init the static varibale
CAppMessageQ* CAppMessageQ::m_pobjAppMessageQ = NULL;

//------------------------------------------------------------------------------
//Get single instance of CAppMessageQ
CAppMessageQ * CAppMessageQ::GetInstance()
{
    //Check if not already created
    if (m_pobjAppMessageQ == NULL)
    {
        //create one and only object
        m_pobjAppMessageQ = new CAppMessageQ();
    }

    //Return the object pointer
    return m_pobjAppMessageQ;
}

//------------------------------------------------------------------------------
//Init the Node Message Queue
void CAppMessageQ :: Init()
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
void CAppMessageQ :: Close()
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
void CAppMessageQ :: AddMessage(APP_MSG_INFO_T p_stAppMsgInfo)
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
    m_qMessageQueue.push(p_stAppMsgInfo);

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
void CAppMessageQ :: Clear()
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
bool CAppMessageQ :: RemoveMessage(APP_MSG_INFO_T& p_rstAppMsgInfo)
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
    p_rstAppMsgInfo = m_qMessageQueue.front();
    
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

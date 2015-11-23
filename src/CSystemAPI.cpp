//------------------------------------------------------------------------------
//  File        :   CSystemAPI.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for System API Module. 
//                  Provides CS_Enter and CS_Exit API to the Application module
//------------------------------------------------------------------------------

//Headers
#include "CSystemAPI.h"

//------------------------------------------------------------------------------        
//Constructor
CSystemAPI :: CSystemAPI(CNode * l_pobjNode)
{
    //Set the Node Object
    m_pobjNode = l_pobjNode;
}

//------------------------------------------------------------------------------
//Destructor
CSystemAPI :: ~CSystemAPI()
{
    //Make the Node object as null
    m_pobjNode = NULL;
}

//------------------------------------------------------------------------------
//API for App to Request CS
void CSystemAPI :: CS_Enter()
{
    //Validate the Node Paramter
    if(NULL == m_pobjNode)
    {
        std::cout<<"Invalid Node Class Object! Can't Execute CS"<<std::endl;
        return;
    }
    
    //Call the Node CS_Enter
    m_pobjNode->CS_Enter();

}

//------------------------------------------------------------------------------
//API for App to Notify CS Exit
void CSystemAPI :: CS_Exit()
{
    //Validate the Node Paramter
    if(NULL == m_pobjNode)
    {
        std::cout<<"Invalid Node Class Object! Can't Update CS Exit"<<std::endl;
        return;
    }

    //Call the Node CS_Enter
    m_pobjNode->CS_Exit();
}
//----------------------------End of File---------------------------------------

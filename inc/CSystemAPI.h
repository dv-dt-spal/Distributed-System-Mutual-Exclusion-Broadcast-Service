//------------------------------------------------------------------------------
//  File        :   CSystemAPI.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for System API Module. 
//                  Provides CS_Enter and CS_Exit API to the Application module
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CSYSTEM_API_H
#define CSYSTEM_API_H

//Headers
#include <iostream>
#include "CNode.h"

//Class Description
class CSystemAPI
{    
    public:
        
        //Constructor
        CSystemAPI(CNode * l_pobjNode);

        //Destructor
        ~CSystemAPI();
        
        //API for App to Request CS
        void CS_Enter();

        //API for App to Notify CS Exit
        void CS_Exit();

    private:

        //Node Object to Call CS_Enter and Exit
		CNode * m_pobjNode;   

};
#endif
//----------------------------End of File---------------------------------------

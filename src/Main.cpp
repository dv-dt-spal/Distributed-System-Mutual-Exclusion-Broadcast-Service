//------------------------------------------------------------------------------
//  File        :   Main.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for project main file
//------------------------------------------------------------------------------

//Headers
#include "CParser.h"
#include "CThread.h"
#include "CServer.h"
#include "CAppServer.h"
#include "CClient.h"
#include "CSystemAPI.h"
#include "CAppModule.h"
#include <unistd.h>

//------------------------------------------------------------------------------
//Function Declaration for System Server Thread
void StartServer(int);
static void* ServerThreadHandler(void* p_pvParam);

//------------------------------------------------------------------------------
//Function Declaration for Application Server Thread
void StartAppServer(int);
static void* AppServerThreadHandler(void* p_pvParam);

//------------------------------------------------------------------------------
//Parser Object
CParser g_objParser;

//------------------------------------------------------------------------------
//Main Function
int main(int argc,char* argv[])
{
    
    #if DEBUGPRINT
    std::cout<<"\n\n\n======================================================================\n";    
    std::cout<<"Starting Node......."<<(atoi(argv[1]))+1<<std::endl;
    std::cout<<"======================================================================\n\n\n";;
    #endif
    //Init the Parser
    g_objParser.Init();   

    //Init the singleton class Node Message Queue - eager instantiation
    CNodeMessageQ::GetInstance()->Init();

    //Init the singleton class App Message Queue - eager instantiation
    CAppMessageQ::GetInstance()->Init();

    //Init the singleton class Token Request Queue - eager instantiation
    CTokenRequestQ::GetInstance()->Init();

    //Thread for server
    CThread l_objThread;

    //Thread for server
    CThread l_objAppServerThread;

    //Current node 
    int l_iCurrNode = atoi(argv[1]);

    //Create a thread to handle the accept connection
    if(!(l_objThread.Create(&ServerThreadHandler,
                                            (void*)&l_iCurrNode)))
    {
        std::cout<<"Server Thread Creation Failed"<<std::endl;
        return 0;            
    }

    //Create a thread to handle the application server accept connection
    if(!(l_objAppServerThread.Create(&AppServerThreadHandler,
                                            (void*)&l_iCurrNode)))
    {
        std::cout<<"Application Server Thread Creation Failed"<<std::endl;
        return 0;            
    }    

    sleep(8);

    //Node 
    CNode l_objNode;

    //Application
    CAppModule l_objAppModule;
    
    //System API
    CSystemAPI l_objSystemAPI(&l_objNode);

    //Start the System Module of Node
    l_objNode.Init(l_iCurrNode + 1);

    //Start the Application Node
    l_objAppModule.Init(l_iCurrNode + 1,&l_objSystemAPI);

    while(1){}
    l_objAppModule.Close();
    l_objNode.Close();

    return 0;
}

//------------------------------------------------------------------------------
//Start System Module Server
void StartServer(int p_iNodeNo)
{
    CServer l_objServer;
    l_objServer.Init(g_objParser.GetNodeInfo(p_iNodeNo+1).m_iPortNo,
                     g_objParser.GetNodeInfo(p_iNodeNo+1).m_strHostName);    
}

//------------------------------------------------------------------------------
//Start Application Module Server
void StartAppServer(int p_iNodeNo)
{
    CAppServer l_objAppServer;
    l_objAppServer.Init(g_objParser.GetAppPortNoByIndex(p_iNodeNo),
                        g_objParser.GetNodeInfo(p_iNodeNo+1).m_strHostName);    
}

//------------------------------------------------------------------------------
//System Module Server Thread HAndler    
static void* ServerThreadHandler(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
    
    int l_iNode = *(int *)(p_pvParam);
    StartServer(l_iNode);  
    return NULL;
}

//------------------------------------------------------------------------------
//Application Module Server Thread HAndler    
static void* AppServerThreadHandler(void* p_pvParam)
{
    if(NULL == p_pvParam)
    {
        return NULL;
    }
    
    int l_iNode = *(int *)(p_pvParam);
    StartAppServer(l_iNode);  
    return NULL;
}
//----------------------------End of File---------------------------------------

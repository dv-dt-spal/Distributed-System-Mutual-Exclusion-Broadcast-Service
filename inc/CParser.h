//------------------------------------------------------------------------------
//  File        :   CParser.h
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Header file for parser to read data from config file
//------------------------------------------------------------------------------

//Guard for multiple inclusion
#ifndef CPARSER_H
#define CPARSER_H

//Headers
#include <iostream>
#include "CTypes.h"
#include <map>
#include <fstream>
#include <vector>
#include <sstream>

//Defines
#define CONFIGFILE "ConfigurationFile.txt"

//Class Description
class CParser
{    
    public:
        
        //Constructor
        CParser();

        //Destructor
        ~CParser();

        //Init the File Class
        void Init();
        
        //Close the File Class
        void Close();

        //Get Node Information by index
        NODE_INFO_T GetNodeInfo(int p_iIndex);

        //Get Node Information by index
        int GetTotalNodes(){return m_iTotalNodes;}

        //Get Root Node Index
        int GetRootNodeIndex(){return m_iRootNode;}
        
        //Get total broadcast nodes
        int GetTotalBroadcastNodes(){return m_iTotalBroadcastNodes;}

        //Get broadcast node info
        BROADCAST_INFO_T GetBroadcastNodeInfo(){return m_stBroadcastInfo;}

        //-------------------------Project 2------------------------------------

        //Get Total Number of CS Requests per Node
        int GetTotalCSRequests(){ return m_iTotalCSRequests; }

        //Get Mean delay between two CS execution
        float GetMeanDelayBetnCS(){ return m_fMeanDelayBetnCS; }

        //Get Mean Duration of CS
        float GetMeanDurationCS(){ return m_fMeanDurationCS; }

        //Get App Port Nos
        std::vector<int> GetAppPortNos(){return m_veciAppPortNos;}

        //Get App Port Nos by Index
        int GetAppPortNoByIndex(int p_iIndex)
                                        {return m_veciAppPortNos[p_iIndex];}

    private:
        
        //Parse the file 
        void Parse();

        //Trim the line
        std::string& Trim(std::string& l_strData);

        //File Object
        std::fstream m_fileobj;

        //Init status
        bool m_bInitStatus;

        //Root Node 
        int m_iRootNode;

        //Total Nodes
        int m_iTotalNodes;

        //Total Broadcast Nodes
        int m_iTotalBroadcastNodes;

        //Broadcast info
        BROADCAST_INFO_T m_stBroadcastInfo;

        //Map to store the Node information based on the config file
        std::map<int,NODE_INFO_T> m_mapNodeInfo;

        //-------------------------Project 2------------------------------------
        //Total Number of CS Requests per Node
        int m_iTotalCSRequests;

        //Mean delay between two CS execution
        float m_fMeanDelayBetnCS;

        //Mean Duration of CS
        float m_fMeanDurationCS;

        //Vector of Application Port Numbers
        std::vector<int> m_veciAppPortNos;
};
#endif
//----------------------------End of File---------------------------------------

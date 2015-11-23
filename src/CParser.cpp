//------------------------------------------------------------------------------
//  File        :   CParser.cpp
//  Author      :   Swaroop Pal Harsh Desai
//  Description :   Source file for parser to read data from config file
//------------------------------------------------------------------------------

//Headers
#include "CParser.h"

//------------------------------------------------------------------------------
//Constructor
CParser :: CParser()
{
   //Init status
    m_bInitStatus = false;

    //Root Node 
    m_iRootNode = -1;

    //Total Nodes
    m_iTotalNodes = 0;

    //Total Broadcast Nodes
    m_iTotalBroadcastNodes = 0;

    //Total Number of CS Requests per Node
    m_iTotalCSRequests = 0;

    //Mean delay between two CS execution
    m_fMeanDelayBetnCS = 0;

    //Mean Duration of CS
    m_fMeanDurationCS = 0;

    //Map to store the Node information based on the config file
    m_mapNodeInfo.clear();
}

//------------------------------------------------------------------------------
//Destructor
CParser :: ~CParser()
{
    //Check if still init close
    if(m_bInitStatus)
    {
        Close();
    }
}

//------------------------------------------------------------------------------
//Init the File Class
void CParser :: Init()
{
    //Check if still init close
    if(m_bInitStatus)
    {
        std::cout<<"Parser already initialized"<<std::endl;
        return;
    }

    //Open the config file for parsing
    m_fileobj.open(CONFIGFILE, std::fstream::in);

    //Set the init status
    m_bInitStatus = true;    

    //Parse
    Parse();
}

//------------------------------------------------------------------------------
//Close the File Class
void CParser :: Close()
{
    //Check if not init return
    if(!m_bInitStatus)
    {
        std::cout<<"Parser not initialized"<<std::endl;
        return;
    }

    //File Object
    if(m_fileobj.is_open())
    {
        //Close the File
        m_fileobj.close();
    }

    //Init status
    m_bInitStatus = false;

    //Root Node 
    m_iRootNode = -1;

    //Total Nodes
    m_iTotalNodes = 0;

    //Total Broadcast Nodes
    m_iTotalBroadcastNodes = 0;

    //Total Number of CS Requests per Node
    m_iTotalCSRequests = 0;

    //Mean delay between two CS execution
    m_fMeanDelayBetnCS = 0;

    //Mean Duration of CS
    m_fMeanDurationCS = 0;

    //Map to store the Node information based on the config file
    m_mapNodeInfo.clear();
}

//------------------------------------------------------------------------------
//Get Node Information by index
NODE_INFO_T CParser :: GetNodeInfo(int p_iIndex)
{
    //Check if not init return
    if(m_bInitStatus)
    {
        //Validate the parameter and init status
        if(0 < p_iIndex)
        {
            //Return the Node information based on the index
            return m_mapNodeInfo.find(p_iIndex)->second;
        }
        else
        {
            std::cout<<"Invalid Index"<<std::endl;
        }
    }
    else
    {
        std::cout<<"Parser not initialized"<<std::endl;     
    }   
}

//------------------------------------------------------------------------------       
//Trim the lines read from file 
std::string& CParser :: Trim(std::string& l_strData)
{
    const char* l_pcTrimChar = " \t\n\r\f\v";
    l_strData.erase(l_strData.find_last_not_of(l_pcTrimChar) + 1);
    l_strData.erase(0, l_strData.find_first_not_of(l_pcTrimChar));
    return l_strData;
}

//------------------------------------------------------------------------------       
//Read from the file 
void CParser :: Parse()
{
    //Check if not init return
    if(!m_bInitStatus)
    {
        std::cout<<"Parser not initialized"<<std::endl;
        return;
    }
    
    //Local String
    std::string l_strLine;

    //Loop to extract Node information from the file
    while (std::getline(m_fileobj, l_strLine))
    {
        //std::cout<<l_strLine<<std::endl;
        l_strLine = Trim(l_strLine);

        //Parse the Total no of nodes
        if(l_strLine.compare("#TotalNode") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_iTotalNodes = atoi(l_strLine.c_str());
        }

        //Parse the root node
        if(l_strLine.compare("#RootNode") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_iRootNode = atoi(l_strLine.c_str());            
        }

        //Parse the entire system topology
        if(l_strLine.compare("#Topology") == 0)
        {
            //Loop for extracting node wise information and store in map
            for(int l_iPos = 1;l_iPos <= m_iTotalNodes;l_iPos++)
            {
                //Parse the entire line for Individual Node info
                std::getline(m_fileobj, l_strLine);
                l_strLine = Trim(l_strLine);

                std::vector<std::string> l_vecstrLine;
                std::stringstream l_ssLine(l_strLine);
                std::string l_strData;

                //Loop to get the string in line delimited by space
                while (l_ssLine >> l_strData)
                {
                    //Push it to the vector for creating node info structure                
                    l_vecstrLine.push_back(l_strData);

                    //Split by string
                    if (l_ssLine.peek() == ' ')
                        l_ssLine.ignore();
                }
        
                //Parse for the Negihbouring Nodes
                std::string l_strNodeNghbrData = l_vecstrLine[2];
                std::vector<int> l_veciNghbrNodes;
                std::stringstream l_ssNghbrNodes(l_strNodeNghbrData);
                int l_iNodeIndex;

                //Loop to get the integer in string delimited by comma
                while (l_ssNghbrNodes >> l_iNodeIndex)
                {
                    //Push to the vector for creating node info structure
                    l_veciNghbrNodes.push_back(l_iNodeIndex);

                    //Split by comma
                    if (l_ssNghbrNodes.peek() == ',')
                        l_ssNghbrNodes.ignore();
                }

                //Create the structure for individual node information
                NODE_INFO_T * l_pstNodeInfo;
                l_pstNodeInfo = new NODE_INFO_T;

                //Fill the structure
                //Node Identification
                l_pstNodeInfo->m_iNodeNo = l_iPos;

                //Host Name of the Node
                l_pstNodeInfo->m_strHostName = l_vecstrLine[0];

                //Port number
                l_pstNodeInfo->m_iPortNo = atoi(l_vecstrLine[1].c_str());

                //The neighbour nodes of this particular node 
                l_pstNodeInfo->m_veciNodeNghbrs = l_veciNghbrNodes;

                //Total No of Nodes
                l_pstNodeInfo->m_iTotalNodes = m_iTotalNodes;

                //Root Node if Index is same as the one specified in config file
                if(l_iPos == m_iRootNode)
                {
                    l_pstNodeInfo->m_bRootNode = true;
                }
                else
                {
                    l_pstNodeInfo->m_bRootNode = false;
                }

                //Insert the strucutre<value> and the index<key> into the map 
                m_mapNodeInfo.
                    insert(std::pair<int,NODE_INFO_T>(l_iPos,*l_pstNodeInfo));
            } 
        }

        //Parse the Total no of nodes
        if(l_strLine.compare("#TotalBroadcast") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_iTotalBroadcastNodes = atoi(l_strLine.c_str());
        }

        //Parse the entire broadcast list
        if(l_strLine.compare("#BroadcastSource") == 0)
        {
            //Set the total broadcasting nodes
            m_stBroadcastInfo.m_iTotalBroadcastingNodes =m_iTotalBroadcastNodes;

            //Loop for extracting node wise information and store in map
            for(int l_iPos = 1;l_iPos <= m_iTotalBroadcastNodes;l_iPos++)
            {
                std::getline(m_fileobj, l_strLine);
                l_strLine = Trim(l_strLine);
                m_stBroadcastInfo.m_qiBroadcastSource
                                                .push(atoi(l_strLine.c_str()));
            }
        }

        //Parse the Total Number of CS Requests per Node
        if(l_strLine.compare("#NumberOfCSReqPerNode") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_iTotalCSRequests = atoi(l_strLine.c_str());
        }

        //Parse the Mean delay between two CS execution
        if(l_strLine.compare("#MeanDelayBetweenCS") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_fMeanDelayBetnCS = atoi(l_strLine.c_str());
        }

        //Parse theMean Duration of CS
        if(l_strLine.compare("#MeanDurationCS") == 0)
        {
            std::getline(m_fileobj, l_strLine);
            l_strLine = Trim(l_strLine);
            m_fMeanDurationCS = atoi(l_strLine.c_str());
        }

        //Parse the Vector of Application Port Numbers
        if(l_strLine.compare("#PortNosApp") == 0)
        {
            //Loop for extracting node wise information and store in map
            for(int l_iPos = 1;l_iPos <= m_iTotalNodes;l_iPos++)
            {
                std::getline(m_fileobj, l_strLine);
                l_strLine = Trim(l_strLine);
                m_veciAppPortNos.push_back(atoi(l_strLine.c_str()));
            }
            break;
        }
      
    }
}
//----------------------------End of File---------------------------------------

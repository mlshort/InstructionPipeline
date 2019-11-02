/**
* @file       DependencyGraph.cpp
* @brief      CDependencyGraph class implementation
*
*
* @author     Mark L. Short
* @date       November 23, 2014
*/

#include "stdafx.h"
#include "DependencyGraph.h"
#include <locale>


bool CGraphNode::AddEdge ( const NODE_ID_T& idToNode, int iWeight )
{
    return AddEdge( CDirectedEdgeData(idToNode, iWeight) );
}

bool CGraphNode::AddEdge ( const CDirectedEdgeData& edge )
{
    bool bReturn = false;

    if ( IsValid ( ) ) // check to make sure we are a valid node before assigning any edges
    {
        _Pairib Result = m_setEdges.insert ( edge );

        if ( Result.second )  // was the insert successful ?
        {
            bReturn = true;
        }
    }

    return bReturn;
}

const size_t DEFAULT_MAX_NODES = 10;

CDependencyGraph::CDependencyGraph ( )
    : m_nMaxNodes(DEFAULT_MAX_NODES), 
      m_nNumNodes(0),
      m_vNodes(DEFAULT_MAX_NODES)
{
}

CDependencyGraph::CDependencyGraph ( size_t nMaxNodes )
    : m_nMaxNodes(nMaxNodes), 
      m_nNumNodes(0),
      m_vNodes(nMaxNodes)
{
}

bool CDependencyGraph::AddNode ( const NODE_ID_T& idNode )
{
    bool bReturn = false;

    size_t nNodeIndex = GetNodeIndex(idNode);

    // check to make sure index is not out of bounds of our vector
    if ( IsValidNodeIndex(nNodeIndex) )
    {
        // check to make sure we have not already added this node
        if ( m_vNodes[nNodeIndex].IsValid() == false )
        {
            // node has not been previously added, lets update
            // the node ID with a valid ID to mark it has been 
            // added now.
            m_vNodes[nNodeIndex].SetNodeID(idNode);
            m_nNumNodes++;
            bReturn = true;
        }
    }

    return bReturn;
}

bool CDependencyGraph::AddEdge ( const NODE_ID_T& idFromNode, const NODE_ID_T& idToNode, int iWeight )
{
    bool bReturn = false;

    // lets validate the input data
    if ( IsValidNodeID(idFromNode) && IsValidNodeID(idToNode) )
    {
        size_t nIndex = GetNodeIndex(idFromNode);

        if ( IsValidNodeIndex(nIndex) )
            bReturn = m_vNodes[nIndex].AddEdge (idToNode, iWeight);
    }


    return bReturn;
}


size_t CDependencyGraph::GetNumNodes ( void ) const
{
    return m_nNumNodes;
}


size_t CDependencyGraph::GetNumEdges ( void ) const
{
    size_t nNumEdges = 0;

    // following static cast added to address compiler warning
    for (int i = 0; i < static_cast<int>(m_nMaxNodes); i++)
    {
        nNumEdges += m_vNodes[i].GetNumEdges();
    }

    return nNumEdges;
}

bool    CDependencyGraph::IsValidNodeID ( const NODE_ID_T& idNode ) const
{
    bool bReturn = false;
// we are only allowing up to 25 instructions, each represented by a char in
// in the range of [a..y].  lets allow some flexibility by being case-insensitive.
    if ( (idNode >= 'a' && idNode <= 'y') || (idNode >= 'A' && idNode <= 'Y') )
        bReturn = true;

    return bReturn;
}

bool    CDependencyGraph::IsValidNodeIndex ( size_t nIndex ) const
{
    bool bReturn = false;

    if ( nIndex < m_nMaxNodes )
        bReturn = true;

    return bReturn;
}

/**
    Following method performs a pseudo-hashing of the node ID to
    determine the proper location of the Node in the vector
*/
size_t  CDependencyGraph::GetNodeIndex ( const NODE_ID_T& idNode ) const
{
    size_t nReturn = INVALID_NODE_INDEX;

    if ( IsValidNodeID(idNode) )
    {
        const std::locale& loc = std::locale();  // construct from the default locale
        TCHAR cId = static_cast<TCHAR>(idNode);
        nReturn   = std::tolower(cId, loc) - _T('a');
    }

    return nReturn;
}

bool CDependencyGraph::HasNode(const NODE_ID_T& idNode) const
{
    bool bReturn = false;

    // first determine if ID will even convert to an actual Node index;

    size_t nIndex = GetNodeIndex(idNode);

    if ( IsValidNodeIndex(nIndex) )
    {
        // now determine if that node associated with the index has
        // been added to the graph or not
        bReturn = m_vNodes[nIndex].IsValid();
    }

    return bReturn;
};

CDependencyGraph::~CDependencyGraph ( )
{
}

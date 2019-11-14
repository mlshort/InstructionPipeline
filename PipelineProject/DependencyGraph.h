/**
* @file       DependencyGraph.h
* @brief      CDependencyGraph class interface
*
*
* @author     Mark L. Short
* @date       November 23, 2014
*
*  Generally, a graph consists of:
*  - a set of nodes (or vertices)
*  - a set of edges (or arc)
*
*  Directed Acyclic Graphs (DAG) - http://en.wikipedia.org/wiki/Directed_acyclic_graph
*
*  Like most scheduling problems, instruction scheduling is usually modeled as a 
*  directed acyclic graph (DAG) evaluation problem. Each node in the data dependency 
*  graph represents a single machine instruction, and each arc represents a dependency 
*  with a weight corresponding to the latency of the relevant instruction.
*  @sa http://www.lighterra.com/papers/basicinstructionscheduling/
*
*  In order to construct a DAG to represent the dependencies between instructions:
*  - For each instruction, create a corresponding vertex in the graph
*  - For each dependency between two instructions, create a corresponding edge in the 
*    graph
*  - This edge is directed : it goes from the earlier instruction to the later one
*/
#pragma once

#if !defined(_DEPENDENCY_GRAPH_H__)
#define _DEPENDENCY_GRAPH_H__

#ifndef _COMMON_DEF_H__
    #include "CommonDef.h"
#endif

#ifndef _SET_
    #include <set>
#endif

#ifndef _VECTOR_
    #include <vector>
#endif


typedef TCHAR        NODE_ID_T;
/// Used to identify an active node
constexpr NODE_ID_T  INVALID_NODE_ID = 0;

/**
  @brief Maintains directed edge data properties.

  Additionally, CDirectedEdgeData overrides the default behavior of the 
  comparison operators such that it is ordered and identified only by 
  its m_idDestNode data member, and thus able to be stored in an STL 
  collection using its node ID as the key.
*/
class CDirectedEdgeData
{
    NODE_ID_T   m_idDestNode; ///< maintains the destination node ID
    int         m_iWeight;    ///< weight value assigned to this edge

public:
    /// Default Constructor
    constexpr CDirectedEdgeData() noexcept
        : m_idDestNode (INVALID_NODE_ID), 
          m_iWeight    (0)
    { };

    /// Initialization Constructor
    constexpr CDirectedEdgeData(const NODE_ID_T& idToNode, int iWeight = 0) noexcept
        : m_idDestNode(idToNode), 
          m_iWeight   (iWeight)
    { };

/**
    @brief Sets the value or ID of the edge destination node

    @param [in] idSet      the destination node ID to be set
*/
    void SetNodeID ( const NODE_ID_T& idSet ) noexcept
    { m_idDestNode = idSet; };

/**
    @brief Retrieves the value or ID of the edge destination node

    @retval NODE_ID_T           the edge's destination node ID
    @retval INVALID_NODE_ID     on error
*/
    constexpr NODE_ID_T GetDestNodeID ( void ) const noexcept
    { return m_idDestNode; };

/**
    @brief Sets the weight value associated with this edge

    @param [in] iSet       new weight value to be set
*/
    void SetWeight(int iSet) noexcept
    { m_iWeight = iSet; };

/**
    @brief Retrieves the current weight value associated with this edge.

    @retval int     the current edge weight
*/
    constexpr int GetWeight(void) const noexcept
    { return m_iWeight; };

/**
    @brief Comparison operation required by STL

    Performs comparison evaluation of this class
    using the m_idDestNode value only.

    @retval bool    less than '<' evaluation
*/
    constexpr bool operator<(const CDirectedEdgeData& rhs) const noexcept
    { return m_idDestNode < rhs.m_idDestNode; };

/**
    @brief Comparison operation required by STL

    Performs comparison evaluation of this class
    using the m_idDestNode value only.

    @retval bool    equal '==' evaluation
*/
    constexpr bool operator==(const CDirectedEdgeData& rhs) const noexcept
    { return m_idDestNode == rhs.m_idDestNode; };
/**
    @brief Comparison operation required by STL

    Performs comparison evaluation of this class
    using the m_idDestNode value only.

    @retval bool    greater than '>' evaluation
*/
    constexpr bool operator>(const CDirectedEdgeData& rhs) const noexcept
    { return m_idDestNode > rhs.m_idDestNode; };
};

/**
    @brief a directed graph node implementation.
    
    The CGraphNode class maintains a node ID, as well 
    as a set of CDirectedEdgeData elements representing the
    set of 'out' edges from this graph node, as a form of
    an adjacency list.
 */
class CGraphNode
{
    typedef std::set<CDirectedEdgeData>   EDGE_SET_T;
    typedef EDGE_SET_T::_Pairib           _Pairib;
    
    NODE_ID_T            m_ID;        ///< this is the node value or ID
    EDGE_SET_T           m_setEdges;  ///< this is a set of directed 'out' edges 

public:
    typedef EDGE_SET_T::const_iterator    const_iterator;
    
    /// Default Constructor
    CGraphNode() noexcept
        : m_ID(INVALID_NODE_ID), 
          m_setEdges()
    { };

    /// Initialization Constructor
    CGraphNode(const NODE_ID_T& idNode) noexcept
        : m_ID(idNode), 
          m_setEdges()
    { };

    /// Default Destructor
    ~CGraphNode() = default;

/**
    @brief Sets this object's node ID

    @param [in] idSet      new node ID to be set
*/
    void SetNodeID(const NODE_ID_T& idSet) noexcept
    { m_ID = idSet; };

/**
    @brief Retrieves this object's node ID

    @retval NODE_ID_T       the current node ID
    @retval INVALID_NODE_ID if node is vacant or has not been assigned a value
*/
    constexpr NODE_ID_T GetNodeID(void) const noexcept
    { return m_ID; };

/**
    @brief Used to check to see if node is active.

    This method checks to see if this node is active
    and assigned to a graph.

    @retval true    if member node ID is valid, denoting it 
                    has been added to the graph
    @retval false   if member node ID deemed invalid
*/
    constexpr bool IsValid(void) const noexcept
    { return m_ID != INVALID_NODE_ID; };

/**
    @brief adds a new edge to the graph
        
    This method adds a new edge, originating from this 
    node, to the associated edge set. The idToNode
    is presumed to be a valid destination node in
    the underlying graph

    @param [in] idToNode    ID of the destination node
    @param [in] iWeight     Edge's weight value

    @retval true            if edge successfully added
    @retval false           on error
*/
    bool AddEdge(const NODE_ID_T& idToNode, int iWeight);

/**
    @brief Adds a new edge the graph
        
    This method adds a new edge, originating from this 
    node, to the associated edge set.

    @param [in] edge        data object containing associated 
                            node destination and edge weight 
                            information

    @retval true            if edge successfully added
    @retval false           on error
*/
    bool AddEdge(const CDirectedEdgeData& edge);

/**
    @brief Retrieves number of edges

    This method retrieves the current number
    of 'out' edges originating from this node.

    @retval size_t  current number of nodes in the
                    edge set 
    @retval 0       if this node is not a valid
                    node
*/
    constexpr size_t GetNumEdges(void) const noexcept
    { return IsValid() ? m_setEdges.size() : 0; };

/**
    @brief Affords iterator functionality

    Method provides limited read-only access to
    iterate over the current edge set.

    @retval const_iterator iterator for beginning of 
                           non-mutable edge sequence
*/
    const_iterator beginEdge(void) const noexcept
    { return m_setEdges.begin(); };

/**
    @brief Affords iterator functionality

    Method provides limited read-only access to
    iterate over the current edge set.

    @retval const_iterator iterator for end of non-mutable 
                           edge sequence
*/
    const_iterator endEdge(void) const noexcept
    { return m_setEdges.end(); };

/**
    @brief Test if given edge exists.

    @param [in] idToNode    target node

    @retval true        if there exists an edge from this node 
                        to target node
    @retval false       if target node or edge is not found
*/
    bool HasEdge ( const NODE_ID_T& idToNode ) const
    { 
        const_iterator itr = m_setEdges.find( CDirectedEdgeData(idToNode) ); 
        return (itr != m_setEdges.end() );
    };
};

/// used to provide a consistent index out-of-range result 
constexpr size_t INVALID_NODE_INDEX = static_cast<size_t>(-1);

/**
    @brief A directed acyclic graph implementation

    The CDependencyGraph class uses a form of an "adjacency list" in order
    to model a DAG, with the following caveats:
    - rather than being implemented as an array of "linked-lists", it is 
      implemented as a vector of sets.  A vector provides random access to the 
      node data and a set is implemented as a balanced red-black tree and 
      provides access to the edge end-point in O(log n) time complexity.
*/
class CDependencyGraph
{
    size_t                  m_nMaxNodes; ///< upper limit on nodes allowed
    size_t                  m_nNumNodes; ///< current number of nodes
    std::vector<CGraphNode> m_vNodes;    ///< container of nodes contained in graph

public:

    typedef std::vector<CGraphNode>::const_iterator    const_iterator;

    /// Default Constructor
    CDependencyGraph() noexcept;

    /**
        @brief Initialization Constructor

        Optimized constructor to allow the pre-allocation
        of the underlying graph node vector.

        @param [in] nMaxNodes   The potential number of nodes to be
                                stored in the graph. This value is
                                used to preallocate enough space
                                in the vector.
    */
    CDependencyGraph(size_t nMaxNodes) noexcept;

    /// Default Destructor
    ~CDependencyGraph() = default;
    /**
        @brief Adds a new node to the graph.

        @param [in] idNode      ID of the new node to be added

        @retval true            if successfully added
        @retval false           if already exists or error
    */
    bool AddNode(const NODE_ID_T& idNode) noexcept;

    /**
        @brief Adds a directed edge between 2 existing nodes.

        @param [in] idFromNode  value of the source node ID
        @param [in] idToNode    value of the destination node ID
        @param [in] iWeight     value of Edge weight

        @retval true            if successfully added
        @retval false           if already exists or error
    */
    bool AddEdge(const NODE_ID_T& idFromNode, const NODE_ID_T& idToNode, int iWeight) noexcept;

    /**
        @brief Retrieves the current number of nodes in the graph

        @retval size_t      the number of nodes (or vertices) in the graph
    */
    constexpr size_t GetNumNodes(void) const noexcept
    {
        return m_nNumNodes;
    };

    /**
        @brief Retrieves the current number of edges in the graph

        @retval size_t      the number of edges (or arcs) in the graph
    */
    size_t GetNumEdges(void) const noexcept;
    /**
        @brief  Affords the ability to query for the
                existence of a particular graph node

        @param [in] idNode  target node ID
        @retval true        if idNode is found in the graph
    */
    bool   HasNode(const NODE_ID_T& idNode) const noexcept;

    /**
        @brief Affords iteration functionality

        Method provides limited read-only access to
        iterate over the current Node set.

        @retval const_iterator iterator for beginning of nonmutable
                               sequence
    */
    const_iterator begin(void) const noexcept
    {
        return m_vNodes.begin();
    };

    /**
        @brief Affords iteration functionality

        Method provides limited read-only access to
        iterate over the current Node set.

        @retval const_iterator iterator for end of nonmutable
                               sequence
    */
    const_iterator end(void) const noexcept
    {
        return m_vNodes.end();
    };



private:
    /**
        @brief Performs basic validation of node ID

        @param [in] idNode      value to be verified

        @retval true            if idNode is a valid ID
        @retval false           on invalid node ID
    */
    bool    IsValidNodeID(const NODE_ID_T& idNode) const noexcept;

    /**
        @brief Performs basic validation of a node index.

        nIndex is probed to see if it falls within the underlying vector
        boundaries.

        @param [in] nIndex      Index to be verified

        @retval true            if Index falls within the current
                                vector range
        @retval false           if Index is found out-of-bounds
    */
    constexpr bool    IsValidNodeIndex(size_t nIndex) const noexcept
    {
        return (nIndex < m_nMaxNodes);
    };

/**
    @brief returns corresponding node index

    Performs a basic hash-translation of the index of
    the node from its associated ID. The returned index
    corresponds to the nodes offset within the vector.

    @param [in] idNode          subject node ID

    @retval size_t              Index for idNode
    @retval INVALID_NODE_INDEX  if no valid Index exists
*/
    size_t  GetNodeIndex    (const NODE_ID_T& idNode) const noexcept;

    /// copy constructor
    CDependencyGraph(const CDependencyGraph& o) = delete;

    /// assignment operator
    CDependencyGraph& operator=(const CDependencyGraph& rhs) = delete;
};

#endif
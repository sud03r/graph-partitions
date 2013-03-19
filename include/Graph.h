#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <map>
using namespace std;

class Edge;

class NodeSearchData {
    bool mIsVisited;
    Edge* mPrevEdge;
    
public:
    NodeSearchData()
    {
        mIsVisited = false;
        mPrevEdge = NULL;
    }
    bool IsVisited() { return mIsVisited; }
    void MarkVisited(Edge* edge) 
    { 
        mPrevEdge = edge;
        mIsVisited = true;
    } 
    Edge* GetPrevEdge() { return mPrevEdge; }
};

class Node {
    int mNodeId;
    vector<Edge*> mOutgoingEdges;
    vector<Edge*> mIncomingEdges;
    NodeSearchData* mSearchData;
    
public:
    Node(int id) 
    { 
        mNodeId = id;
        mSearchData = NULL;
    }
    Node(Node& n);
    ~Node();
    void Pr();

    int GetNodeId() { return mNodeId; }
    int GetAbsId() { return mNodeId & 0xffff; }
    int PartitionId() { return mNodeId >> 16; }
    
    void AddOutgoingEdge(Edge* edge) { mOutgoingEdges.push_back(edge); }
    void AddIncomingEdge(Edge* edge) { mIncomingEdges.push_back(edge); }
    const vector<Edge*>& GetOutgoingEdges() { return mOutgoingEdges; }
    const vector<Edge*>& GetIncomingEdges() { return mIncomingEdges; }
    NodeSearchData* GetSearchData() { return mSearchData; }
    void SetSearchData(NodeSearchData* data) { mSearchData = data; }
};

class Edge {
    int mSrcNodeId;
    int mDstNodeId;

public:
    Edge(int srcId, int dstId) 
    {
        mSrcNodeId = srcId;
        mDstNodeId = dstId;
    }
    virtual ~Edge() {}
    int GetSrcNodeId() { return mSrcNodeId; }
    int GetDstNodeId() { return mDstNodeId; }
    bool IsInterPartitionEdge()
    {
        int srcPartId = mSrcNodeId >> 16;
        int dstPartId = mDstNodeId >> 16;
        return (srcPartId != dstPartId);
    }
    bool IsIntraPartitionEdge()
    {
        return !IsInterPartitionEdge();
    }

    virtual bool IsHopEdge() { return false; }
    virtual void Pr();
};

class Graph {

protected:
    map<int, Node*> mNodes;

public:
    Graph() {}
    virtual ~Graph();
    
    virtual void Pr();
    void ReadGraph(const char* filename);
    
    const map<int, Node*>& GetNodes() { return mNodes; }
    void AddNode(Node* n) 
    { 
        mNodes.insert(pair<int, Node*>(n->GetAbsId(), n));
    }
    void CreateSrcDstNodesIfReqd(int srcNodeId, int dstNodeId)
    {
         int absSrcIdx = srcNodeId & 0xffff;
         int absDstIdx = dstNodeId & 0xffff;

         if (mNodes.find(absSrcIdx) == mNodes.end())
            AddNode(new Node(srcNodeId));

         if (mNodes.find(absDstIdx) == mNodes.end())
            AddNode(new Node(dstNodeId));
    }
    
    Node* GetNode(int nodeId) 
    { 
        // drop the partition id.
        int absNodeIdx = nodeId & 0xffff;
        if (mNodes.find(absNodeIdx) != mNodes.end())
            return mNodes.at(absNodeIdx);
        return NULL; 
    }

    virtual Node* GetRemoteNode(int nodeId)
    {
        return GetNode(nodeId);
    }
    
    Node* GetNodeAbsIdx(int nodeIdx) 
    { 
        if (mNodes.find(nodeIdx) != mNodes.end())
            return mNodes.at(nodeIdx);
        return NULL;
    }
    
    void RegisterEdge(Edge* edge) 
    {
        GetNode(edge->GetSrcNodeId())->AddOutgoingEdge(edge);
        GetNode(edge->GetDstNodeId())->AddIncomingEdge(edge);
    }
    void FindPath(int srcId, int dstId, vector<Edge*>& path)
    {
        Node* src = GetNodeAbsIdx(srcId);
        Node* dst = GetNodeAbsIdx(dstId);
        if (src && dst)
            Traverse(src, dst, path);
    }
    void Traverse(Node* srcNode, Node* dstNode, vector<Edge*>& path);
    virtual bool IsValidEdge(Edge* e, Node* srcNode, Node* dstNode) 
    { 
        return true; // all edges can be taken.
    }
    virtual void InitializeSearchData();
    virtual void ClearSearchData();
};

#endif

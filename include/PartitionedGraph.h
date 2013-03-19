#ifndef _PARTITIONED_GRAPH_H
#define _PARTITIONED_GRAPH_H

#include "Graph.h"

class HopEdge : public Edge 
{
    vector<Edge*> mMemberEdges;
    bool memberEdgesCreated;

public:

    HopEdge(int srcNodeId, int dstNodeId) : 
        Edge(srcNodeId, dstNodeId)
    {
        memberEdgesCreated = false;
    }
    HopEdge(HopEdge& hop);
    ~HopEdge();
    void AddMemberEdge(Edge* e) { mMemberEdges.push_back(e); }
    virtual bool IsHopEdge() { return true; }
    virtual void Pr();
};

class PartitionedGraph;
class Partition : public Graph 
{
    int mPartitionId;
    vector<Node*> mExternalNodes; // Partition boundary.

public:

    Partition(int id, PartitionedGraph* p) 
    { 
        mPartitionId = id;
    }
    void Initialize() {
        IdentifyExternalNodes();
        CreateHopEdges();
    }
    void IdentifyExternalNodes();
    void CreateHopEdges();
    const vector<Node*>& GetExternalNodes() { return mExternalNodes; }
    virtual void Pr();
    virtual bool IsValidEdge(Edge* e, Node* srcNode, Node* dstNode)
    {
        if (e->IsHopEdge() || e->IsInterPartitionEdge())
            return false;
        return true;
    }
};

// This graph segregates External nodes with Inter-partition
// and hop edges.
class ExternalGraph : public Graph 
{
    Node* CopyEdgesToGraph(const vector<Edge*>& edges);
public:
    ExternalGraph(const vector<Partition*>& partitions);
};

class CombinedGraph : public Graph
{
public:
    CombinedGraph(const vector<Partition*>& partitions);
    virtual bool IsValidEdge(Edge* e, Node* srcNode, Node* dstNode);
};

class PartitionedGraph 
{
    vector<Partition*> mPartitions;
    ExternalGraph* mExternalGraph;
    CombinedGraph* mCombinedGraph;

public:

    PartitionedGraph(Graph* parent);
    ~PartitionedGraph();
    Partition* GetPartition(int partitionId)
    {
        if (mPartitions.size() <= partitionId)
            mPartitions.resize(partitionId + 1);
        if (!mPartitions.at(partitionId))
            mPartitions[partitionId] = new Partition(partitionId, this);
        return mPartitions.at(partitionId);
    }
    
    void InitializePartitions()
    {
        for (Partition* p : mPartitions)
            p->Initialize();
    }
    
    void Pr() 
    {
        for (Partition* p : mPartitions)
            p->Pr();
    }
   
    Partition* FindPartition(int nodeId)
    {
        for (Partition* p : mPartitions)
            if (p->GetNodeAbsIdx(nodeId))
                return p;
        return NULL;
    }
    void FindPath(int srcId, int dstId, vector<Edge*>& path);
};

#endif

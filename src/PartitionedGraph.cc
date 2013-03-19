#include <iostream>
#include <cassert>
#include "PartitionedGraph.h"
using namespace std;

HopEdge::HopEdge(HopEdge& hop) : 
    Edge(hop.GetSrcNodeId(), hop.GetDstNodeId())
{
    memberEdgesCreated = true;
    for (Edge* edge : hop.mMemberEdges)
        mMemberEdges.push_back(new Edge(*edge));
}

HopEdge::~HopEdge()
{
    if (!memberEdgesCreated)
        return;
    for (Edge* edge : mMemberEdges)
        delete edge;
}

void HopEdge::Pr()
{
    Edge::Pr();
    for (Edge* edge : mMemberEdges)
    {
        cout << "  (H)";
        edge->Pr();
    }
}

void Partition::IdentifyExternalNodes()
{
    const map<int, Node*>& nodes = GetNodes();
    for (auto& it : nodes)
    {
        Node* node = it.second;
        bool nodeAdded = false;
        // If a node has an outgoing/incoming interpartition
        // edge, it is an External Node. (i.e will lie on the
        // partition boundary)

        for (Edge* edge : node->GetOutgoingEdges())
        {
            if (edge->IsInterPartitionEdge())
            {
                mExternalNodes.push_back(node);
                nodeAdded = true;
                break;
            }
        }
        // mExternalNodes shouldnot have a duplicate
        // entry.
        if (nodeAdded)
            continue;
        
        for (Edge* edge : node->GetIncomingEdges())
        {
            if (edge->IsInterPartitionEdge())
            {
                mExternalNodes.push_back(node);
                break;
            }
        }

    }
}

void Partition::CreateHopEdges()
{
    for (Node* srcNode : mExternalNodes)
    {
        for (Node* dstNode : mExternalNodes)
        {
            if (srcNode == dstNode)
                continue;
            vector<Edge*> path;
            // Slightly unoptimal, but okay as we can use the same function.
            Traverse(srcNode, dstNode, path);
            if (path.size() > 1)
            {
                // Create a Hop Edge from srcNode to dstNode
                // srcNode ... hopEdge[essentially a cache for edges in path].. dstNode
                HopEdge* hopEdge = new HopEdge(srcNode->GetNodeId(), dstNode->GetNodeId());
                vector<Edge*>::reverse_iterator it;
                for (it = path.rbegin(); it != path.rend(); it++)
                    hopEdge->AddMemberEdge(*it);
                RegisterEdge(hopEdge);
            }
        }
    }
}

void Partition::Pr()
{
    cout << endl;
    cout << "Partition: " << mPartitionId << endl;
    cout << "External Nodes: ";

    for (Node* node : mExternalNodes)
        cout << node->GetAbsId() << " ";
    cout << endl;
    Graph::Pr();
    cout << endl;
}

Node* ExternalGraph::CopyEdgesToGraph(const vector<Edge*>& edges)
{
    for (Edge* edge : edges)
    {
        if (edge->IsHopEdge())
        {
            HopEdge* hopEdge = static_cast<HopEdge*>(edge);
            assert(hopEdge);
            RegisterEdge(new HopEdge(*hopEdge));
        }
        else if (edge->IsInterPartitionEdge())
            RegisterEdge(new Edge(*edge));
        else
        {
            // If both src and destination are external nodes,
            // copy that edge as well.
            Node* src = GetNode(edge->GetSrcNodeId());
            Node* dst = GetNode(edge->GetDstNodeId());
            if (src && dst)
                RegisterEdge(new Edge(*edge));
        }
    }
}

ExternalGraph::ExternalGraph(const vector<Partition*>& partitions)
{
    // First Create Nodes.
    for (Partition* partition : partitions)
    {
        const vector<Node*>& externalNodes = partition->GetExternalNodes();
        for (Node* node : externalNodes)
           AddNode(new Node(node->GetNodeId()));
    }
    
    // Now Copy desired edges to External Graph.
    for (Partition* partition : partitions)
    {
        const vector<Node*>& externalNodes = partition->GetExternalNodes();
        for (Node* node : externalNodes)
            CopyEdgesToGraph(node->GetOutgoingEdges());
    }
}

CombinedGraph::CombinedGraph(const vector<Partition*>& partitions)
{
    for (Partition* partition : partitions)
    {
        const map<int, Node*>& nodes = partition->GetNodes();
        mNodes.insert(nodes.begin(), nodes.end());
    }
}

bool CombinedGraph::IsValidEdge(Edge* e, Node* srcNode, Node* dstNode) 
{
    if (e->IsHopEdge())
    {
        Node* edgeSrcNode = GetNode(e->GetSrcNodeId());
        // We will only take a hop edge if edgeSrc and edgeDst nodes are
        // in a partition different from src or dst nodes.
        return (srcNode->PartitionId() != edgeSrcNode->PartitionId()) &&
               (dstNode->PartitionId() != edgeSrcNode->PartitionId());
    }
    return true;
}


PartitionedGraph::PartitionedGraph(Graph* parent)
{
    const map<int, Node*>& nodes = parent->GetNodes();
    for (auto& it : nodes)
    {
        Node* node = it.second;
        Partition* partition = GetPartition(node->PartitionId());
        partition->AddNode(new Node(*node));
    }
    InitializePartitions();
    mExternalGraph = new ExternalGraph(mPartitions);
    mCombinedGraph = new CombinedGraph(mPartitions);
}

void PartitionedGraph::FindPath(int srcNodeId, int dstNodeId, vector<Edge*>& path)
{
    // Method 1: using the hop edges, and traversing the entire graph,
    // hopping through partitions.
    mCombinedGraph->FindPath(srcNodeId, dstNodeId, path);

    // Method 2: Use the External graph to find paths b/w nodes
    // at the partition boundary.
    #if 0
    if (srcPartition == dstPartition)
    {
        // Intrapartition
        srcPartition->FindPath(srcNodeId, dstNodeId, path);
    }
    else
    {
        // Interpartition.
        // 1. Traverse from srcNode to srcPartition Boundary.
        // 2. Traverse from src partition boundary to dst partition boundary.
        // 3. Traverse from dstPartition boundary to dstNode.
        // Downside: 
        // Simple BFS will not suffice as ordering of nodes will differ.
        // TODO.
    }
    #endif
}

PartitionedGraph::~PartitionedGraph()
{
    for (Partition* partition : mPartitions)
        delete partition;
    delete mExternalGraph;
}

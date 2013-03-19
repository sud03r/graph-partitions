#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <queue>
#include "Graph.h"
using namespace std;

void Graph::ReadGraph(const char* inpFile)
{
    ifstream iFile(inpFile);
    while (iFile.peek() != EOF)
    {
        string line;
        getline(iFile, line);
       
        // only spaces/blank line
        if(line.find_first_not_of(' ') == string::npos)
            continue;

        if (line.find('#') != string::npos) // Must be a comment.
            continue;
        
        int srcPart, dstPart;
        int srcId, dstId;
        sscanf(line.c_str(), "%d %d %d %d", &srcPart, &srcId, 
                                            &dstPart, &dstId);
        
        int srcNodeId = srcPart << 16 | srcId;
        int dstNodeId = dstPart << 16 | dstId;
        CreateSrcDstNodesIfReqd(srcNodeId, dstNodeId);
        Edge* edge = new Edge(srcNodeId, dstNodeId);
        RegisterEdge(edge);
    }
}

void Graph::Pr()
{
    cout << "Graph Nodes:" << endl;
    for (auto& it : mNodes)
    {
        Node* curNode = it.second;
        cout << "Node : " << it.first << endl;
        curNode->Pr();
    }
}

Graph::~Graph()
{
    for (auto& it : mNodes)
        delete it.second;
}

Node::Node(Node& n)
{
    mNodeId = n.mNodeId;
    mSearchData = NULL;
    for (Edge* edge : n.mOutgoingEdges)
        mOutgoingEdges.push_back(new Edge(*edge));
    for (Edge* edge : n.mIncomingEdges)
        mIncomingEdges.push_back(new Edge(*edge));
}

void Node::Pr()
{
    for (Edge* edge : mOutgoingEdges)
        edge->Pr();
}

Node::~Node()
{
    for (Edge* edge : mOutgoingEdges)
        delete edge;
}

void Edge::Pr()
{
    const char* edgeType = IsInterPartitionEdge() ? 
                           " InterPartition" : " IntraPartition";
    const char* edgeAttribute = IsHopEdge() ? "Hop" : "";
    cout << (GetSrcNodeId() & 0xffff) << " --> " << (GetDstNodeId() & 0xffff) 
         << edgeType << edgeAttribute << endl;
}

void Graph::InitializeSearchData()
{
    for (auto& it : mNodes)
    {
        Node* node = it.second;
        node->SetSearchData(new NodeSearchData());
    }
}

void Graph::ClearSearchData()
{
   // Clear Search data.
    for (auto& it : mNodes)
    {
        Node* node = it.second;
        delete node->GetSearchData();
        node->SetSearchData(NULL);
    }
}

void Graph::Traverse(Node* srcNode, Node* dstNode, vector<Edge*>& path)
{
    InitializeSearchData();
    queue<Node*> q;
    q.push(srcNode);
    srcNode->GetSearchData()->MarkVisited(NULL);
    while (!q.empty())
    {
        Node* curNode = q.front();
        q.pop();
        const vector<Edge*>& edges = curNode->GetOutgoingEdges();
        for (Edge* curEdge : edges )
        {
            if (!IsValidEdge(curEdge, srcNode, dstNode))
                continue;
            Node* nextNode = GetNode(curEdge->GetDstNodeId());
            NodeSearchData* searchData = nextNode->GetSearchData(); 
            if (!searchData->IsVisited())
            {
                searchData->MarkVisited(curEdge);
                q.push(nextNode);
            }
            
            // We're done here.
            if (dstNode == nextNode)
            {
                // Clear the queue.
                queue<Node*> empty;
                swap(q, empty);
                break;
            }
        }
    }

    // Populate path from srcNode -> dstNode. (reverse order.)
    Edge* prevEdge = dstNode->GetSearchData()->GetPrevEdge();;
    while (prevEdge != NULL) 
    {
        path.push_back(prevEdge);
        Node* curNode = GetNode(prevEdge->GetSrcNodeId());
        prevEdge = curNode->GetSearchData()->GetPrevEdge();
    }
    ClearSearchData();
}

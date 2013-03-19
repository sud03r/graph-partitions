#include <iostream>
#include "Graph.h"
#include "PartitionedGraph.h"
using namespace std;

static void sPrintPath(vector<Edge*>& path)
{
    if (path.size() != 0)
        for (auto rit = path.rbegin(); rit != path.rend(); rit++)
            (*rit)->Pr();
    else
        cout << "No path exists" << endl;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " input-file" << endl;
        return 1;
    }

    Graph g;
    g.ReadGraph(argv[1]);
    PartitionedGraph pGraph(&g);
    
    vector<Edge*> path;
    g.FindPath(1, 11, path);
    cout << "Un-partitioned Graph:" << endl;
    sPrintPath(path);
    
    path.clear();
    pGraph.FindPath(1, 11, path);
    cout << "Partitioned Graph:" << endl;
    sPrintPath(path);
    return 0;
}

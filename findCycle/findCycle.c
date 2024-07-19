#include "../graphutils.h"

// A program to find a cycle in a directed graph
// You may use DFS or BFS as needed
bool dfsCheck(graphNode_t node, int visited[], int recStack[], AdjacencyListNode adjacencyList[], int graphNodeCount, int cycleNodes[]){
    //marks the visited and recStack index as 1
    visited[node] = 1;
    recStack[node] = 1;

    //find the neighbors the current node
    AdjacencyListNode* neighbor = adjacencyList[node].next;
    //look at all the neighbors
    while(neighbor){
        graphNode_t dest = neighbor->graphNode;
        //if that neighbor hasn't been visited yet
        if(visited[dest] == -1) {
            //use dfs recursion to check
            if(dfsCheck(dest, visited, recStack, adjacencyList, graphNodeCount, cycleNodes)) {
                //if its true, mark the neighbor in the cycleNodes array as part of the path
                cycleNodes[dest] = 1;
                return true;
            }
        //back edge is found
        }else if(recStack[dest] == 1){
            cycleNodes[dest] = 1;
            cycleNodes[node] = 1;
            return true;
        }
        neighbor = neighbor->next;
    }

    //if you go through all the neighbors, back track
    recStack[node] = 0;
    return false;
}


int main ( int argc, char* argv[] ) {
    // READ INPUT FILE TO CREATE GRAPH ADJACENCY LIST
    AdjacencyListNode* adjacencyList;
    size_t graphNodeCount = adjMatrixToList(argv[1], &adjacencyList);

    //initialize visited, recStack, and cycleNodes arrays
    // visited-> marks -1 if it hasn't been visited yet, marks 1 if it has
    // recStack-> marks 0 if it isn't in the current path, marks 1 if it is
    // cycleNodes-> marks 0 if it is a part of the cycle, marks 1 if it is
    int visited[graphNodeCount];
    int recStack[graphNodeCount];
    int cycleNodes[graphNodeCount];

    for (graphNode_t i=0; i<graphNodeCount; i++) {
        visited[i] = -1;
        recStack[i] = 0;
        cycleNodes[i] = 0;
    }

    bool isCyclic = false;
    //loops through the adjacency list
    for (unsigned source=0; source<graphNodeCount; source++) {
        //if it hasnt been visited yet, call the dfsCheck function
        if(visited[source] == -1){
            //if it returns true, mark flag as true and break
            if(dfsCheck(source, visited, recStack, adjacencyList, graphNodeCount, cycleNodes)){
                isCyclic = true;
                break;
            }
        }
    }
   
    if (!isCyclic){
        printf("DAG\n");
    }
    else{
        //print the nodes in the cycle
        for(int i = 0; i < graphNodeCount; i++){
            if(cycleNodes[i] == 1){
                printf("%d ", i);
            }
        }
        printf("\n");
    }

    freeAdjList ( graphNodeCount, adjacencyList );
    return EXIT_SUCCESS;
}



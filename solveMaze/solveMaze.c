#include "../graphutils.h" // header for functions to load and free adjacencyList
#include "../queue/queue.h" // header for queue

// A program to solve a maze that may contain cycles using BFS

int main ( int argc, char* argv[] ) {

    // First, read the query file to get the source and target nodes in the maze
    FILE* fp = fopen(argv[2], "r");
    if (!fp) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    char buf[256];
    if (!fscanf(fp, "%s\n", buf)) {
        perror("reading the source node failed");
        exit(EXIT_FAILURE);
    }
    int sourceNode = atoi(buf);
    if (!fscanf(fp, "%s\n", buf)) {
        perror("reading the target node failed");
        exit(EXIT_FAILURE);
    }
    int target = atoi(buf);

    // READ INPUT FILE TO CREATE GRAPH ADJACENCY LIST
    AdjacencyListNode* adjacencyList = NULL;
    size_t graphNodeCount = adjMatrixToList(argv[1], &adjacencyList);

    // USE A QUEUE TO PERFORM BFS
    Queue queue = { .front=NULL, .back=NULL };

    // An array that keeps track of who is the parent node of each graph node we visit
    graphNode_t* parents = calloc( graphNodeCount, sizeof(graphNode_t) );
    for (size_t i=0; i<graphNodeCount; i++) {
        parents[i] = -1; // -1 indicates that a nodes is not yet visited
    }

    //sets up curren, target, and sets the root of the parents array
    int current = sourceNode;
    parents[current] = current;

    while ( current != target ) {
        // so long as we haven't found the target node yet, iterate through the adjacency list
        // add each neighbor that has not been visited yet (has no parents) to the queue of nodes to visit
        for (graphNode_t source=0; source<graphNodeCount; source++) {
            //finds the root/starting point
            if(parents[source] != -1){
                AdjacencyListNode* neighbor = adjacencyList[source].next;
                //checks all of its neighbors
                while (neighbor) {
                    graphNode_t dest = neighbor->graphNode;
                    //if it hasn't been visited yet
                    if(parents[dest] == -1){
                        //create a pointer and add it to the queue and set the parents[dest] = source
                        unsigned long int* pointer = malloc( sizeof(unsigned long int) );
                        *pointer = (long)(neighbor->graphNode);
                        enqueue(&queue, pointer);
                        parents[dest] = source;
                    }
                    neighbor = neighbor->next;
                }
            }
        }
        // Visit the next node at the front of the queue of nodes to visit
        AdjacencyListNode* temp = dequeue(&queue);
        current = temp->graphNode;
        free(temp);
    }

    // // Now that we've found the target graph node, use the parent array to print maze solution
    // // Print the sequence of edges that takes us from the source to the target node
    for (size_t source=0; source<graphNodeCount; source++) {
        if(parents[source] != -1){
            if(parents[source] != source){
                printf("%ld %ld\n", parents[source], source);
            }
        }
    }

    // free any queued graph nodes that we never visited because we already solved the maze
    while ( queue.front ) {
        unsigned char* data = dequeue(&queue);
        free(data);
    }

    free (parents);
    freeAdjList ( graphNodeCount, adjacencyList );

    return EXIT_SUCCESS;
}

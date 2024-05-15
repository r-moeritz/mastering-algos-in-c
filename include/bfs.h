#ifndef BFS_H
#define BFS_H

#include "graph.h"

typedef struct {
    void* data;
    VertexColour colour;
    int hops;
    List* path;
} BfsVertex;

int bfs(Graph* graph, BfsVertex* start, List* hops);

#endif

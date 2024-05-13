#include <string.h>
#include "../include/graph.h"

void graph_init(Graph* graph,
                int (*match)(const void* key1, const void* key2),
                void (*destroy)(void* data)) {
    graph->vcount = 0;
    graph->ecount = 0;
    graph->match = match;
    graph->destroy = destroy;

    list_init(&graph->adjlists, NULL);
}

void graph_destroy(Graph* graph) {
    AdjList* adjlist;

    /* Remove each adjacency list struct & destroy its adjacency list */
    while (list_size(&graph->adjlists)) {
        if (list_rem_next(&graph->adjlists, NULL, (void**)&adjlist)) {
            continue; /* failed to remove, try again */
        }

        set_destroy(&adjlist->adjacent);
        if (graph->destroy) {
            graph->destroy(adjlist->vertex);
        }
        free(adjlist);
    }

    /* Destroy empty list of adjacency list structures */
    list_destroy(&graph->adjlists);
    memset(graph, 0, sizeof(Graph));
}

int graph_ins_vertex(Graph* graph, const void* data) {
    ListElmt* element;
    AdjList* adjlist;
    int rc;

    /* Don't allow insertion of duplicate vertices */
    for (element = list_head(&graph->adjlists); element != NULL; element = list_next(element)) {
        if (graph->match(data, ((AdjList*)list_data(element))->vertex)) {
            return 1;
        }
    }

    adjlist = malloc(sizeof(AdjList));
    if (!adjlist) {
        return -1;
    }

    adjlist->vertex = (void*) data;
    set_init(&adjlist->adjacent, graph->match, NULL);
    rc = list_ins_next(&graph->adjlists, list_tail(&graph->adjlists), adjlist);
    if (rc) {
        return rc;
    }

    graph->vcount += 1;
    return 0;
}


int graph_ins_edge(Graph* graph, const void* data1, const void* data2) {
    ListElmt* element;
    int rc;

    /* Don't allow insertion of an edge without both its vertices in the graph */
    for (element = list_head(&graph->adjlists); element != NULL; element = list_next(element)) {
        if (graph->match(data1, ((AdjList*)list_data(element))->vertex)) {
            break;
        }
    }

    if (!element) {
        return -1; /* 1st vertex not found */
    }

    for (element = list_head(&graph->adjlists); element != NULL; element = list_next(element)) {
        if (graph->match(data2, ((AdjList*)list_data(element))->vertex)) {
            break;
        }
    }

    if (!element) {
        return -1; /* 2nd vertex not found */
    }

    /* Insert 2nd vertex into the adjacency list of the 1st vertex */
    rc = set_insert(&((AdjList*)list_data(element))->adjacent, data2);
    if (rc) {
        return rc;
    }

    graph->ecount += 1;
    return 0;
}

int graph_rem_vertex(Graph* graph, void** data) {
    ListElmt* element;
    ListElmt* temp;
    ListElmt* prev;
    AdjList* adjlist;
    int found;

    /* Traverse each adjacency list & the vertices it contains */
    prev = NULL;
    found = 0;

    for (element = list_head(&graph->adjlists); element != NULL; element = list_next(element)) {
        /* Don't allow removal of the vertex if it's in an adjacency list */
        if (set_is_member(&((AdjList*)list_data(element))->adjacent, *data)) {
            return -1;
        }

        /* Keep a pointer to the vertex to be removed */
        if (graph->match(*data, ((AdjList*)list_data(element))->vertex)) {
            temp = element;
            found = 1;
        }

        /* Keep a pointer to the vertex before the one to be removed */
        if (!found) {
            prev = element;
        }
    }

    if (!found) {
        return -1;
    }

    /* Don't allow removal of the vertex if its adjacency list is not empty */
    if (set_size(&((AdjList*)list_data(temp))->adjacent) > 0) {
        return -1;
    }

    /* Remove the vertex */
    if (list_rem_next(&graph->adjlists, prev, (void**)&adjlist)) {
        return -1;
    }

    /* Free the storage allocated by the abstract datatype */
    *data = adjlist->vertex;
    free(adjlist);

    graph->vcount -= 1;
    return 0;
}

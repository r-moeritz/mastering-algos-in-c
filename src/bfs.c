#include "../include/bfs.h"
#include "../include/queue.h"

int bfs(Graph* graph, BfsVertex* start, List* hops) {
    Queue queue;
    AdjList* adjlist;
    AdjList* clr_adjlist;
    BfsVertex* clr_vertex;
    BfsVertex* adj_vertex;
    ListElmt* element;
    ListElmt* member;

    /* Initialize all the vertices in the graph */
    for (element = list_head(&graph_adjlists(graph)); element != NULL; element = list_next(element)) {
        clr_vertex = ((AdjList*)list_data(element))->vertex;

        if (graph->match(clr_vertex, start)) {
            /* Initialize the start vertex */
            clr_vertex->colour = GRAY;
            clr_vertex->hops = 0;
        }
        else {
            /* Initialize a vertex other than the start vertex */
            clr_vertex->colour = WHITE;
            clr_vertex->hops = -1;
        }
    }

    /* Initialize the queue with the adjacency list of the start vertex */
    queue_init(&queue, NULL);

    if (graph_adjlist(graph, start, &clr_adjlist)) {
        queue_destroy(&queue);
        return -1;
    }

    if (queue_enqueue(&queue, clr_adjlist)) {
        queue_destroy(&queue);
        return -1;
    }

    /* Perform a breadth-first search */
    while (queue_size(&queue)) {
        adjlist = queue_peek(&queue);

        /* Traverse each vertex in the current adjacency list */
        for (member = list_head(&adjlist->adjacent); member != NULL; member = list_next(member)) {
            adj_vertex = list_data(member);

            /* Determine the colour of the next adjacent vertex */
            if (graph_adjlist(graph, adj_vertex, &clr_adjlist)) {
                queue_destroy(&queue);
                return -1;
            }

            clr_vertex = clr_adjlist->vertex;

            /* Colour each white vertex gray & enqueue its adjacency list */
            if (clr_vertex->colour != WHITE) {
                continue;
            }

            clr_vertex->colour = GRAY;
            clr_vertex->hops = ((BfsVertex*)adjlist->vertex)->hops + 1;

            if (queue_enqueue(&queue, clr_adjlist)) {
                queue_destroy(&queue);
                return -1;
            }
        }

        /* Dequeue the current adjacency list & colour its vertex black */
        if (!queue_dequeue(&queue, (void**)&adjlist)) {
            ((BfsVertex*)adjlist->vertex)->colour = BLACK;
        }
        else {
            queue_destroy(&queue);
            return -1;
        }
    }

    queue_destroy(&queue);

    /* Pass back the hop count for each vertex in a list */
    list_init(hops, NULL);

    for (element = list_head(&graph_adjlists(graph)); element != NULL; element = list_next(element)) {
        clr_vertex = ((AdjList*)list_data(element))->vertex;

        /* Skip vertices that were not visited (those with hop counts of -1),
         * as well as the starting node (hop count of 0), */
        if (clr_vertex->hops <= 0) {
            continue;
        }

        if (list_ins_next(hops, list_tail(hops), clr_vertex)) {
            list_destroy(hops);
            return -1;
        }
    }

    return 0;
}

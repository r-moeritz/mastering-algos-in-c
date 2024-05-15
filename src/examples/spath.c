#include "../../include/bfs.h"
#include <stdio.h>
#include <string.h>

BfsVertex* create_vertex(const char* name) {
    BfsVertex* v = malloc(sizeof(BfsVertex));
    if (!v) {
        return NULL;
    }

    v->data = (void*) name;
    return v;
}

void build_graph(Graph* graph) {
    /*
                ("node1")
                 /     \
                /       \
         ("node2")-----("node3")
            \              /
             \            /
          ("node4")---("node5")
                        /
                       /
                   ("node6")
     */
    BfsVertex* n1 = create_vertex("node1");
    BfsVertex* n2 = create_vertex("node2");
    BfsVertex* n3 = create_vertex("node3");
    BfsVertex* n4 = create_vertex("node4");
    BfsVertex* n5 = create_vertex("node5");
    BfsVertex* n6 = create_vertex("node6");

    graph_ins_vertex(graph, n1);
    graph_ins_vertex(graph, n2);
    graph_ins_vertex(graph, n3);
    graph_ins_vertex(graph, n4);
    graph_ins_vertex(graph, n5);
    graph_ins_vertex(graph, n6);

    graph_ins_edge(graph, n1, n2);
    graph_ins_edge(graph, n1, n3);
    graph_ins_edge(graph, n2, n3);
    graph_ins_edge(graph, n2, n4);
    graph_ins_edge(graph, n3, n5);
    graph_ins_edge(graph, n4, n5);
    graph_ins_edge(graph, n5, n6);
}

int compare_vertices_by_name(const void* vertex1, const void* vertex2) {
    const BfsVertex* v1 = (const BfsVertex*) vertex1;
    const BfsVertex* v2 = (const BfsVertex*) vertex2;

    return strcmp(v1->data, v2->data);
}

void free_vertex(void* vertex) {
    BfsVertex* v = (BfsVertex*)vertex;
    if (v->path) {
        list_destroy(v->path);
        free(v->path);
    }
    free(v);
}

int main(void) {
    Graph graph;
    List hops;
    ListElmt* elem;

    graph_init(&graph, compare_vertices_by_name, free_vertex);

    build_graph(&graph);

    elem = list_head(&graph_adjlists(&graph));
    BfsVertex* start = ((AdjList*)list_data(elem))->vertex;

    bfs(&graph, start, &hops);

    printf("Shortest path from %s to all other nodes\n", (const char*) start->data);

    for (elem = list_head(&hops); elem != NULL; elem = list_next(elem)) {
        BfsVertex* v1 = ((AdjList*)list_data(elem))->vertex;
        for (ListElmt* elem2 = list_head(v1->path); elem2 != NULL; elem2 = list_next(elem2)) {
            BfsVertex* v2 = ((AdjList*)list_data(elem2))->vertex;
            printf("%s, ", (const char*) v2->data);
        }
        puts(v1->data);
    }

    return 0;
}

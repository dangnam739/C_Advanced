#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libfdr/jrb.h"
#include "libfdr/dllist.h" //Double linked list

typedef struct{
    JRB edges;
    JRB vertices;
} Graph;

Graph createGraph();
void addVertex(Graph graph, int id, char *name);
char *getVertex(Graph graph, int id);
void addEgde(Graph graph, int v1, int v2);
int hasEdge(Graph graph, int v1, int v2);
int indegree(Graph graph, int v, int* output);
int outdegree(Graph graph, int v, int* output);
int getComponents(Graph graph);
int DAG(Graph graph);
void topologicalSort(Graph graph, int *output, int *n);
void dropGraph(Graph graph);

Graph createGraph()
{
    Graph graph;

    graph.edges = make_jrb();
    graph.vertices = make_jrb();

    return graph;
}

void dropGraph(Graph graph)
{
    JRB node, tree;

    jrb_traverse(node, graph.edges)
    {
        tree = (JRB)jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph.edges);
    jrb_free_tree(graph.vertices);
}

void addVertex(Graph graph, int id, char *name)
{
    JRB node = jrb_find_int(graph.vertices, id);

    if(node == NULL)
        jrb_insert_int(graph.vertices, id, new_jval_s(strdup(name)));
}

char *getVertex(Graph graph, int id)
{
    JRB node = jrb_find_int(graph.vertices, id);

    if(node == NULL)
        return NULL;
    else
        return jval_s(node->val);
}

int hasEdge(Graph graph, int v1, int v2)
{
    JRB node, tree;

    node = jrb_find_int(graph.edges, v1);
    if(node == NULL)
        return 0;
    tree = (JRB)jval_v(node->val);
    if(jrb_find_int(tree, v2) == NULL)
        return 0;
    else
        return 1;
}

void addEdge(Graph graph, int v1, int v2)
{
    JRB node, tree;
    if(!hasEdge(graph, v1, v2))
    {
        node = jrb_find_int(graph.edges, v1);
        if(node == NULL)
        {
            tree = make_jrb();
            jrb_insert_int(graph.edges, v1, new_jval_v(tree));
        }
        else
        {
            tree = (JRB)jval_v(node->val);
        }
        jrb_insert_int(tree, v2, new_jval_i(1));
    }
}

int indegree(Graph graph, int v, int *output)
{
    JRB tree, node;
    int total = 0;
    jrb_traverse(node, graph.edges)
    {
        tree = (JRB)jval_v(node->val);
        if(jrb_find_int(tree, v))
        {
            output[total] = jval_i(node->key);
            total++;
        }
    }
    return total;
}

int outdegree(Graph graph, int v, int *output)
{
    JRB tree, node;
    int total = 0;

    node = jrb_find_int(graph.edges, v);

    if(node == NULL)
        return 0;

    tree = (JRB)jval_v(node->val);

    jrb_traverse(node, tree)
    {
        output[total] = jval_i(node->key);
        total++;
    }
    return total;
}

int DAG(Graph graph)
{
    int visited[1000] = {};
    int n, output[100], i, u, v, start;
    Dllist node, stack;
    JRB vertex;

    jrb_traverse(vertex, graph.vertices)
    {
        memset(visited, 0, sizeof(visited));//copy size character 0 into size first vistied
        start = jval_i(vertex->key);
        stack = new_dllist();
        dll_append(stack, new_jval_i(start)); /* Inserts at the end of the list */

        while(!dll_empty(stack))
        {
            node = dll_last(stack);
            u = jval_i(node->val);
            dll_delete_node(node);

            if(!visited[u])
            {
                visited[u] = 1;
                n = outdegree(graph, u, output);
                for (i = 0; i < n; i++)
                {
                    v = output[i];
                    if (v == start) //cycle detected
                        return 0;
                    if(!visited[v])
                        dll_append(stack, new_jval_i(v));
                }
            }
        }
    }

    return 1;//no cycle
}

void topologicalSort(Graph graph, int *output, int *n)
{
    int indeg[1000], tmp[100], m, i, u, v, total;
    JRB vertex;
    Dllist node, queue;

    queue = new_dllist();
    jrb_traverse(vertex, graph.vertices)
    {
        u = jval_i(vertex->key);
        indeg[u] = indegree(graph, u, tmp);//tong cac dinh vao u, luu tru trong tmp
        if(indeg[u] == 0)//dinh ko co cung vao
            dll_append(queue, new_jval_i(u));//chen vao queue
    }
    total = 0;//chi so output cho ra topo sort
    while(!dll_empty(queue))
    {
        node = dll_first(queue);
        u = jval_i(node->val);//dinh cua node
        dll_delete_node(node);
        output[total++] = u;//cho dinh u vao list
        m = outdegree(graph, u, tmp); //tong so cung di ra tu u
        for (i = 0; i < m;i++)
        {
            v = tmp[i];//dinh ma u chi toi
            indeg[v]--;//xoa bot lien ket tu u->v
            if(indeg[v] == 0)
                dll_append(queue, new_jval_i(v));
        }
    }
    *n = total;
}
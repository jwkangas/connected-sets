#include <stdio.h>
#include "graph.h"

int n;					// number of vertices
int adj[MAXN][MAXN];	// adjacency matrix
int deg[MAXN];			// degree array

// initializes (empties and sets n to 0)
void init_graph()
{
	n = 0;
	
	for (int i = 0; i < MAXN; i++) {
		deg[i] = 0;
		
		for (int j = 0; j < MAXN; j++) {
			adj[i][j] = 0;
		}
	}
}

// adds edge (i,j) (must not be called if (i,j) already added)
void add_edge(int i, int j)
{
	adj[i][j] = 1;
	adj[j][i] = 1;
	deg[i]++;
	deg[j]++;
}

// deletes edge (i,j) (must not be called if (i,j) not added)
void del_edge(int i, int j)
{
	adj[i][j] = 0;
	adj[j][i] = 0;
	deg[i]--;
	deg[j]--;
}

// returns true if has edge (i,j)
int has_edge(int i, int j)
{
	return adj[i][j];
}

// prints an easily machine-readable version of the graph
void print_graph()
{
	printf("%i", n-1);
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			if (!has_edge(i, j)) continue;
			printf(" %i %i", i, j);
		}
	}
	printf(" -1\n");
}

// prints a more readable version of the graph
void print_graph_readable()
{
// 	printf("%2i  ", n-1);
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n-1; j++) {
			if (has_edge(i, j)) printf("%i-%i ", i, j);
		}
	}
	printf("\n");
}

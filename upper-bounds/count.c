#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "graph.h"

/* tools for enumerating boundary-connected sets */

int neighbors[MAXN][MAXN];	// list of neighbors of each vertex
int included[MAXN];			// a bit array to mark included vertices
int excluded[MAXN];			// a bit array to mark excluded vertices
int empty;					// true if 'included' is empty
int n_connected;			// number of connected sets

// empties 'included'
void empty_set()
{
	for (int i = 0; i < n; i++) included[i] = 0;
}

// returns true iff node i is a neighbor of an included node
int in_fringe(int i)
{
	for (int j = 0; j < deg[i]; j++) {
		if (included[neighbors[i][j]]) return 1;
	}
	
	return 0;
}

// enumerates all connected sets that contain all vertices in 'included'
// and none of the vertices in 'excluded'
void connected_interval()
{
	int exc[n], en = 0;		// list of vertices exluded in this call
	n_connected++;			// count the set 'included'
	
	// if 'included' is empty, recurse on all singleton sets
	if (empty) {
		empty = 0;
		for (int i = 0; i < n; i++) {
			included[i] = 1;		// included in this call
			connected_interval();
			included[i] = 0;		// excluded in all following calls
			excluded[i] = 1;
		}
		return;
	}
	
	// otherwise recurse on sets obtained by adding a neighbor of 'included'
	for (int i = 0; i < n; i++) {
		// skip if already included or excluded
		if (included[i] || excluded[i]) continue;
		// skip if not a neighbor of any included vertex
		if (!in_fringe(i)) continue;
		
		// otherwise
		included[i] = 1;		// include and recurse
		connected_interval();
		included[i] = 0;		// exclude for the following calls
		excluded[i] = 1;
		exc[en++] = i;			// add to excluded list
	}
	
	// remove 'excluded' status of all those excluded in this call
	for (int i = 0; i < en; i++) excluded[exc[i]] = 0;
}

// enumerates all connected supersets of 'included'
// the array will be restored to its original state after the call
void connected_supersets()
{
	empty = 1;
	
	// initialize and build a neighbor list for each vertex
	for (int i = 0; i < n; i++) {
		if (included[i]) empty = 0;
		int k = excluded[i] = 0;
		for (int j = 0; j < n; j++) {
			if (has_edge(i,j)) neighbors[i][k++] = j;
		}
	}
	
	// since 'excluded' is empty, this will enumerate supersets of 'included'
	connected_interval();
}

// enumerates and counts all boundary-connected sets
// the last vertex of the graph is considered the boundary vertex
int count_boundary_connected()
{
	n_connected = 0;
	empty_set();
	included[n-1] = 1;		// always include the boundary vertex
	connected_supersets();	// count the connected combinations of remaining vertices
	return n_connected;
}

/* tools for counting end here */



int moore_bound;
double max_ratio;
double max_bound;

// computes the ratio between the number of boundary-connected sets and all subsets
void compute_ratio()
{
	double ratio = (double)count_boundary_connected() / (1 << (n-1));
	
	if (ratio <= max_ratio) return;
	max_ratio = ratio;
	
	// as a result, a bound on the number of connected sets is obtained
	double bound = 2 * pow(ratio, 1.0 / moore_bound);
	max_bound = bound;
	printf("  Found a neighborhood with a higher ratio: ");
	print_graph_readable();
	printf("  Boundary-connected sets: %6i    Ratio: %f    Bound: %f\n", n_connected, ratio, bound);
}

// reads a graph from standard input
int read_graph()
{
	init_graph();
	int k;
	assert(scanf("%d", &k) == 1);
	if (k == -1) return 0;
	n = k + 1;
	for (;;) {
		int u, v;
		assert(scanf("%d", &u) == 1);
		if (u == -1) return 1;
		assert(scanf("%d", &v) == 1);
		add_edge(u, v);
	}
}

int main(int argc, const char **argv)
{
	if (argc < 2) {
		printf("Usage: %s <degree>\n", argv[0]);
		return 0;
	}
	
	int max_deg = atoi(argv[1]);
	moore_bound = max_deg * max_deg + 1;
	max_ratio = 0.0;
	max_bound = 0.0;
	int prev_n = 0;
	
	if (max_deg > 10) {
		eprintf("The degree is too large.\n");
		return 0;
	}
	
	while (read_graph()) {
		if (n != prev_n) {
			printf("n=%i\n", n-1);
			prev_n = n;
		}
		compute_ratio();
	}
	
	printf("Bound: %f^n\n", max_bound);
	
	return 0;
}

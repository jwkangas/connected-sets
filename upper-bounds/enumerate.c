#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"

int max_deg;	// maximum degree
int p, q;		// number of vertices at depth 1 and 2

long long unsigned search_space_size;

// returns 0 iff there are edges that can still be added
int satisfied()
{
	if (deg[n-1] == 0) return 0;
	
	for (int u = 1; u < 1 + p; u++) {
		for (int w = u+1; w < n-1; w++) {
			// if edge (u,w) not added but could be added
			if (!has_edge(u,w) && deg[u] < max_deg && deg[w] < max_deg) return 0;
		}
	}
	
	return 1;
}

// finish a neighborhood by adding edges to the outer vertex, and output
void choose_12_configuration()
{
	// add an edge from all unsatisfied 2-vertices to the outer vertex
	for (int w = 1 + p; w < 1 + p + q; w++) {
		if (deg[w] < max_deg) add_edge(w, n-1);
	}
	
	// if the neighborhood is satisfied, output
	if (satisfied()) {
		search_space_size++;
		print_graph();
	}
	
	// remove the added edges
	for (int w = 1 + p; w < 1 + p + q; w++) {
		if (has_edge(w, n-1)) del_edge(w, n-1);
	}
}

// returns w's set of neighbors encoded as an integer
int neighbor_set(int w)
{
	unsigned int power = 1 << (p - 1);
	unsigned int n = 0;
	
	for (int u = 1; u < 1 + p; u++) {
		n += power * has_edge(u, w);
		power = power >> 1;
	}
	
	return n;
}

// decides on edge (u,w), branches and recurses on the next edge
// 1-neighbors are chosen for each 2-vertex before moving to the next 2-vertex
// pnset is the neighbor set of w-1
void branch_on_12_edge(int u, int w, int pnset)
{
// 	assert(pnset >= 0);
	// if w is past the last 2-vertex, fix the 1-2-configuration
	if (w == n - 1) {
		choose_12_configuration();
		return;
	}
	
	// if u is past the last 1-vertex, move to (1,w+1)
	if (u == 1 + p) {
		// w's set of 1-neighbors, encoded as an integer
		int nset = neighbor_set(w);
		
		// w's neighbor set must not be empty
		// or lexicographically larger than (w-1)'s
		if (nset == 0) return;
		if (nset > pnset) return;
		
		// choose 1-vertex neighbors for the next 2-vertex
		branch_on_12_edge(1, w+1, nset);
		return;
	}
	
	// branch on adding and not adding (u,w), without breaking degree bounds
	if (deg[u] < max_deg && deg[w] < max_deg) {
		add_edge(u, w);
		branch_on_12_edge(u+1, w, pnset);
		del_edge(u, w);
	}
	
	branch_on_12_edge(u+1, w, pnset);
}

// returns 0 iff the 1-configuration isn't canonical
int canonical_degree_order()
{
	// a vertex with a higher label must not have a higher degree
	for (int u = 2; u < 1 + p; u++) {
		if (deg[u] > deg[u-1]) return 0;
	}
	
	return 1;
}

// decide on (u,u_)
void branch_on_11_edge(int u, int u_)
{
	// if u is the last 1-vertex, fix configuration
	if (u == p) {
		// if the configuration isn't canonical, prune out
		if (!canonical_degree_order()) return;
		// otherwise continue to branch on 12-edges
		branch_on_12_edge(1, 1 + p, 1 << n);
		return;
	}
	
	// if u_ is past the last 1-vertex
	if (u_ == 1 + p) {
		branch_on_11_edge(u+1, u+2);
		return;
	}
	
	// branch between adding and not adding (u,u_)
	add_edge(u, u_);
	branch_on_11_edge(u, u_+1);
	del_edge(u, u_);
	branch_on_11_edge(u, u_+1);
}

void choose_pq_configuration()
{
	eprintf("  p=%-2i, q=%-2i      #graphs = %llu\n", p, q, search_space_size);
	
	// add 0-1 edges
	for (int u = 1; u <= p; u++) add_edge(0, u);
	
	// enumerate configurations between 1-vertices
	branch_on_11_edge(1, 2);
	
	// remove 0-1 edges
	for (int u = 1; u <= p; u++) del_edge(0, u);
}

// enumerate all possible numbers of vertices at each depth
void enumerate_pq_configurations()
{
	// fix the number of vertices at depth 1 (p) and 2 (q)
	for (p = 1; p <= max_deg; p++) {
		for (q = 1; q <= p * (max_deg - 1); q++) {
			if (1 + p + q != n - 1) continue;
			choose_pq_configuration();
		}
	}
}

// enumerate neighborhoods of size k
void enumerate_neighborhoods(int k)
{
	eprintf("n=%i\n", k);
	
	// for technical reasons, we increment n by one to accommodate the outside vertex
	init_graph();
	n = k + 1;
	
	enumerate_pq_configurations();
}

int main(int argc, const char **argv)
{
	if (argc < 2) {
		eprintf("Usage: %s <degree> [<neighborhood size>]\n", argv[0]);
		return 0;
	}
	
	max_deg = atoi(argv[1]);
	
	if (max_deg < 3) {
		eprintf("Degree must be at least 3.\n");
		return 0;
	}
	
	int max_size = max_deg * max_deg + 1;
	search_space_size = 0;
	
	if (max_size > MAXN) {
		eprintf("The degree is too large. This program can feasibly enumerate neighborhood graphs up to degree 5.\n");
		return 0;
	}
	
	if (argc < 3) {
		for (int k = 3; k <= max_size; k++) {
			enumerate_neighborhoods(k);
		}
	} else {
		int k = atoi(argv[2]);
		if (k < 3 || k > max_size) {
			eprintf("Size must be within [3,%i].\n", max_size);
			return 0;
		}
		enumerate_neighborhoods(k);
	}
	
	eprintf("Total number of enumerated graphs: %llu\n", search_space_size);
	
	// mark the end of output
	printf("-1\n");
	return 0;
}

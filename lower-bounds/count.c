
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int type;
int n;
int **neighbors;
int *degree;

int *included;
int *excluded;
// int *status;
long long unsigned int n_connected;
long long unsigned int n_connected_u;
long long unsigned int n_connected_d;
long long unsigned int n_connected_b;


// returns true iff node i is a neighbor of an included node
int in_fringe(int i)
{
	for (int j = 0; j < degree[i]; ++j) {
		if (included[neighbors[i][j]]) return 1;
	}
	
	return 0;
}

// counts the connected sets in the interval specified by 'included' and 'excluded'
void count_type_i_interval()
{
	int exc[n], en = 0;  // list of vertices exluded in this call
	n_connected++;       // count the set 'included'
	
	// recurse on sets obtained by adding a neighbor of 'included'
	for (int i = 1; i < n; i++) {
		// skip if already included or excluded
		if (included[i] || excluded[i]) continue;
		// skip if not a neighbor of any included vertex
		if (!in_fringe(i)) continue;
		
		// otherwise
		included[i] = 1;      // include and recurse
		count_type_i_interval();
		included[i] = 0;      // exclude for the following calls
		excluded[i] = 1;
		exc[en++] = i;        // remember that i was excluded in this call
	}
	
	// remove 'excluded' status of all those excluded in this call
	for (int i = 0; i < en; i++) excluded[exc[i]] = 0;
}

// counts the number of connected sets in a gadget of type I
void count_type_i()
{
	for (int i = 0; i < n; ++i) {
		included[i] = 0;
		excluded[i] = 0;
	}
	
	included[0] = 1;       // always include the join vertex
	n_connected = 0;
	count_type_i_interval();
	printf("%llu\n", n_connected);
}

// this is similar to count_type_i_interval, but now the status of the vertices 0 and 1 is fixed,
// and we count separately the combinations of vertices 2 and 3
void count_type_ii_interval()
{
	int exc[n], en = 0;  // list of vertices exluded in this call
	
	// increment the appropriate counter
	if (included[2]) {
		if (included[3]) {
			++n_connected_b;
		} else {
			++n_connected_u;
		}
	} else if (included[3]) {
		++n_connected_d;
	}
	
	// recurse on sets obtained by adding a neighbor of 'included'
	for (int i = 2; i < n; i++) {
		// skip if already included or excluded
		if (included[i] || excluded[i]) continue;
		// skip if not a neighbor of any included vertex
		if (!in_fringe(i)) continue;
		
		// otherwise
		included[i] = 1;      // include and recurse
		count_type_ii_interval();
		included[i] = 0;      // exclude for the following calls
		excluded[i] = 1;
		exc[en++] = i;        // remember that i was excluded in this call
		
	}
	
	// remove 'excluded' status of all those excluded in this call
	for (int i = 0; i < en; i++) excluded[exc[i]] = 0;
}

void count_type_ii_uv(int u, int v)
{
	// TODO: could be replaced by a single array
	for (int i = 0; i < n; ++i) {
		included[i] = 0;
		excluded[i] = 0;
	}
	
	included[0] = u;
	included[1] = v;
	
	n_connected_u = 0;
	n_connected_d = 0;
	n_connected_b = 0;
	
	count_type_ii_interval();
	
	printf("%llu\n", n_connected_b);
	printf("%llu\n", n_connected_u);
	printf("%llu\n", n_connected_d);
}

void count_type_ii()
{
	count_type_ii_uv(1, 1);   // count sets with both vertices u and v
	count_type_ii_uv(1, 0);   // count sets with with only u
	count_type_ii_uv(0, 1);   // count sets with with only v
}


int main(int argc, char **argv)
{
	// check that type and n are given and each edge has two vertices
	assert(argc >= 3);
	assert(argc % 2 == 1);
	
	// read gadget type
	if (!strcmp(argv[1], "I")) {
		type = 1;
	} else if (!strcmp(argv[1], "II")) {
		type = 2;
	} else {
		exit(1);
	}
	
	// read n
	n = atoi(argv[2]);
	
	neighbors = malloc(n * sizeof(int*));
	degree = malloc(n * sizeof(int));
	included = malloc(n * sizeof(int));
	excluded = malloc(n * sizeof(int));
// 	status = malloc(n * sizeof(int));
	
	for (int i = 0; i < n; ++i) {
		neighbors[i] = malloc(n * sizeof(int));
		degree[i] = 0;
	}
	
	// read the edges
	for (int i = 3; i < argc; i+=2) {
		int u = atoi(*(argv+i));
		int v = atoi(*(argv+i+1));
		neighbors[u][degree[u]] = v;
		neighbors[v][degree[v]] = u;
		++degree[u];
		++degree[v];
	}
	
	if (type == 1) {
		count_type_i();
	} else if (type == 2) {
		count_type_ii();
	}
	
	return 0;
}

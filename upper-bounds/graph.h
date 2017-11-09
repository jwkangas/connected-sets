
#define MAXN 128
#define eprintf(...) fprintf (stderr, __VA_ARGS__)

void init_graph();
void add_edge(int i, int j);
void del_edge(int i, int j);
int has_edge(int i, int j);
void print_graph();
void print_graph_readable();

extern int n;
extern int adj[MAXN][MAXN];
extern int deg[MAXN];

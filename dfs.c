typedef struct StackRep *Stack;
static Stack newStack();
static void  disposeStack(Stack);
static void  pushOnto(Stack,char *);
static char  *popFrom(Stack);
static int   emptyStack(Stack);
static void  showStack(Stack s);

int dfs (hunter_view *hv, location_t src, location_t dest, location_t *path) {

    assert(hv->world_map != NULL);    
    int visited[hv->world_map->n_vertices];
    location_t st[hv->world_map->n_vertices];
    
    

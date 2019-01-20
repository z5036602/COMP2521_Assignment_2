////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.c: an implementation of a Map type
// You can change this as much as you want!
//
// 2017-11-30 v1.0 Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31 v2.0 Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "map.h"
#include "places.h"



static void add_connections(map *);
static void add_connection(map *, location_t, location_t, transport_t);
static inline bool is_sentinel_edge(connection);

static map_adj *adjlist_insert(map_adj *, location_t, transport_t);
static bool adjlist_contains(map_adj *, location_t, transport_t);

// Create a new empty graph (for a map)
// #Vertices always same as NUM_PLACES
map *map_new(void)
{
	map *g = malloc(sizeof *g);
	if (g == NULL)
		err(EX_OSERR, "couldn't allocate Map");

	(*g) = (map){
		.n_vertices = NUM_MAP_LOCATIONS,
		.n_edges = 0,
		.connections = {NULL}};

	add_connections(g);
	return g;
}

// Remove an existing graph
void map_drop(map *g)
{
	assert(g != NULL);

	for (size_t i = 0; i < g->n_vertices; i++)
	{
		map_adj *curr = g->connections[i];
		while (curr != NULL)
		{
			map_adj *next = curr->next;
			free(curr);
			curr = next;
		}
	}
	free(g);
}

// Display content of Map/Graph
void map_show(map *g)
{
	assert(g != NULL);

	printf("V=%zu, E=%zu\n", g->n_vertices, g->n_edges);
	for (size_t i = 0; i < g->n_vertices; i++)
		for (map_adj *curr = g->connections[i];
			 curr != NULL; curr = curr->next)
			printf(
				"%s connects to %s by %s\n",
				location_get_name((location_t)i),
				location_get_name(curr->v),
				transport_to_s(curr->type));
}

// Return count of nodes
size_t map_nv(map *g)
{
	assert(g != NULL);
	return g->n_vertices;
}

// Return count of edges of a particular type
size_t map_ne(map *g, transport_t type)
{
	assert(g != NULL);
	assert(valid_transport_p(type) || type == ANY);

	size_t n_edges = 0;

	for (size_t i = 0; i < g->n_vertices; i++)
		for (map_adj *curr = g->connections[i];
			 curr != NULL; curr = curr->next)
			if (curr->type == type || type == ANY)
				n_edges++;

	return n_edges;
}

/// Add edges to Graph representing map of Europe
static void add_connections(map *g)
{
	assert(g != NULL);

	for (size_t i = 0; !is_sentinel_edge(CONNECTIONS[i]); i++)
		add_connection(
			g,
			CONNECTIONS[i].v, CONNECTIONS[i].w,
			CONNECTIONS[i].t);
}

/// Add a new edge to the Map/Graph
static void add_connection(
	map *g,
	location_t start,
	location_t end,
	transport_t type)
{
	assert(g != NULL);
	assert(start != end);
	assert(valid_location_p(start));
	assert(valid_location_p(end));
	assert(valid_transport_p(type));

	// don't add edges twice
	if (adjlist_contains(g->connections[start], end, type))
		return;

	g->connections[start] = adjlist_insert(g->connections[start], end, type);
	g->connections[end] = adjlist_insert(g->connections[end], start, type);
	g->n_edges++;
}

/// Is this the magic 'sentinel' edge?
static inline bool is_sentinel_edge(connection x)
{
	return x.v == -1 && x.w == -1 && x.t == ANY;
}

/// Insert a node into an adjacency list.
static map_adj *adjlist_insert(map_adj *list, location_t v, transport_t type)
{
	assert(valid_location_p(v));
	assert(valid_transport_p(type));

	map_adj *new = malloc(sizeof *new);
	if (new == NULL)
		err(EX_OSERR, "couldn't allocate map_adj");
	(*new) = (map_adj){.v = v, .type = type, .next = list};
	return new;
}

/// Does this adjacency list contain a particular value?
static bool adjlist_contains(map_adj *list, location_t v, transport_t type)
{
	assert(valid_location_p(v));
	assert(valid_transport_p(type));

	for (map_adj *curr = list; curr != NULL; curr = curr->next)
		if (curr->v == v && curr->type == type)
			return true;
	return false;
}
location_t *check_for_connections_0_round(Map g, location_t src, bool road, bool rail, bool sea, size_t *n_locations)
{
	assert(g != NULL);
	map_adj *curr = g->connections[src];
	location_t adj_locations_tmp[NUM_MAP_LOCATIONS];
	adj_locations_tmp[0] = src;
	bool visted[NUM_MAP_LOCATIONS] = {false};
	visted[src] = true;
	int counter = 1;

	while (curr != NULL)
	{
		//if (curr->v == end ){
		if (road == true && curr->type == ROAD && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (sea == true && curr->type == BOAT && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		curr = curr->next;
	}
	location_t *adj_locations = malloc((counter) * sizeof(location_t)); /// need to free !!!
	int i = 0;
	for (i = 0; i < counter; i++)
	{
		adj_locations[i] = adj_locations_tmp[i];
	}
	//adj_locations[i]= src;
	*n_locations = counter;
	return adj_locations;
}
location_t *check_for_connections_1_round(Map g, location_t src, bool road, bool rail, bool sea, size_t *n_locations)
{
	assert(g != NULL);
	map_adj *curr = g->connections[src];
	location_t adj_locations_tmp[NUM_MAP_LOCATIONS];
	adj_locations_tmp[0] = src;
	bool visted[NUM_MAP_LOCATIONS] = {false};
	visted[src] = true;
	int counter = 1;

	while (curr != NULL)
	{
		//if (curr->v == end ){
		if (road == true && curr->type == ROAD && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (rail == true && curr->type == RAIL && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (sea == true && curr->type == BOAT && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		curr = curr->next;
	}
	location_t *adj_locations = malloc((counter) * sizeof(location_t)); /// need to free !!!
	int i = 0;
	for (i = 0; i < counter; i++)
	{
		adj_locations[i] = adj_locations_tmp[i];
	}
	//adj_locations[i]= src;
	*n_locations = counter;
	return adj_locations;
}
location_t *check_for_connections_2_round(Map g, location_t src, bool road, bool rail, bool sea, size_t *n_locations)
{
	assert(g != NULL);
	map_adj *curr = g->connections[src];
	location_t adj_locations_tmp[NUM_MAP_LOCATIONS];
	adj_locations_tmp[0] = src;
	bool visted[NUM_MAP_LOCATIONS] = {false};
	visted[src] = true;
	int counter = 1;

	while (curr != NULL)
	{
		//if (curr->v == end ){
		if (road == true && curr->type == ROAD && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (rail == true && curr->type == RAIL && visted[curr->v] == false)
		{
			if (visted[curr->v] == false)
			{
				adj_locations_tmp[counter] = curr->v;
				visted[curr->v] = true;
				counter++;
			}
			map_adj *curr_2 = g->connections[curr->v];
			while (curr_2 != NULL)
			{
				if (curr_2->type == RAIL)
				{
					if (visted[curr_2->v] == false)
					{
						adj_locations_tmp[counter] = curr_2->v;
						visted[curr_2->v] = true;
						counter++;
					}
				}
				curr_2 = curr_2->next;
			}
		}
		if (sea == true && curr->type == BOAT && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		curr = curr->next;
	}
	location_t *adj_locations = malloc((counter) * sizeof(location_t)); /// need to free !!!
	int i = 0;
	for (i = 0; i < counter; i++)
	{
		adj_locations[i] = adj_locations_tmp[i];
	}
	//adj_locations[i]= src;
	*n_locations = counter;
	return adj_locations;
}
location_t *check_for_connections_3_round(Map g, location_t src, bool road, bool rail, bool sea, size_t *n_locations)
{
	assert(g != NULL);
	map_adj *curr = g->connections[src];
	location_t adj_locations_tmp[NUM_MAP_LOCATIONS];
	adj_locations_tmp[0] = src;
	bool visted[NUM_MAP_LOCATIONS] = {false};
	visted[src] = true;
	int counter = 1;
	while (curr != NULL)
	{
		//if (curr->v == end ){
		if (road == true && curr->type == ROAD && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (rail == true && curr->type == RAIL)
		{
			if (visted[curr->v] == false)
			{
				adj_locations_tmp[counter] = curr->v;
				visted[curr->v] = true;
				counter++;
			}
			map_adj *curr_2 = g->connections[curr->v];
			while (curr_2 != NULL)
			{
				if (curr_2->type == RAIL)
				{
					if (visted[curr_2->v] == false)
					{
						adj_locations_tmp[counter] = curr_2->v;
						visted[curr_2->v] = true;
						counter++;
					}
					map_adj *curr_3 = g->connections[curr_2->v];
					while (curr_3 != NULL)
					{
						if (curr_3->type == RAIL)
						{
							if (visted[curr_3->v] == false)
							{
								adj_locations_tmp[counter] = curr_3->v;
								visted[curr_3->v] = true;
								counter++;
							}
						}
						curr_3 = curr_3->next;
					}
				}
				curr_2 = curr_2->next;
			}
		}
		if (sea == true && curr->type == BOAT && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		curr = curr->next;
	}
	location_t *adj_locations = malloc((counter) * sizeof(location_t)); /// need to free !!!
	int i = 0;
	for (i = 0; i < counter; i++)
	{
		adj_locations[i] = adj_locations_tmp[i];
	}
	//adj_locations[i]= src;
	*n_locations = counter;
	return adj_locations;
}
location_t *check_for_connections_dracula(Map g, location_t src, bool road, bool rail, bool sea, size_t *n_locations)
{
	assert(g != NULL);
	//assert(1==2);
	map_adj *curr = g->connections[src];
	location_t adj_locations_tmp[NUM_MAP_LOCATIONS];
	adj_locations_tmp[0] = src;
	bool visted[NUM_MAP_LOCATIONS] = {false};
	visted[src] = true;
	int flag = 0;
	int counter = 1;

	while (curr != NULL)
	{
		if (curr->type == ROAD && curr->v != ST_JOSEPH_AND_ST_MARYS)
		{
			flag = 1;
		}
		if (curr->type == BOAT && curr->v != ST_JOSEPH_AND_ST_MARYS)
		{
			flag = 1;
		}

		if (road == true && curr->type == ROAD && curr->v != ST_JOSEPH_AND_ST_MARYS && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		if (sea == true && curr->type == BOAT && curr->v != ST_JOSEPH_AND_ST_MARYS && visted[curr->v] == false)
		{
			adj_locations_tmp[counter] = curr->v;
			visted[curr->v] = true;
			counter++;
		}
		curr = curr->next;
	}
	if (flag == 0)
	{
		adj_locations_tmp[1] = TELEPORT;
		counter++;
	}
	location_t *adj_locations = malloc((counter) * sizeof(location_t)); /// need to free !!!
	int i = 0;
	for (i = 0; i < counter; i++)
	{
		adj_locations[i] = adj_locations_tmp[i];
	}
	//adj_locations[i]= src;
	*n_locations = counter;
	return adj_locations;
}

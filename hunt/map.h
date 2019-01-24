////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.h: an interface to a Map data type
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>

#include <stdbool.h>
#include <stdlib.h>

#include "places.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

typedef struct edge {
    location_t start, end;
    transport_t type;
} edge;

typedef struct map_adj map_adj;
typedef struct map
{
	size_t n_vertices, n_edges;

	struct map_adj
	{
		location_t v;					// ALICANTE, etc
		transport_t type;				// ROAD, RAIL, BOAT
		struct map_adj *next;			// link to next node
	} * connections[NUM_MAP_LOCATIONS]; // array of lists
} map;

// graph representation is hidden
typedef struct map *Map;

/** Create a new Map. */
Map map_new (void);
/** Release resources associated with a Map. */
void map_drop (Map);
/** Print a Map to `stdout`. */
void map_show (Map);
/** Get the number of vertices. */
size_t map_nv (Map);
/** Get the number of edges. */
size_t map_ne (Map, transport_t);
/** Get the adj_matrix of edges. */
location_t* check_for_connections_0_round(Map g, location_t src, bool road, bool sea,size_t *n_locations);
location_t* check_for_connections_1_round(Map g, location_t src, bool road, bool rail, bool sea,size_t *n_locations);
location_t* check_for_connections_2_round(Map g, location_t src, bool road, bool rail, bool sea,size_t *n_locations);
location_t* check_for_connections_3_round(Map g, location_t src, bool road, bool rail, bool sea,size_t *n_locations);
location_t* check_for_connections_dracula(Map g, location_t src, bool road, bool sea,size_t *n_locations);
#endif // !defined(FOD__MAP_H_)

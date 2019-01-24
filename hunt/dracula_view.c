////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <stdio.h>

#include "places.h"
#include "dracula_view.h"
#include "game.h"
#include "game_view.h"
#include "map.h"
//tingting
//#include "DDList.h"
// #include "map.h" ... if you decide to use the Map ADT

typedef struct DLListNode
{
	location_t location; // value of this list item (string)
	struct DLListNode *prev;
	// pointer previous node in list
	struct DLListNode *next;
	// pointer to next node in list
} DLListNode;

typedef struct DLListRep
{
	size_t nitems;	 // count of items in list
	DLListNode *first; // first node in list
	DLListNode *curr;  // current node in list
	DLListNode *last;  // last node in list
} DLListRep;

typedef struct DLListRep *DLList;

enum
{
	MAX_TRAPS_NUMBER = 6
};

typedef struct dracula_view
{
	
	GameView gv;											//parent gv ADT
	Map world_map;											//graph ADT
	char *past_plays;									
	player_message *messages;
	DLList player_path[NUM_PLAYERS];						//path for each player
	DLList dracula_real_path;								//dracula real path
	size_t num_of_turns;									//number of turns 
	int traps[MAX_TRAPS_NUMBER];							//record trap locations
	int is_vampire_alive;									//whether the vampier is alive
	location_t vampire_location;							//current vampire locations
} dracula_view;

//helper function
static void trap_update(dracula_view *dv);
static location_t abbrevToLocation(char *abbrev);
static void hunter_update_location(dracula_view *dv, enum player player, location_t new_location);
static void apply_hunter_rule(dracula_view *dv, char *token, location_t this_location);
static void dracula_update_location(dracula_view *dv, location_t new_location);
static void apply_dracula_rule(dracula_view *dv, char *token, location_t this_location);
static DLList newDLList(void);
static void freeDLList(DLList L);
static void DLListInsert(DLList L, location_t location);
static void original_string_to_real(char *string);
static location_t *location_dracula_can_move(
	dracula_view *dv, size_t *n_locations,
	location_t from, bool road, bool sea);

dracula_view *dv_new(char *past_plays, player_message messages[])
{
	dracula_view *new = malloc(sizeof *new);
	if (new == NULL)
		err(EX_OSERR, "couldn't allocate DraculaView");
	(*new) = (dracula_view){
		.gv = gv_new(past_plays, messages),
		.dracula_real_path = newDLList(),
		.num_of_turns = 0,
		.past_plays = strdup(past_plays),
		.messages = NULL,
		.is_vampire_alive = 0,
		.world_map = map_new()};
	//initialize player life and path
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		new->player_path[i] = newDLList();
	}
	//initialize trap
	for (int i = 0; i < MAX_TRAPS_NUMBER; i++)
		new->traps[i] = UNKNOWN_LOCATION;
	//check past_plays
	if (strcmp(past_plays, "") == 0)
		return new;

	char *token, *s;
	s = strdup(past_plays);
	char *start = s;                             
	original_string_to_real(s);													  // change the TP HI DB to its real location
	while ((token = strsep(&s, " ")) != NULL)                                     // parse the string by " " into each node 
	{
		char abbrev[3];
		abbrev[0] = token[1];
		abbrev[1] = token[2];
		abbrev[2] = '\0';
		location_t this_location = abbrevToLocation(abbrev);

		enum player this_player = new->num_of_turns % NUM_PLAYERS;
		if (this_player == PLAYER_DRACULA)
		{
			dracula_update_location(new, this_location);
			apply_dracula_rule(new, token, this_location);
		}
		else
		{
			hunter_update_location(new, this_player, this_location);
			apply_hunter_rule(new, token, this_location);
		}
		new->num_of_turns++;
	}
	free(s);
	free(start);              
	new->messages = malloc((new->num_of_turns / NUM_PLAYERS) * sizeof(player_message));
	/*size_t j = 0;
	for (size_t i = PLAYER_DRACULA; i < new->num_of_turns; i += NUM_PLAYERS)
	{
		strcpy(new->messages[j], messages[i]);
		j++;
	}*/
	return new;
}

void dv_drop(dracula_view *dv)
{
	gv_drop(dv->gv);
	map_drop(dv->world_map);
	free(dv->past_plays);
	if (dv->messages != NULL)
		free(dv->messages);
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		freeDLList(dv->player_path[i]);
	}
	freeDLList(dv->dracula_real_path);
	free(dv);
}

round_t dv_get_round(dracula_view *dv)
{
	assert(dv!=NULL);
	return gv_get_round(dv->gv);
}

int dv_get_score(dracula_view *dv)
{
	assert(dv!=NULL);
	return gv_get_score(dv->gv);
}

int dv_get_health(dracula_view *dv, enum player player)
{
	assert(dv!=NULL);
	return gv_get_health(dv->gv, player);
}

location_t dv_get_location(dracula_view *dv, enum player player)
{
	assert(dv!=NULL);
	if (player == PLAYER_DRACULA)
	{
		if (dv->dracula_real_path->nitems == 0)
			return UNKNOWN_LOCATION;
		else
			return dv->dracula_real_path->last->location;
	}
	else
		return gv_get_location(dv->gv, player);
}

void dv_get_player_move(
	dracula_view *dv, enum player player,
	location_t *start, location_t *end)
{
	assert(dv!=NULL);
	location_t trail[TRAIL_SIZE];
	gv_get_history(dv->gv, player, trail);
	*end = trail[0];
	*start = trail[1];
	return;
}

void dv_get_locale_info(
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	assert(dv!=NULL);
	if (dv->is_vampire_alive && dv->vampire_location == where)
		*n_vamps = dv->is_vampire_alive;
	else
		*n_vamps = 0;
	*n_traps = 0;
	for (int i = 0; i < MAX_TRAPS_NUMBER; i++)
	{
		if (dv->traps[i] == where)
			(*n_traps)++;
	}
	return;
}

void dv_get_trail(
	dracula_view *dv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	assert(dv!=NULL);
	if (player == PLAYER_DRACULA)
	{
		DLListNode *curr = dv->dracula_real_path->last;
		for (size_t i = 0; i < TRAIL_SIZE; i++)
		{
			if (i < dv->dracula_real_path->nitems)
			{
				trail[i] = curr->location;
				curr = curr->prev;
			}
			else
				trail[i] = -1;
		}
	}
	else
		gv_get_history(dv->gv, player, trail);
}

location_t *dv_get_dests(											//consider the trail
	dracula_view *dv, size_t *n_locations, bool road, bool sea)
{
	assert(dv!=NULL);
	*n_locations = 0;
	if (dv->dracula_real_path->last != NULL)
	{
		return location_dracula_can_move(dv, n_locations, dv->dracula_real_path->last->location, road, sea);
	}
	else
	{
		return NULL;
	}
}

location_t *dv_get_dests_player(                              //consider the trail 
	dracula_view *dv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	assert(dv!=NULL);
	*n_locations = 0;
	if (player == PLAYER_DRACULA)
		return dv_get_dests(dv, n_locations, road, sea);
	else
	{
		if (dv->player_path[player]->last == NULL)
			return NULL;
		return gv_get_connections(dv->gv, n_locations, dv->player_path[player]->last->location, player, dv_get_round(dv), road, rail, sea);
	}
}


/////////////////////////////////////////////////////////helper functions/////////////////////////////////////////////////////////////////////

static location_t abbrevToLocation(char *abbrev)						// change abrev string to location id
{
	if (strcmp(abbrev, "C?") == 0)
		return CITY_UNKNOWN;
	else if (strcmp(abbrev, "S?") == 0)
		return SEA_UNKNOWN;
	else if (strcmp(abbrev, "HI") == 0)
		return HIDE;
	else if (abbrev[0] == 'D')
	{
		switch (abbrev[1])
		{
		case '1':
			return DOUBLE_BACK_1;
		case '2':
			return DOUBLE_BACK_2;
		case '3':
			return DOUBLE_BACK_3;
		case '4':
			return DOUBLE_BACK_4;
		case '5':
			return DOUBLE_BACK_5;
		default:
			return location_find_by_abbrev(abbrev);
		}
	}
	else
		return location_find_by_abbrev(abbrev);
}

static void hunter_update_location(dracula_view *dv, enum player player, location_t new_location)
{
	DLListInsert(dv->player_path[player], new_location);
}

static void apply_hunter_rule(dracula_view *dv, char *token, location_t this_location)
{
	for (size_t i = 3; i < 7; i++)
	{
		switch (token[i])
		{
		case 'T':
			// Trap encountered
			for (size_t j = 0; j < MAX_TRAPS_NUMBER; j++)
				if (dv->traps[j] == this_location)
					dv->traps[j] = -1;
			break;
		case 'V':
			// Immature Vampire was encountered
			dv->is_vampire_alive = 0;
			break;
		case 'D':
			// Dracula was confronted
			break;
		}
	}
}

static void dracula_update_location(dracula_view *dv, location_t new_location)
{
	location_t old_location = new_location;
	if (new_location >= DOUBLE_BACK_1 && new_location <= DOUBLE_BACK_5)
	{
		DLListNode *curr = dv->dracula_real_path->last;
		for (int i = 0; i < (new_location - DOUBLE_BACK_1); i++)
		{
			if (i + 1 < (int)dv->dracula_real_path->nitems)
				curr = curr->prev;
		}
		new_location = curr->location;
	}
	else if (new_location == HIDE)                                       //if the current location is HIDE 
	{
		new_location = dv->dracula_real_path->last->location;			 //this two if is actually not neccessary,becasue TP HI are replaced
	}																	 
	else if (new_location == TELEPORT)									 // if the current location TP
	{
		new_location = CASTLE_DRACULA;
	}

	DLListInsert(dv->player_path[PLAYER_DRACULA], old_location);
	DLListInsert(dv->dracula_real_path, new_location);
}

static void apply_dracula_rule(dracula_view *dv, char *token, location_t this_location)
{
	for (int i = 3; i <= 4; i++)
	{
		switch (token[i])
		{
		case 'T':
			// Trap was placed
			dv->traps[0] = this_location;
			break;
		case 'V':
			// Immature Vampire was placed
			dv->vampire_location = this_location;
			dv->is_vampire_alive = 1;
			break;
		}
	}
	switch (token[5])
	{
	case 'M':
		// Trap left trail
		break;
	case 'V':
		// Vampire has matured

		dv->is_vampire_alive = 0;
		break;
	}
	trap_update(dv);
}

static void trap_update(dracula_view *dv)
{
	for (int i = MAX_TRAPS_NUMBER - 1; i > 0; i--)
	{
		dv->traps[i] = dv->traps[i - 1];
	}
	dv->traps[0] = UNKNOWN_LOCATION;
}
////////////////////////////////linked_list interface///////////////////////////////
static DLListNode *newDLListNode(location_t location)
{
	DLListNode *new;
	new = malloc(sizeof(DLListNode));
	assert(new != NULL);
	new->location = location;
	new->prev = new->next = NULL;
	return new;
}

// create a new empty DLList
static DLList newDLList()
{
	struct DLListRep *new;
	new = malloc(sizeof(struct DLListRep));
	assert(new != NULL);
	new->nitems = 0;
	new->curr = NULL;
	new->first = NULL;
	new->last = NULL;
	return new;
}

// free up all space associated with list
static void freeDLList(DLList L)
{
	assert(L != NULL);
	DLListNode *curr, *prev;
	curr = L->first;
	while (curr != NULL)
	{
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(L);
}

static void DLListInsert(DLList L, location_t location)
{
	assert(L != NULL);
	DLListNode *ins = newDLListNode(location);
	if (L->nitems == 0)
	{
		L->first = ins;
	}
	else
	{
		L->last->next = ins;
		ins->prev = L->last;
	}
	L->curr = L->first;
	L->last = ins;
	L->nitems++;
}

/*static void DLListInsert_nodup(DLList L, location_t location)
{
	assert(L != NULL);
	for (DLListNode *curr = L->first; curr != NULL; curr = curr->next)
	{
		if (curr->location == location)
			return;
	}
	DLListInsert(L, location);
}*/
///////////////////////////////////////////////////////////////////////////////////////
static void original_string_to_real(char *string)							//turns TB,HI,CD to their real loaction
{
	size_t i = 73;
	while (i < strlen(string))
	{
		if (string[i] == 'H' && string[i + 1] == 'I')
		{
			string[i] = string[i - 40];
			string[i + 1] = string[i + 1 - 40];
		}
		else if (string[i] == 'D' && string[i + 1] == '1')
		{
			string[i] = string[i - 40];
			string[i + 1] = string[i + 1 - 40];
		}
		else if (string[i] == 'D' && string[i + 1] == '2')
		{
			string[i] = string[i - 80];
			string[i + 1] = string[i + 1 - 80];
		}
		else if (string[i] == 'D' && string[i + 1] == '3')
		{
			string[i] = string[i - 120];
			string[i + 1] = string[i + 1 - 120];
		}
		else if (string[i] == 'D' && string[i + 1] == '4')
		{
			string[i] = string[i - 160];
			string[i + 1] = string[i + 1 - 160];
		}
		else if (string[i] == 'D' && string[i + 1] == '5')
		{
			string[i] = string[i - 200];
			string[i + 1] = string[i + 1 - 200];
		}
		else if (string[i] == 'T' && string[i + 1] == 'P')
		{
			string[i] = 'C';
			string[i + 1] = 'D';
		}
		i = i + 40;
	}
}

static location_t *location_dracula_can_move(
	dracula_view *dv, size_t *n_locations,
	location_t from, bool road, bool sea)
{
	DLList connection_list = newDLList();

	int visited[NUM_MAP_LOCATIONS];

	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		visited[i] = -1;
	visited[from] = 1;
																					// dracula can't go to ST_JOSEPH_AND_ST_MARYS
	visited[ST_JOSEPH_AND_ST_MARYS] = 1;
																					// can't go a location in his most recent 5 moves
	location_t trail[TRAIL_SIZE];
	gv_get_history(dv->gv, PLAYER_DRACULA, trail);
	
	for (int counter_a = 0;counter_a<TRAIL_SIZE-1;counter_a++){
		if (valid_location_p (trail[counter_a]) == 1 ){
			visited[trail[counter_a]] = 1;
		}
	}

	for (map_adj *curr_1 = dv->world_map->connections[from]; curr_1 != NULL; curr_1 = curr_1->next)
	{
		if ((road && curr_1->type == ROAD && visited[curr_1->v] == -1) || (sea && curr_1->type == BOAT && visited[curr_1->v] == -1))
		{
			visited[curr_1->v] = 1;
			DLListInsert(connection_list, curr_1->v);
		}
	}
	int flag1 = 0;
	int flag2 = 0;
	for (int counter = 0;counter<TRAIL_SIZE-1;counter++){
		if (DOUBLE_BACK_1<=trail[counter] && trail[counter]<=DOUBLE_BACK_5 ){
			flag1=1;
		}
		if (trail[counter]==HIDE){

			flag2=1;
		}
		
	}
	if (flag1==0){
		location_t real_path_trail[TRAIL_SIZE];
		dv_get_trail(dv,PLAYER_DRACULA,real_path_trail);
		int visited_for_doubleback[NUM_MAP_LOCATIONS];
		//for (int counter_3=0; counter_3< TRAIL_SIZE;counter_3++){
		//	printf("stuff %s\n ",location_get_name(real_path_trail[counter_3]));
		//}
		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
			visited_for_doubleback[i] = -1;
		visited_for_doubleback[from] = 1;	
		for (int counter_3=0; counter_3< TRAIL_SIZE-1;counter_3++){
			printf("locations that choice from double_back_to %s\n ",location_get_name(real_path_trail[counter_3]));
		}
		for (int counter_1=0; counter_1< TRAIL_SIZE-1;counter_1++){
			if (real_path_trail[counter_1]==from){
				DLListInsert(connection_list, counter_1+DOUBLE_BACK_1);
			}else {
				int flag3 = 0;
				for (map_adj *curr_2 = dv->world_map->connections[from]; curr_2 != NULL; curr_2 = curr_2->next){
					if(real_path_trail[counter_1]==curr_2->v && ((road && curr_2->type == ROAD )|| (sea && curr_2->type == BOAT))){
						flag3=1;
					}
				}
				if (flag3==1){
					DLListInsert(connection_list, counter_1+DOUBLE_BACK_1);
				}
			}
		}			

	}	
	if (flag2==0 && sea_p (from)== 0){
		DLListInsert(connection_list, HIDE);
	}

	*n_locations = connection_list->nitems;
	if (*n_locations == 0){
		DLListInsert(connection_list, TELEPORT);
		(*n_locations)++;
	}

	location_t *result = malloc(*n_locations * sizeof(location_t));
	DLListNode *curr_connection_assignment = connection_list->first;
	for (size_t i = 0; i < *n_locations; i++)
	{
		result[i] = curr_connection_assignment->location;
		curr_connection_assignment = curr_connection_assignment->next;
	}
	for (int j=0;j<*n_locations;j++){
		printf("My_legal_move %s\n",location_get_abbrev(result[j]));

	}
	freeDLList(connection_list);      
	return result;
}

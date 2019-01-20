////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
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

#include "game.h"
#include "game_view.h"
#include "map.h" 

typedef struct game_view {
	//Map map;
	round_t curr_turn;
	int curr_score;
	int player_health[NUM_PLAYERS];
	int player_moves[NUM_PLAYERS][TRAIL_SIZE];
	//location_t trap[TRAIL_SIZE];
	location_t immature_vampire;
} game_view;

static enum player player_string_to_id (int count, char *past_plays);
static location_t abbrev_string_to_location (int count, char *past_plays);
static void fill_player_moves_array (game_view *gv, enum player player, location_t location);
static void when_hunter_encounter (game_view *gv, char *past_plays, enum player player, int count);
static void when_dracula_action (game_view *gv, char *past_plays, int count, location_t location);
//static void fill_trap_array(game_view *gv, enum player player, location_t location);
//static void remove_when_trap_encounter(game_view *gv, enum player player);

game_view *gv_new (char *past_plays, player_message messages[])
{
	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
    
    //new->map = map_new ();
    
    //initial score and turn number;
    new->curr_turn = 0;
	new->curr_score = GAME_START_SCORE;

	//initial player's health;
	new->player_health[PLAYER_LORD_GODALMING] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_DR_SEWARD] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_VAN_HELSING] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_MINA_HARKER] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
	
	//initial player's location(UNKNOWN_LOCATION);
	int n = 0;
	while (n < NUM_PLAYERS) {
	  	int m = 0;
	  	while (m < TRAIL_SIZE) {
	  		new->player_moves[n][m] = UNKNOWN_LOCATION;
	  		m++;
	  	}
	    n++;
	}

	//update game_view by given past_player;
	int count = 0;
	int length = strlen (past_plays);
	
	while (count < length) {

	    enum player curr_player = player_string_to_id(count, past_plays);  // multiple character_problem;

	    //if current player is Dracula;
	    if (curr_player == PLAYER_DRACULA) {

	    	//update curr location (move);
	    	location_t curr_loc_dracula = abbrev_string_to_location (count, past_plays);   // cant get abbrev of nowhere location
	    	//printf("cuee_loc_dracula = %s\n", curr_loc_dracula);
	    	fill_player_moves_array (new, curr_player, curr_loc_dracula); 

	    	if (curr_loc_dracula == SEA_UNKNOWN) {
	    		new->player_health[PLAYER_DRACULA] == new->player_health[PLAYER_DRACULA] - LIFE_LOSS_SEA;

	    	// no idea how to do this double back when is sea;
	    	} else if (curr_loc_dracula == DOUBLE_BACK_1) {   // also need to consider the cd case, blood gain decrease order

	    	}
	    	//update curr action;
	    	when_dracula_action (new, past_plays, count, curr_loc_dracula);

	    	//every dracula finish a turn score - 1;
	    	new->curr_score --;

	    //if current player is one of hunters;
	    } else {

	    	//update curr location (move);
	    	location_t curr_loc_hunter = abbrev_string_to_location (count, past_plays);
	    	fill_player_moves_array (new, curr_player, curr_loc_hunter);

	    	//update curr action;
	    	when_hunter_encounter (new, past_plays, curr_player, count);

	    }
	    
	    count = count + 8;
	    new->curr_turn ++;
	}
	
	return new;
}

void gv_drop (game_view *gv)
{

	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	round_t round = gv->curr_turn / 5;
	return round;
}

enum player gv_get_player (game_view *gv)
{
	enum player p = gv->curr_turn % 5;
	
	return p;
}

int gv_get_score (game_view *gv)
{
	return gv->curr_score;
}

int gv_get_health (game_view *gv, enum player player)
{
	
	return gv->player_health[player];
}

location_t gv_get_location (game_view *gv, enum player player)
{
	
	return gv->player_moves[player][0];
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{	
	int n = 0;
	while (n < TRAIL_SIZE) {
		trail[n] = gv->player_moves[player][n];
		n++;
	}
	return;
}

location_t *gv_get_connections (
	game_view *gv, size_t *n_locations,
	location_t from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	
	*n_locations = 0;
	return NULL;
}


// /////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------static function--------------------------------------//										  

// /////////////////////////////////////////////////////////////////////////////////////////

static enum player player_string_to_id (int count, char *past_plays) 
{
	enum player player;

	if (past_plays[count] == 'G') player = PLAYER_LORD_GODALMING;
	else if (past_plays[count] == 'S') player = PLAYER_DR_SEWARD;
	else if (past_plays[count] == 'H')	player = PLAYER_VAN_HELSING;
	else if (past_plays[count] == 'M')	player = PLAYER_MINA_HARKER;
	else if (past_plays[count] == 'D') player = PLAYER_DRACULA;
	
    else return 0;
    
	return player;
}

static location_t abbrev_string_to_location (int count, char *past_plays) 
{
	char abbrev[3];

	abbrev[0] = past_plays[count + 1];
	abbrev[1] = past_plays[count + 2];
	abbrev[2] = '\0';

	location_t loc = location_find_by_abbrev(abbrev);

	return loc;
}

//update the player_move;
static void fill_player_moves_array (game_view *gv, enum player player, location_t location) 
{
	int i = 1;

	while (i < TRAIL_SIZE) {

		gv->player_moves[player][i] = gv->player_moves[player][i-1];
		i++;
	}

	gv->player_moves[player][0] = location;
}

static void when_hunter_encounter (game_view *gv, char *past_plays, enum player player, int count) 
{
	int n = count + 3;
	while (n < TRAIL_SIZE) {

		if (past_plays[n] == 'T') {
			gv->player_health[player] -= LIFE_LOSS_TRAP_ENCOUNTER;
			//remove_when_trap_encounter (gv, player);
		}

		if (past_plays[n] == 'V') {
			gv->immature_vampire = NOWHERE;
		}

		if (past_plays[n] == 'D') {
			gv->player_health[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
			gv->player_health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
		}

		if (gv->player_health[player] <= 0) {
			gv->curr_score = gv->curr_score - 6;
		}

		n++;
	}
}

static void when_dracula_action (game_view *gv, char *past_plays, int count, location_t location) // this is really a good function 
{
	int n = count + 3;

	if (past_plays[n] == 'T') {
		//fill_trap_array (gv, player, location);
	} 

	if (past_plays[n + 1] =='V' ) {
		gv->immature_vampire = location;
	}

	if (past_plays[n + 2] == 'M') {

	}

	if(past_plays[n + 3] == 'V') {
		gv->curr_score = gv->curr_score - SCORE_LOSS_VAMPIRE_MATURES;
	}


}
/*
static void fill_trap_array(game_view *gv, enum player player, location_t location) 
{
	int i = 1;

	while (i < TRAIL_SIZE) {

		gv->trap[i] = gv->trap[i-1];
		i++;
	}

	gv->trap[0] = location;

}

static void remove_when_trap_encounter(game_view *gv, enum player player) 
{

}
*/

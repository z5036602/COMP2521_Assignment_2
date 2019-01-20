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
#include <stdio.h>
#include <sysexits.h>
#include <string.h>

#include "game.h"
#include "game_view.h"
#include "map.h"

typedef struct game_view {
	round_t curr_turn;
	int curr_score;
	int player_health[NUM_PLAYERS];
	int player_moves[NUM_PLAYERS][TRAIL_SIZE];
	int player_curr_location[NUM_PLAYERS];
	int Dracula_real_curr_location;
	char *real_past_plays;
	bool is_vampire_alive;
} game_view;


static location_t abbrev_string_to_location (char a, char b);
static void rotating_array_insert_size_6 (int *array,int a,int size);
static void record_dracula_location (game_view *gv,char * past_play_array,int position);
static void record_hunter_location (game_view *gv,char * past_play_array,int position,int id);
static void damage_counting_at_hunter_turn(game_view *gv,char * past_play_array,int position,int id );
static void damage_counting_at_dracula_turn(game_view *gv,char * past_play_array,int position);
static void original_string_to_real (char* string);
static void revive_the_hunter_after_death(game_view *gv,int id);

game_view *gv_new (char *past_plays, player_message messages[])
{
	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
    new->curr_turn = 0;
	new->curr_score = GAME_START_SCORE;
	new->player_health[PLAYER_LORD_GODALMING] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_DR_SEWARD] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_VAN_HELSING] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_MINA_HARKER] = GAME_START_HUNTER_LIFE_POINTS;
	new->player_health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
	new->real_past_plays = NULL;
	new->is_vampire_alive = false;		
	for (int n = 0;n < NUM_PLAYERS;n++) {
	  	for (int m = 0;m < TRAIL_SIZE;m++) {
	  		new->player_moves[n][m] = UNKNOWN_LOCATION;
	  	}
		new->player_curr_location[n] = UNKNOWN_LOCATION;
	
	}
	if(strlen(past_plays) == 0) return new;

	new->curr_turn = (strlen(past_plays)+1)/8;
	new->curr_score = GAME_START_SCORE;
	new->is_vampire_alive = false;
	new->real_past_plays = strdup(past_plays);
	original_string_to_real(new->real_past_plays);
	enum player real_player = 0;
	for(size_t k=0; k < strlen(new->real_past_plays);k+=8){
		switch(past_plays[k]) {
  		case 'G' :
		real_player = PLAYER_LORD_GODALMING; break;
 		case 'S' :
		real_player = PLAYER_DR_SEWARD; break;
		case 'H' :
		real_player = PLAYER_VAN_HELSING;break;
		case 'M':
		real_player = PLAYER_MINA_HARKER;break;
   		default :
		real_player = PLAYER_DRACULA;break;
   		}
		if (real_player!=PLAYER_DRACULA){
			record_hunter_location (new,past_plays,k,real_player);		    
	    	damage_counting_at_hunter_turn(new,past_plays,k,real_player);	        
			if (new->player_health[real_player] <= 0) {
				new->player_curr_location[real_player] = ST_JOSEPH_AND_ST_MARYS;
			} else {
				new->player_curr_location[real_player] = new->player_moves[real_player][0];
			}        
		}else{

			if(past_plays[k + 5] == 'V') {
				new->curr_score = new->curr_score - SCORE_LOSS_VAMPIRE_MATURES;
			}
			record_dracula_location (new,past_plays,k);
			location_t id = abbrev_string_to_location(new->real_past_plays[k+1],new->real_past_plays[k+2]);
			if(valid_location_p(id)==true){
				new->Dracula_real_curr_location = id;
			}else{
				new->Dracula_real_curr_location=UNKNOWN;
			}
			new->player_curr_location[PLAYER_DRACULA] = new->player_moves[PLAYER_DRACULA][0];
				damage_counting_at_dracula_turn(new,new->real_past_plays,k);
			new->curr_score--;
			
		}

	}

		return new;
	

}


void gv_drop (game_view *gv)
{
	free (gv->real_past_plays);
	free (gv);
}

round_t gv_get_round (game_view *gv)
{
	assert(gv!=NULL);
	round_t round = gv->curr_turn / 5;
	return round;
}

enum player gv_get_player (game_view *gv)
{
	assert(gv!=NULL);
	int num_player = gv->curr_turn % 5;
	return num_player;
}

int gv_get_score (game_view *gv)
{
	assert(gv!=NULL);
	return gv->curr_score;
}

int gv_get_health (game_view *gv, enum player player)
{
	assert(gv!=NULL);
	return gv->player_health[player];
}

location_t gv_get_location (game_view *gv, enum player player)
{
	assert(gv!=NULL);
	return gv->player_curr_location[player];
}

void gv_get_history (
	game_view *gv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	assert(gv!=NULL);
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
	assert(gv!=NULL);
	location_t* edges = NULL;
	Map my_map = map_new ();
    
	if (  player != PLAYER_DRACULA){
		if((round + player) %4 ==0){
			edges = check_for_connections_0_round(my_map, from, road, sea,n_locations);
		}
		if((round + player) %4 ==1){
			edges = check_for_connections_1_round(my_map, from, road, rail, sea,n_locations);
		}
		if((round + player) %4 ==2){
			edges = check_for_connections_2_round(my_map, from, road, rail, sea,n_locations);
		}
		if((round + player)%4 ==3){
			edges = check_for_connections_3_round(my_map, from, road, rail, sea,n_locations);
		}
	}else{		
		if(valid_location_p(from)==true){
			edges = check_for_connections_dracula(my_map, from, road, sea,n_locations);
		}else{

			edges = check_for_connections_dracula(my_map, gv->Dracula_real_curr_location, road, sea,n_locations);
		}
	}
	map_drop(my_map);
	return edges;

}



static void original_string_to_real (char* string){
	int i=73;
	while(i < strlen(string)) {
		if (string[i] == 'H' && string[i+1]=='I'){
			string[i] = string[i-40];
			string[i+1] = string[i+1-40];
		}
		else if (string[i] == 'D' && string[i+1]=='1'){
			string[i] = string[i-40];
			string[i+1] = string[i+1-40];
		}
		else if (string[i] == 'D' && string[i+1]=='2'){
			string[i] = string[i-80];
			string[i+1] = string[i+1-80];
		}
		else if (string[i] == 'D' && string[i+1]=='3'){
			string[i] = string[i-120];
			string[i+1] = string[i+1-120];
		}
		else if (string[i] == 'D' && string[i+1]=='4'){
			string[i] = string[i-160];
			string[i+1] = string[i+1-160];
		}
		else if (string[i] == 'D' && string[i+1]=='5'){
			string[i] = string[i-200];
			string[i+1] = string[i+1-200];
			}
		else if (string[i] == 'T' && string[i+1]=='P'){
			string[i] = 'C';
			string[i+1] = 'D';
		}
		i=i+40;
		
	}

}











static location_t abbrev_string_to_location (char a, char b) {
	char abbrev[3];

	abbrev[0] = a;
	abbrev[1] = b;
	abbrev[2] = '\0';

	return location_find_by_abbrev(abbrev);
}
static void rotating_array_insert_size_6 (int *array,int a,int size){
  	for(int i=size-1 ; i>=1 ; i--){
    	array[i]=array[i-1] ;
	}
  	array[0]=a;
}
static void record_hunter_location (game_view *gv,char * past_play_array,int position,int id ){
	int location_id = abbrev_string_to_location (past_play_array[position+1], past_play_array[position+2]);
	rotating_array_insert_size_6 (gv->player_moves[id],location_id,TRAIL_SIZE);
}
static void record_dracula_location (game_view *gv,char * past_play_array,int position){
	if (past_play_array[position+1] == 'C' && past_play_array[position+2] == '?') {
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],CITY_UNKNOWN,6);
	}
	else if (past_play_array[position+1] == 'S' && past_play_array[position+2] == '?'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],SEA_UNKNOWN,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '1'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_1,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '2'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_2,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '3'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_3,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '4'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_4,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '5'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_5,6);
	}
	else if (past_play_array[position+1] == 'H' && past_play_array[position+2] == 'I'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],HIDE,6);
	}
	else if (past_play_array[position+1] == 'T' && past_play_array[position+2] == 'P'){
		rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],TELEPORT,6);
	}
	else{
		int location_id = abbrev_string_to_location (past_play_array[position+1], past_play_array[position+2]);
    	rotating_array_insert_size_6 (gv->player_moves[PLAYER_DRACULA],location_id,6);
	}
}
static void damage_counting_at_hunter_turn(game_view *gv,char * past_play_array,int position,int id ){
	int flag = 0;
	revive_the_hunter_after_death(gv,id);
	for (int i = 0;i<4;i++){
		switch(past_play_array[position+3+i]) {
      		case 'T' :
			gv->player_health[id] = (gv->player_health[id]-LIFE_LOSS_TRAP_ENCOUNTER<=0)? 0:gv->player_health[id]-LIFE_LOSS_TRAP_ENCOUNTER; break;
     		case 'V' :
 			break;
			case 'D' :
			flag = 1;
			gv->player_health[id] =(gv->player_health[id]-LIFE_LOSS_DRACULA_ENCOUNTER<=0)? 0:gv->player_health[id]-LIFE_LOSS_DRACULA_ENCOUNTER;break;
       		default :
			break;
   		}
	}
	if (position>=40 && gv->player_health[id] > 0){     
		assert(past_play_array[position-40] == past_play_array[position]);
		if (past_play_array[position-40+1] == past_play_array[position+1]
			&& past_play_array[position-40+2] == past_play_array[position+2]){
			gv->player_health[id] = (gv->player_health[id]+LIFE_GAIN_REST>=9)? 9:gv->player_health[id]+LIFE_GAIN_REST;

		}
	}
	if (flag == 1){
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_HUNTER_ENCOUNTER	;
		
	}
	if (gv->player_health[id]<=0){		
		gv->curr_score=gv->curr_score-6;
	}
}
static void damage_counting_at_dracula_turn(game_view *gv,char * past_play_array,int position){
	if (past_play_array[position+1] == 'S' && past_play_array[position+2] == '?') {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if (past_play_array[position+1] == 'C' && past_play_array[position+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0){
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	location_t id = abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]);
	if(valid_location_p(id)==true){
		if (sea_p(abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]))) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
		}
	}
}

static void damage_counting_at_dracula_view_string(game_view *gv,char * past_play_array,int position){
	location_t id = abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]);
	if(valid_location_p(id)==true){
		if (sea_p(abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]))) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
		}
	}
	
}
static void revive_the_hunter_after_death(game_view *gv,int id){
	if (gv->player_health[id]<=0){			
		gv->player_health[id]=9;
	}
}


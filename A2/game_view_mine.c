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
	//Map map;
	round_t curr_turn;
	bool is_dracula_view;
	int curr_score;
	int player_health[NUM_PLAYERS];
	int player_moves[NUM_PLAYERS][TRAIL_SIZE];
} game_view;
//static player player_string_to_id (int count, char *past_player);
static location_t abbrev_string_to_location (char a, char b);
static void rotating_sarray (int *array,int a,int size);
static void record_location (game_view *gv,char * past_play_array,int position,int id);
static void damage_counting_at_hunter_turn(game_view *gv,char * past_play_array,int position,int id );
static void damage_counting_at_dracula_turn(game_view *gv,char * past_play_array,int position);
static void record_dracula_location (game_view *gv,char * past_play_array,int position);
static void damage_counting_at_dracula_view_string(game_view *gv,char * past_play_array,int position);
game_view *gv_new (char *past_plays, player_message messages[])
{
	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");
    
    //new->map = map_new ();
    
    //initial score and turn number;
	if(strlen(past_plays) == 0){
    	new->curr_turn = 0;
		new->curr_score = GAME_START_SCORE;
		new->is_dracula_view = false;
	//initial player's health;
		new->player_health[PLAYER_LORD_GODALMING] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_DR_SEWARD] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_VAN_HELSING] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_MINA_HARKER] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
		int n=0;
		while (n < NUM_PLAYERS) {
	  		int m = 0;
	  		while (m < TRAIL_SIZE) {
	  			new->player_moves[n][m] = UNKNOWN_LOCATION;
	  			m++;
	  		}
	    	n++;
		}
		return new;
	}else{
		new->player_health[PLAYER_LORD_GODALMING] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_DR_SEWARD] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_VAN_HELSING] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_MINA_HARKER] = GAME_START_HUNTER_LIFE_POINTS;
		new->player_health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
		new->is_dracula_view = false;
		new->curr_turn = (strlen(past_plays)+1)/8;
		new->curr_score = GAME_START_SCORE;
		int n=0;
		while (n < NUM_PLAYERS) {
	  		int m = 0;
	  		while (m < TRAIL_SIZE) {
	  			new->player_moves[n][m] = UNKNOWN_LOCATION;
	  			m++;
	  		}
	    	n++;
		}
		int k=0;
		location_t location_id_for_distinguish_dracula_view = -1;
		location_id_for_distinguish_dracula_view = abbrev_string_to_location (past_plays[33], past_plays[34]);
		
		if(valid_location_p(location_id_for_distinguish_dracula_view)){
			new->is_dracula_view = true;
		}
		//printf("true or not: %d\n",new->is_dracula_view);
		while(k<strlen(past_plays)){
			if (k==0|| (past_plays[k] == 'G' && past_plays[k-1] == ' ')){
				record_location (new,past_plays,k,PLAYER_LORD_GODALMING);
				damage_counting_at_hunter_turn(new,past_plays,k,PLAYER_LORD_GODALMING);
				k++;
			}else if (past_plays[k] == 'S' && past_plays[k-1] == ' '){
				record_location (new,past_plays,k,PLAYER_DR_SEWARD);	
				damage_counting_at_hunter_turn(new,past_plays,k,PLAYER_DR_SEWARD);		
				k++;
			}else if (past_plays[k] == 'H' && past_plays[k-1] == ' '){
				record_location (new,past_plays,k,PLAYER_VAN_HELSING);
				damage_counting_at_hunter_turn(new,past_plays,k,PLAYER_VAN_HELSING);
				k++;
			}else if (past_plays[k] == 'M' && past_plays[k-1] == ' '){
				record_location (new,past_plays,k,PLAYER_MINA_HARKER);
				damage_counting_at_hunter_turn(new,past_plays,k,PLAYER_LORD_GODALMING);
				k++;
			}else if (past_plays[k] == 'D' && past_plays[k-1] == ' '){
				record_dracula_location (new,past_plays,k);
				if (new->is_dracula_view ){
					damage_counting_at_dracula_view_string(new,past_plays,k);
				}else{
					damage_counting_at_dracula_turn(new,past_plays,k);
				}
				new->curr_score--;
				k++;
			}else {
				
				k++;
			}

		}

		return new;
	}

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
	int num_player = gv->curr_turn % 5;
	return num_player;
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
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}



static location_t abbrev_string_to_location (char a, char b) {
	char abbrev[3];

	abbrev[0] = a;
	abbrev[1] = b;
	abbrev[2] = '\0';

	return location_find_by_abbrev(abbrev);
}
static void rotating_sarray (int *array,int a,int size){
  	for(int i=size-1 ; i>=1 ; i--){
    	array[i]=array[i-1] ;
	}
  	array[0]=a;
}
static void record_location (game_view *gv,char * past_play_array,int position,int id ){
	int location_id = abbrev_string_to_location (past_play_array[position+1], past_play_array[position+2]);	
	rotating_sarray (gv->player_moves[id],location_id,TRAIL_SIZE);
}
static void record_dracula_location (game_view *gv,char * past_play_array,int position){
	if (past_play_array[position+1] == 'C' && past_play_array[position+2] == '?') {
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],CITY_UNKNOWN,6);			
	}
	else if (past_play_array[position+1] == 'S' && past_play_array[position+2] == '?'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],SEA_UNKNOWN,6);		
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '1'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_1,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '2'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_2,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '3'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_3,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '4'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_4,6);
	}
	else if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '5'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],DOUBLE_BACK_5,6);
	}
	else if (past_play_array[position+1] == 'H' && past_play_array[position+2] == 'I'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],HIDE,6);
	}
	else if (past_play_array[position+1] == 'T' && past_play_array[position+2] == 'P'){
		rotating_sarray (gv->player_moves[PLAYER_DRACULA],TELEPORT,6);
	}
	else{
		int location_id = abbrev_string_to_location (past_play_array[position+1], past_play_array[position+2]);
    	rotating_sarray (gv->player_moves[PLAYER_DRACULA],location_id,6);
	}
}
static void damage_counting_at_hunter_turn(game_view *gv,char * past_play_array,int position,int id ){
	int flag = 0;
	if (gv->player_health[id]<=0){			//hospital blood is zero;
		gv->player_health[id]=9;
	}
	for (int i = 0;i<4;i++){
		switch(past_play_array[position+3+i]) {
      		case 'T' :
			gv->player_health[id] = gv->player_health[id]-LIFE_LOSS_TRAP_ENCOUNTER; break;
     		case 'V' :
 			break;
			case 'D' :
			flag = 1;
			gv->player_health[id] = gv->player_health[id]-LIFE_LOSS_DRACULA_ENCOUNTER; break;
       		default :
			break;
   		}
	}
	if (position>=40 && gv->player_health[id] > 0){     // Does string provide the defeat cases say MLOTTTD, apparently he cant meet D ,since all blood out??
		assert(past_play_array[position-40]==past_play_array[position]);
		if (past_play_array[position-40+1]==past_play_array[position+1]
			&& past_play_array[position-40+2]==past_play_array[position+2]){
			gv->player_health[id]=(gv->player_health[id]+LIFE_GAIN_REST>=9)? 9:gv->player_health[id]+LIFE_GAIN_REST;
			
		}
	}
	if (flag == 1){
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_HUNTER_ENCOUNTER	;
	}	
	if (gv->player_health[id]<=0){			//hospital blood is zero;
		gv->curr_score=gv->curr_score-6;
	}
}
static void damage_counting_at_dracula_turn(game_view *gv,char * past_play_array,int position){
	if (past_play_array[position+1] == 'S' && past_play_array[position+2] == '?') {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if ((past_play_array[position+1] == 'D' && past_play_array[position+2] == '1'
	&& past_play_array[position-40+1] == 'S' && past_play_array[position-40+2] == '?')) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if ((past_play_array[position+1] == 'D' && past_play_array[position+2] == '2'
	&& past_play_array[position-80+1] == 'S' && past_play_array[position-80+2] == '?')) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if ((past_play_array[position+1] == 'D' && past_play_array[position+2] == '3'
	&& past_play_array[position-120+1] == 'S' && past_play_array[position-120+2] == '?')) {
		assert(past_play_array[position-120]==past_play_array[position]);
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if ((past_play_array[position+1] == 'D' && past_play_array[position+2] == '4'
	&& past_play_array[position-160+1] == 'S' && past_play_array[position-160+2] == '?')) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if ((past_play_array[position+1] == 'D' && past_play_array[position+2] == '5'
	&& past_play_array[position-200+1] == 'S' && past_play_array[position-200+2] == '?')) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
	}
	if (past_play_array[position+1] == 'C' && past_play_array[position+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0){
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '1'
	&& past_play_array[position-40+1] == 'C' && past_play_array[position-40+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '2'
	&& past_play_array[position-80+1] == 'C' && past_play_array[position-80+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '3'
	&& past_play_array[position-120+1] == 'C' && past_play_array[position-120+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
		assert(past_play_array[position-120]==past_play_array[position]);
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '4'
	&& past_play_array[position-160+1] == 'C' && past_play_array[position-160+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '5'
	&& past_play_array[position-200+1] == 'C' && past_play_array[position-200+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'T' && past_play_array[position+2] == 'P'&& gv->player_health[PLAYER_DRACULA]> 0) {
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}
	if (past_play_array[position+1] == 'H' && past_play_array[position+2] == 'I' && past_play_array[position-40+1]=='C'
			&& past_play_array[position-40+2]=='D' && gv->player_health[PLAYER_DRACULA]> 0){
		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
	}

}
static void damage_counting_at_dracula_view_string(game_view *gv,char * past_play_array,int position){
	location_t id = abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]);
	if (id!=NOWHERE){
		if (sea_p(abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]))) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
		}
	}else{
		if (past_play_array[position+2] == '1' && sea_p(abbrev_string_to_location(past_play_array[position-40+1],past_play_array[position-40+2]))) 			{
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
		}
		else if (past_play_array[position+2] == '2' && sea_p(abbrev_string_to_location(past_play_array[position-80+1],past_play_array[position-80+2]))) 			{
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
		}
		else if (past_play_array[position+2] == '3' && sea_p(abbrev_string_to_location(past_play_array[position-120+1],past_play_array[position-120+2]))) 			{
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
		}
		else if (past_play_array[position+2] == '4' && sea_p(abbrev_string_to_location(past_play_array[position-160+1],past_play_array[position-160+2]))) 			{
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
		}
		else if (past_play_array[position+2] == '5' && sea_p(abbrev_string_to_location(past_play_array[position-200+1],past_play_array[position-200+2]))) 			{
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
		}
		
		if (past_play_array[position+1] == 'C' && past_play_array[position+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0){
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
		if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '1'
			&& past_play_array[position-40+1] == 'C' && past_play_array[position-40+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
		if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '2'
			&& past_play_array[position-80+1] == 'C' && past_play_array[position-80+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
		if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '3'
			&& past_play_array[position-120+1] == 'C' && past_play_array[position-120+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
			assert(past_play_array[position-120]==past_play_array[position]);
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
		if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '4'
			&& past_play_array[position-160+1] == 'C' && past_play_array[position-160+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
		if (past_play_array[position+1] == 'D' && past_play_array[position+2] == '5'
			&& past_play_array[position-200+1] == 'C' && past_play_array[position-200+2] == 'D'&& gv->player_health[PLAYER_DRACULA]> 0) {
			gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]+LIFE_GAIN_CASTLE_DRACULA;
		}
	}




}
	//assert(1==2);
//	if (sea_p(abbrev_string_to_location(past_play_array[position+1],past_play_array[position+2]))) {
//		gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;
//	}
	//if (past_play_array[position+2] == '1' && sea_p(abbrev_string_to_location(past_play_array[position-40+1],past_play_array[position-40+2]))) {
		//gv->player_health[PLAYER_DRACULA]=gv->player_health[PLAYER_DRACULA]-LIFE_LOSS_SEA;	
	//}

//assert(1==2);

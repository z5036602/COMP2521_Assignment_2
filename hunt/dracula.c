////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"
#include <time.h>
#include <stdio.h>
void decide_dracula_move (DraculaView dv)
{
	time_t t1;
	srand((unsigned) time(&t1));
	round_t curr_round = dv_get_round(dv);
	if (curr_round == 0){
		register_best_play ("CD", "haha");
		//register_best_play ("CD", "");
		//register_best_play ("CD", "");
	}else if (curr_round == 1){
		register_best_play ("GA", "hahaha");

	}else if (curr_round == 2){
		register_best_play ("KL", "");

	}else if (curr_round == 3){
		register_best_play ("D3", "");

	}else if (curr_round == 4){
		register_best_play ("HI", "TP");
	
	}
	else{
		size_t n_locations;
		location_t *array = dv_get_dests (dv, &n_locations, true, true);
		int number = n_locations;
		int index = 1+rand()%number;
		char *play = location_get_abbrev (array[index-1]);
		printf("index is :%d\n",index);
		printf("n_location is :%d\n",number);
		
		//char *new_play = (char*) play;
		/// @todo Replace this with something better!
		register_best_play (play, "");
	}
}	

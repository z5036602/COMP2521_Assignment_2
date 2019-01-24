////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include <stdbool.h>
#include <time.h>
void decide_hunter_move (HunterView hv)
{
	time_t t1;
	srand((unsigned) time(&t1));
	round_t curr_round = hv_get_round(hv);
	if (curr_round == 0){
		if(hv_get_player (hv) == PLAYER_LORD_GODALMING) {
			register_best_play ("BU", "");
		}else if (hv_get_player (hv) == PLAYER_DR_SEWARD){
			register_best_play ("GR", "");
		}else if (hv_get_player (hv) == PLAYER_VAN_HELSING){
			register_best_play ("PR", "");
		}else if (hv_get_player (hv) == PLAYER_MINA_HARKER){
			register_best_play ("SO", "");
		}
	}else{
		//int player = hv_get_player (hv);
		size_t n_locations;
		location_t *array = hv_get_dests(hv, &n_locations, true, true, true);
		int number = n_locations-1;
		int index = rand()%number;
		char *play = location_get_abbrev (array[index]);

		//char *new_play = (char*) play;
		/// @todo Replace this with something better!*/
		register_best_play (play, "");
	}
}

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
#include <stdio.h>


//int findPath(Graph g, Vertex src, Vertex dest, int max, int *path);
void decide_hunter_move (HunterView hv)
{	
	enum player curr_player = hv_get_player(hv); // get current player
	round_t curr_round = hv_get_round(hv);
	location_t dra_trail[TRAIL_SIZE];
	hv_get_trail(hv, PLAYER_DRACULA, dra_trail);
	location_t dra_last_seen = -1;

	if (curr_round == 0){
		if(curr_player == PLAYER_LORD_GODALMING) {
			register_best_play ("BU", "");
		}else if (curr_player == PLAYER_DR_SEWARD){
			register_best_play ("GR", "");
		}else if (curr_player == PLAYER_VAN_HELSING){
			register_best_play ("VI", "");
		}else if (curr_player == PLAYER_MINA_HARKER){
			register_best_play ("SO", "");
		}
		/*location_t Dra_location = hv_get_location (hv, PLAYER_DRACULA);	//get dracula_current_location
		if (valid_location_p (Dra_location)){
			hv_dra_last_seen_setter (hv,Dra_location);		
		}*/
	}else{
		//location_t Dra_location = hv_get_location (hv, PLAYER_DRACULA);	//get dracula_current_location
		//if (!valid_location_p (Dra_location)) {	
			//int found_flag=false;		
			for (int i =0; i<TRAIL_SIZE;i++){
				if (valid_location_p (dra_trail[i])){
					dra_last_seen = dra_trail[i];
				}
			}
			if (dra_last_seen == -1){														//if the location is UNKNOWN
				/*size_t n_locations;															//initialize number of locations
				location_t trail[TRAIL_SIZE];												//initalize Trail array
				location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true); 	//initialize destination array
				hv_get_trail (hv, curr_player,trail);
				int All_dest_has_been = true;
				for(int i = 0;i<n_locations;i++){
					int found_flag = false;
					for(int j = 0; j<TRAIL_SIZE;j++){
						if(H_dests[i] == trail[j]){
							found_flag = true;
						
						}
					}
					if (found_flag == false){
						register_best_play (location_get_abbrev(H_dests[i]), "");
						return;				
					}

				}*/
				register_best_play (location_get_abbrev(hv_get_location (hv, curr_player)), "");
				return;
			}else{
				location_t path[100];
				int number_of_hops = findPath_hunter(hv, hv_get_location(hv,curr_player), dra_last_seen, path);
				for (int i =0;i<number_of_hops;i++){
					printf("%s-> ",  location_get_abbrev(path[i]));
				}
				size_t n_locations;		
				location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true);
				//for(int i = 0;i<n_locations;i++){
					//for(int j = 0; j<number_of_hops;j++){
						//if(H_dests[i] == path[j]){
							if (hv_get_location(hv,curr_player) != dra_last_seen){
								register_best_play (location_get_abbrev(path[1]), "");
								return;
							}else{
								register_best_play (location_get_abbrev(path[0]), "");
								return;
							}
						//}	}
					//}
				//}

			}
		
		
		//}else{
			//printf("dracula_location_found is %s\n",location_get_abbrev(Dra_location));
			//hv_dra_last_seen_setter (hv,Dra_location);			
			//location_t path[100];
			//int number_of_hops = findPath_hunter(hv, hv_get_location(hv,curr_player), Dra_location, path);
			//for (int i =0;i<number_of_hops;i++){
			//	printf("%s-> ",  location_get_abbrev(path[i]));
			//}
			//size_t n_locations;		
			//location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true);
			//for(int i = 0;i<n_locations;i++){
			//	for(int j = 0; j<number_of_hops;j++){
			//		if(H_dests[i] == path[j]){
			//			register_best_play (location_get_abbrev(H_dests[i]), "");
			//			return;
			//		}
				//}
			//}
		
	}
}

/*time_t t1;
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
		//register_best_play (play, "");
	//}                                               random strategy


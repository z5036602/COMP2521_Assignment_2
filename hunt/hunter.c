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
#include <assert.h>
int distance (location_t src, location_t dest,HunterView hv);
location_t* select_dests_not_in_trail(location_t *dests,size_t *n_locations,int size,HunterView hv);
void searching_move(HunterView hv,location_t anker_point,int distance);
location_t * select_dests_less_than_distance(location_t *dests,size_t *n_locations,location_t anker_point,int distance_away,int size,HunterView hv);
location_t* find_common_locations (location_t *dests_1,location_t *dests_2,int size_1,int size_2,size_t *n_locations);
void chase(location_t dra_last_seen,HunterView hv);
//int findPath(Graph g, Vertex src, Vertex dest, int max, int *path);
void decide_hunter_move (HunterView hv)
{	
	enum player curr_player = hv_get_player(hv); // get current player
	round_t curr_round = hv_get_round(hv);
	location_t dra_trail[TRAIL_SIZE];
	hv_get_trail(hv, PLAYER_DRACULA, dra_trail);
	for (int j = 0; j<TRAIL_SIZE;j++){
		printf("%s->",location_get_abbrev(dra_trail[j]));	
	}
	location_t dra_last_seen = -1;

	if (curr_round == 0){
		if(curr_player == PLAYER_LORD_GODALMING) {
			register_best_play ("LO", "");
		}else if (curr_player == PLAYER_DR_SEWARD){
			register_best_play ("BE", "");
		}else if (curr_player == PLAYER_VAN_HELSING){
			register_best_play ("MA", "");
		}else if (curr_player == PLAYER_MINA_HARKER){
			register_best_play ("ZU", "");
		}
	}else{
		for (int i =0; i<TRAIL_SIZE;i++){
			if (valid_location_p (dra_trail[i])){
				dra_last_seen = dra_trail[i];
				break;
			}		
		}
		if (dra_last_seen == -1 ){
			//if(curr_round< 7){
				if(curr_player == PLAYER_LORD_GODALMING) {
					searching_move(hv,BRUSSELS,6);
					return;
				}else if (curr_player == PLAYER_DR_SEWARD){
					searching_move(hv,GRANADA,6);
					return;
				}else if (curr_player == PLAYER_VAN_HELSING){
					searching_move(hv,VIENNA,6);
					return;
				}else if (curr_player == PLAYER_MINA_HARKER){
					searching_move(hv,SOFIA,6);
					return;
				}
			//}
			//else{
			//	register_best_play (location_get_abbrev(hv_get_location(hv,curr_player)), "");
			//	return;
			//}
		}else{		
			if (hv_get_location(hv,PLAYER_DRACULA) != dra_last_seen && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)<3){
				searching_move(hv,dra_last_seen,4);
				return;
			}
				if(curr_player == PLAYER_LORD_GODALMING) {
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{

						searching_move(hv,dra_last_seen,4);
						return;

					}
				}else if (curr_player == PLAYER_DR_SEWARD){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen,4);
						return;

					}
				}else if (curr_player == PLAYER_VAN_HELSING){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen,4);
						return;

					}
				}else if (curr_player == PLAYER_MINA_HARKER){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen,4);
						return;

					}
				}	
//		
	
		}


	}
	

}
	
	





int distance (location_t src, location_t dest,HunterView hv){
	location_t path[100];
	int number_of_hops = findPath_hunter(hv, src, dest, path);
	return number_of_hops;
}

void searching_move(HunterView hv,location_t anker_point,int distance){
	time_t t1;
	srand((unsigned) time(&t1));
	enum player curr_player = hv_get_player(hv);
	size_t n_total_dests;		
	size_t n_not_in_trail_dests;
	size_t n_within_dis;
	size_t n_withids_and_not_intrial;
	location_t* H_dests = hv_get_dests (hv, &n_total_dests,true, true, true); 
	location_t* dests_not_in_trail = select_dests_not_in_trail(H_dests,&n_not_in_trail_dests,n_total_dests,hv);
	location_t* dests_in_dis = select_dests_less_than_distance(H_dests,&n_within_dis,anker_point,distance,n_total_dests,hv);
	location_t* dests_withids_and_not_intrial = find_common_locations 	   (dests_in_dis,dests_not_in_trail,n_within_dis,n_not_in_trail_dests,&n_withids_and_not_intrial);
	if (n_withids_and_not_intrial!=0){
		int index = rand()%n_withids_and_not_intrial;
		int true_index = (index>(n_withids_and_not_intrial-1))? index-1:index;
		register_best_play (location_get_abbrev(dests_withids_and_not_intrial[true_index]), "");	
	}else if (n_within_dis !=0){
		int index = rand()%n_within_dis;
		int true_index = (index>(n_within_dis-1))? index-1:index;
		register_best_play (location_get_abbrev(dests_in_dis[true_index]), "");	
	}else if (n_not_in_trail_dests!=0){
		int index = rand()%n_not_in_trail_dests;
		int true_index = (index>(n_withids_and_not_intrial-1))? index-1:index;
		register_best_play (location_get_abbrev(dests_not_in_trail[true_index]), "");	
	}else {
		int index = rand()%n_total_dests;
		int true_index = (index>(n_total_dests-1))? index-1:index;
		register_best_play (location_get_abbrev(H_dests[true_index]), "");

	}
}



location_t * select_dests_less_than_distance(location_t *dests,size_t *n_locations,location_t anker_point,int distance_away,int size,HunterView hv){
	size_t counter = 0;
	int selected_dests[100];
	for (int i = 0; i<size;i++){
			if (distance(dests[i],anker_point,hv)< distance_away){
				selected_dests[counter] = dests[i];
				counter++;
			} 
	}
	location_t *selected_locations = malloc((counter) * sizeof(location_t));
	for (int k = 0; k < counter; k++)
	{
		selected_locations[k] = selected_dests[k];
	}
	
	*n_locations = counter;
	return (counter==0)? NULL:selected_locations;
}




location_t* select_dests_not_in_trail(location_t *dests,size_t *n_locations,int size,HunterView hv){
	location_t trail[TRAIL_SIZE];
	hv_get_trail (hv, hv_get_player(hv),trail);
	location_t selected_dests[100];
	size_t counter_k = 0;
	for(int i = 0;i<size;i++){
		int found_flag = false;
		for(int j = 0; j<TRAIL_SIZE;j++){
			if(dests[i] == trail[j]){
				found_flag = true;
							
			}
		}
		if (found_flag == false){
			selected_dests[counter_k] = dests[i];
			counter_k++;				
		}

	}
	location_t *selected_locations = malloc((counter_k) * sizeof(location_t));
	for (int k = 0; k < counter_k; k++)
	{
		selected_locations[k] = selected_dests[k];
	}
	*n_locations = counter_k;

	return (counter_k==0)? NULL:selected_locations;
}


location_t* find_common_locations (location_t *dests_1,location_t *dests_2,int size_1,int size_2,size_t *n_locations){
	location_t common[100];
	int counter = 0;
	for(int i = 0;i<size_1;i++){
		for(int j = 0; j<size_2;j++){
			if(dests_1[i] == dests_2[j]){
				common[counter] = dests_1[i];
				counter++;			
			}
		}
	}
	location_t *selected_locations = malloc((counter) * sizeof(location_t));
	for (int k = 0; k < counter; k++)
	{
		selected_locations[k] = common[k];
	}
	*n_locations = counter;
	return (counter==0)? NULL:selected_locations;
}

void chase(location_t dra_last_seen,HunterView hv){
	enum player curr_player = hv_get_player(hv);
	location_t path[100];				
	int number_of_hops = findPath_hunter(hv, hv_get_location(hv,curr_player), dra_last_seen, path);
	for (int j = 0; j<number_of_hops;j++){
		printf("%s->",location_get_abbrev(path[j]));	
	}
	size_t n_locations;		
	location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true);
	int counter = 0;
	for(int i = 0;i<number_of_hops;i++){
		for(int j = 0; j<n_locations;j++){
			if(H_dests[j] == path[i]){
				counter = i;
			}
		}
	}
				
	if (hv_get_location(hv,curr_player) != dra_last_seen){
		register_best_play (location_get_abbrev(path[counter]), "");
	return;
				
	}else{
		register_best_play (location_get_abbrev(path[0]), "");
		return;
				


	}


}









	/*for (int j = 0; j<counter;j++){
		printf("seleted locations within distance %s\n",location_get_abbrev(final_dests[j]));	
	}
	printf("counter is %d\n",counter);
	int index = rand()% (counter+1);
	printf("index is %d\n",index);*/







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
/*size_t n_locations;															
					location_t trail[TRAIL_SIZE];
					location_t trail1[TRAIL_SIZE];
					location_t trail2[TRAIL_SIZE];
					location_t trail3[TRAIL_SIZE];											
					location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true); 	
					hv_get_trail (hv, PLAYER_LORD_GODALMING,trail);
					hv_get_trail (hv, PLAYER_DR_SEWARD,trail1);
					hv_get_trail (hv, PLAYER_VAN_HELSING,trail2);
					hv_get_trail (hv, PLAYER_MINA_HARKER,trail3);
					int All_dest_has_been = true;
					for(int i = 0;i<n_locations;i++){
						int found_flag = false;
						for(int j = 0; j<TRAIL_SIZE;j++){
							if(H_dests[i] == trail[j] || H_dests[i] == trail1[j] || H_dests[i] == trail2[j] || H_dests[i] == trail3[j]){
								found_flag = true;
							
							}
						}
						if (found_flag == false){
							register_best_play (location_get_abbrev(H_dests[i]), "");
							return;				
						}

					}
					register_best_play (location_get_abbrev(hv_get_location (hv, curr_player)), "");
					return;

				}else {


					register_best_play (location_get_abbrev(hv_get_location (hv, curr_player)), "");






				}*/



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



//location_t Dra_location = hv_get_location (hv, PLAYER_DRACULA);	//get dracula_current_location
		//if (!valid_location_p (Dra_location)) {	
			//int found_flag=false;	

/*for (int i = 0; i<n_locations;i++){
		printf("dests are %s\n",location_get_abbrev(H_dests[i]));	
	}
	
	

	hv_get_trail (hv, curr_player,trail);
	for (int k = 0; k<TRAIL_SIZE;k++){
		printf("trail are %s\n",location_get_abbrev(trail[k]));	
	}

	
	for (int i = 0; i<counter_k;i++){
		printf("seleted locations not in trail %s\n",location_get_abbrev(locations_not_in_trail[i]));	
	}*/
	//assert(1==2);




/*for (int j = 0; j<n_total_dests;j++){
		printf("total_dests are %s\n",location_get_abbrev(H_dests[j]));	
	}
	for (int j = 0; j<n_not_in_trail_dests;j++){
		printf("seleted locations not in trail %s\n",location_get_abbrev(dests_not_in_trail[j]));	
	}
	for (int j = 0; j<n_within_dis;j++){
		printf("seleted locations within distance %s\n",location_get_abbrev(dests_in_dis[j]));	
	}
	for (int j = 0; j<n_withids_and_not_intrial;j++){
		printf("seleted locations within distance &&not in trail %s\n",location_get_abbrev(dests_withids_and_not_intrial[j]));	
	}*/


/*if(curr_player == PLAYER_LORD_GODALMING) {
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{

						searching_move(hv,dra_last_seen);
						return;

					}
				}else if (curr_player == PLAYER_DR_SEWARD){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen);
						return;

					}
				}else if (curr_player == PLAYER_VAN_HELSING){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_LORD_GODALMING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_MINA_HARKER),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen);
						return;

					}
				}else if (curr_player == PLAYER_MINA_HARKER){
					if ( (distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_VAN_HELSING),dra_last_seen,hv))||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_DR_SEWARD),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv) )||(distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv,PLAYER_VAN_HELSING),dra_last_seen,hv) && distance(hv_get_location(hv,curr_player),dra_last_seen,hv)< distance(hv_get_location(hv, PLAYER_LORD_GODALMING),dra_last_seen,hv))){

					chase(dra_last_seen,hv);

					}
					else{
						searching_move(hv,dra_last_seen);
						return;

					}
				}	
//		





/*location_t path[100];

				//printf("%d",distance_);
				int number_of_hops = findPath_hunter(hv, hv_get_location(hv,curr_player), dra_last_seen, path);
				//assert(1==2);
				//for (int i =0;i<number_of_hops;i++){
				//	printf("%s-> ",  location_get_abbrev(path[i]));
				//}
				size_t n_locations;		
				location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true);
				int counter = 0;
				for(int i = 0;i<number_of_hops;i++){
					for(int j = 0; j<n_locations;j++){
						if(H_dests[j] == path[i]){
							counter = i;
						}
					}
				}
				if (hv_get_location(hv,curr_player) != dra_last_seen){
						register_best_play (location_get_abbrev(path[counter]), "");
						return;
				
				}else{
						register_best_play (location_get_abbrev(path[0]), "");
						return;
				}

			
			}*/

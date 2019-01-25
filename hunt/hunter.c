////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01 v1.0 Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01 v1.1 Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31 v2.0 Team Dracula <cs2521@cse.unsw.edu.au>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include <stdbool.h>
#include <time.h>



//int findPath(Graph g, Vertex src, Vertex dest, int max, int *path);
void decide_hunter_move (HunterView hv)
{
 enum player curr_player = hv_get_player(hv); // get current player
 round_t curr_round = hv_get_round(hv);
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
 }else{
    int path[70];
    int n = findPath_hunter(hv, BRUSSELS, CASTLE_DRACULA, path);
    
    for (int i =0; i < n; i++){
           printf("shorted path is %s\n",location_get_abbrev(path[i]));      
    }
  location_t Dra_location = hv_get_location (hv, PLAYER_DRACULA); //get dracula_current_location
  //if (!valid_location_p (Dra_location)) {       //if the location is UNKNOWN
   size_t n_locations;           //initialize number of locations
   location_t trail[TRAIL_SIZE];        //initalize Trail array
   location_t* H_dests = hv_get_dests (hv, &n_locations,true, true, true); //initialize destination array
   hv_get_trail (hv, curr_player,trail);
   //if(curr_player == PLAYER_LORD_GODALMING &&  trail[5] == BRUSSELS ){
   // register_best_play ("BU", "");
   // return;
  // }
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

   }
   register_best_play (location_get_abbrev(trail[1]), "");
   return;
  
  //}else{




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

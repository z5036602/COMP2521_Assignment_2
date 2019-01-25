////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//Zhengyue LIU Rudy
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>

#include "game.h"
#include "game_view.h"
#include "hunter_view.h"
#include "map.h" 
//#include "Queue.h"





#define ItemCopy(i)     (i)
#define ItemEQ(i1,i2)   ((i1) == (i2))
#define ItemShow(i)     printf("%d",(i))
typedef int Item;
typedef struct QueueRep *Queue;
typedef struct QueueNode {
	Item value;
	struct QueueNode *next;
} QueueNode;

typedef struct QueueRep {
	QueueNode *head;  // ptr to first node
	QueueNode *tail;  // ptr to last node
} QueueRep;






typedef struct hunter_view {
	GameView gv;                             //gameview ADT parent class
	char *real_past_plays;				     //record the real past playstring(a string which turns TBHI,CD to their real loaction		        
	int real_location_array[TRAIL_SIZE];     //save real loction from double back and hide;
	location_t Dracula_real_curr_location; 
	player_message *messages;
	Map world_map;
	location_t dra_last_seen;
} hunter_view;

static void original_string_to_real (char* string);
static location_t abbrev_string_to_location (char a, char b) ;
static void rotating_sarray (int *array,int a,int size);
static void record_location (hunter_view *hv,char * past_play_array,unsigned int position);
static Queue newQueue(); // create new empty queue
static void dropQueue(Queue); // free memory used by queue
//void showQueue(Queue); // display as 3 > 5 > 4 > ...
static void QueueJoin(Queue,Item); // add item on queues
static Item QueueLeave(Queue); // remove item from queue
static int QueueIsEmpty(Queue); // check for no items
static void swap_element_array(int a,int b,int* array);
hunter_view *hv_new (char *past_plays, player_message messages[])
{
	
	//create a new hunter view;
	hunter_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate HunterView");

	assert (new != NULL);
    new->world_map = map_new();
	//initial struct and turn past plays to real locations;
	new->real_past_plays = strdup(past_plays);
	original_string_to_real(new->real_past_plays);
	new->messages =messages;
	new->gv = gv_new (past_plays, messages);
	assert (new->gv != NULL);
	new->dra_last_seen = -1;

	//initial and create hunter view real location; in order to save real loction from double back and hide;
    int m = 0;
	  		while (m < TRAIL_SIZE) {
	  			new->real_location_array[m] = UNKNOWN_LOCATION;
	  			m++;
	  		}
	
	//deal with real past plays;
	unsigned int k = 0;
	while(k < strlen(new->real_past_plays)){	    																		
		if (new->real_past_plays[k] == 'D'){					//when player is dracula;
			location_t id = abbrev_string_to_location(new->real_past_plays[k+1],new->real_past_plays[k+2]);
			if(valid_location_p(id) == true){
				new->Dracula_real_curr_location = id;     		// if the location is valid actually valid , the dracula db to a valid location
			}else{
				new->Dracula_real_curr_location=gv_get_location (new->gv, PLAYER_DRACULA);
			}
	        record_location (new, new->real_past_plays, k);
		}
		
		k+=8;
       
	}
	return new;
}

void hv_drop (hunter_view *hv)
{
    gv_drop (hv->gv);
    map_drop(hv->world_map);
	free(hv->real_past_plays);
	free (hv);
}
void hv_dra_last_seen_setter (hunter_view *hv,location_t last_seen){
	hv->dra_last_seen = last_seen;

}
location_t hv_dra_last_seen_getter (hunter_view *hv){
	return hv->dra_last_seen;
}
round_t hv_get_round (hunter_view *hv)
{
	assert(hv!=NULL);
	return gv_get_round (hv->gv);
}

enum player hv_get_player (hunter_view *hv)
{
	assert(hv!=NULL);
	return gv_get_player (hv->gv);
}

int hv_get_score (hunter_view *hv)
{
	assert(hv!=NULL);
	return gv_get_score(hv->gv);
}

int hv_get_health (hunter_view *hv, enum player player)
{
	assert(hv!=NULL);
	return gv_get_health (hv->gv, player);
}

location_t hv_get_location (hunter_view *hv, enum player player)
{	
	assert(hv!=NULL);
	if(player != PLAYER_DRACULA){
		return gv_get_location (hv->gv, player);
	}else{
		return hv->Dracula_real_curr_location;

	}
}

void hv_get_trail (
	hunter_view *hv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	assert(hv!=NULL);
	gv_get_history (hv->gv, player, trail);
}

location_t *hv_get_dests (
	hunter_view *hv, size_t *n_locations,
	bool road, bool rail, bool sea)
{	
	assert(hv!=NULL);
	enum player curr_player = hv_get_player (hv);
	location_t curr_location = hv_get_location (hv, curr_player);
	round_t curr_round = hv_get_round (hv);

	location_t *where_to_go = gv_get_connections (hv->gv, n_locations, curr_location, curr_player, curr_round, road, rail, sea);
	
	
	return where_to_go;
}

location_t *hv_get_dests_player (
	hunter_view *hv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{   
	assert(hv!=NULL);
	//when player is dracula;map_drop(dv->world_map);
    if (player == PLAYER_DRACULA) {

        //when real loction is city unkonwn or sea unknown;
        if (!valid_location_p (hv->Dracula_real_curr_location)) {
            *n_locations = 0;
            return NULL;
        
        // when real location is city or sea; 
        } else {
            round_t curr_round = hv_get_round (hv);
	        location_t *where_to_go = gv_get_connections (hv->gv, n_locations, hv->Dracula_real_curr_location, PLAYER_DRACULA, curr_round, road, rail, sea);
	        
	        // replace location (on trail) with rondom number (bigger than all places number);
	        for (size_t i = 1; i < *n_locations; i++) {
	            for (size_t j = 0; j < TRAIL_SIZE; j++) {
	                if (where_to_go[i] == hv->real_location_array[j]) {
	                    where_to_go[i] = '\0';
	                }
	                
	            }
	        }

	        // get size;
	        size_t new_n_location = 0;
	        
	        for (unsigned int k = 0; k < *n_locations; k++) {
	            if (where_to_go[k] != '\0') {
	                new_n_location ++;
	                
	            }
	        }

	        // malloc a new array;
	        location_t *update_where_to_go = malloc (sizeof (int) * new_n_location);
	        
	        int a = 0;
	        unsigned int b = 0;
	        
	        //put loction into new armap_drop(dv->world_map);ray;
	        while (b < *n_locations) {
	            if (where_to_go[b] != '\0') {
	                update_where_to_go[a] = where_to_go[b];
	                a++;
	                
	            }
	            b++;
            }
   
	        *n_locations = new_n_location;

	        free (where_to_go);  

	        return update_where_to_go;
	    
	    } 
	// when player is hunter;  
	} else {
	
	location_t curr_location = hv_get_location (hv, player);
	
	round_t curr_round = hv_get_round (hv);
	
	location_t *where_to_go = gv_get_connections (hv->gv, n_locations, curr_location, player, curr_round, road, rail, sea);
	
	return where_to_go;
    }
}
//BFS
int findPath_hunter(hunter_view *hv, location_t src, location_t dest, location_t *path){
    assert(hv->world_map != NULL);
    int visited[hv->world_map->n_vertices];
    location_t st[hv->world_map->n_vertices];
    //printf("%d\n",g->edges[start_node_of_edge][0]);
    //size_t counter = 0;
    for (int i = 0;i < hv->world_map->n_vertices;i++){
        visited [i] = -1;
        st [i] = -1;
    }
    Queue q = newQueue();
    QueueJoin (q, src);
    visited[src]=1;
    while ( QueueIsEmpty(q) != 1 ){ 
         location_t start_node_of_edge = QueueLeave(q);   // to find the ajacent
        for (map_adj *curr_1 = hv->world_map->connections[start_node_of_edge]; curr_1 != NULL; curr_1 = curr_1->next){
            if (visited[curr_1->v] == -1){
                                  // it actually means the vertice is already saved
                 QueueJoin (q, curr_1->v);
                 visited[curr_1->v]=1;   
                 st[curr_1->v] = start_node_of_edge;
            }
        }
  
  
     }
    dropQueue(q);
     if (dest == src){
         path[0] = src;
        return 1;
     }
    if (st[dest] == -1){ 
         return 0; 
    }

    int i = 0;
     while (src != dest){
      path[i]= dest;
     dest = st[dest];
     i++;
     }
    path[i]= src;
    int number_elements = i+1;

    int j=0;
    while (i>=j){
        swap_element_array(i,j,path);
         i--;
         j++;
    }
    return number_elements;


}

///////////////////////////////////////////////////////helper functions//////////////////////////////////////////////////////////
static void swap_element_array(int a,int b,int* array){
	int tmp = array[a];
	array[a] = array[b];
	array[b] = tmp;
}
static void original_string_to_real (char* string){
	unsigned int i=73;
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

static void record_location (hunter_view *hv,char * past_play_array,unsigned int position){
	int location_id = abbrev_string_to_location (past_play_array[position+1], past_play_array[position+2]);
	
	rotating_sarray (hv->real_location_array,location_id,TRAIL_SIZE);
}

static void rotating_sarray (int *array,int a,int size){
  	for(int i=size-1 ; i>=1 ; i--){
    	array[i]=array[i-1] ;
	}
  	array[0]=a;
}
static Queue newQueue(void)
{
	Queue q;
	q = malloc(sizeof(QueueRep));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Queue
static void dropQueue(Queue Q)
{
	QueueNode *curr, *next;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue rep
	free(Q);
}

// display as 3 > 5 > 4 > ...
/*void showQueue(Queue Q)
{
	QueueNode *curr;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		ItemShow(curr->value);
		if (curr->next != NULL)
			printf(">");
		curr = curr->next;
	}
	printf("\n");
}*/

// add item at end of Queue 
static void QueueJoin(Queue Q, Item it)
{
	assert(Q != NULL);
	QueueNode *new = malloc(sizeof(QueueNode));
	assert(new != NULL);
	new->value = ItemCopy(it);
	new->next = NULL;
	if (Q->head == NULL)
		Q->head = new;
	if (Q->tail != NULL)
		Q->tail->next = new;
	Q->tail = new;
}

// remove item from front of Queue
static Item QueueLeave(Queue Q)
{
	assert(Q != NULL);
	assert(Q->head != NULL);
	Item it = ItemCopy(Q->head->value);
	QueueNode *old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL)
		Q->tail = NULL;
	free(old);
	return it;
}

// check for no items
static int QueueIsEmpty(Queue Q)
{
	return (Q->head == NULL);
}

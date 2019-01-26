////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "dracula.h"

#define ItemCopy(i) (i)
#define ItemEQ(i1, i2) ((i1) == (i2))
#define ItemShow(i) printf("%d", (i))
typedef int Item;
typedef struct QueueRep *Queue;
typedef struct QueueNode
{
	Item value;
	struct QueueNode *next;
} QueueNode;

typedef struct QueueRep
{
	QueueNode *head; // ptr to first node
	QueueNode *tail; // ptr to last node
} QueueRep;

static Queue newQueue();	  // create new empty queue
static void dropQueue(Queue); // free memory used by queue
//void showQueue(Queue); // display as 3 > 5 > 4 > ...
static void QueueJoin(Queue, Item); // add item on queues
static Item QueueLeave(Queue);		// remove item from queue
static int QueueIsEmpty(Queue);		// check for no items
int minFour(int a, int b, int c, int d);

void findAllPathBFS(Map map, location_t src, int visited[]);

void decide_dracula_move(DraculaView dv)
{
	// time_t t1;
	// srand((unsigned) time(&t1));
	// round_t curr_round = dv_get_round(dv);
	// if (curr_round == 0){
	// 	register_best_play ("CD", "haha");
	// 	//register_best_play ("CD", "");
	// 	//register_best_play ("CD", "");
	// }else if (curr_round == 1){
	// 	register_best_play ("GA", "hahaha");

	// }else if (curr_round == 2){
	// 	register_best_play ("KL", "");

	// }else if (curr_round == 3){
	// 	register_best_play ("D3", "");

	// }else if (curr_round == 4){
	// 	register_best_play ("HI", "TP");

	// }
	// else{
	// 	size_t n_locations;
	// 	location_t *array = dv_get_dests (dv, &n_locations, true, true);
	// 	int number = n_locations;
	// 	int index = 1+rand()%number;
	// 	char *play = location_get_abbrev (array[index-1]);
	// 	printf("index is :%d\n",index);
	// 	printf("n_location is :%d\n",number);

	// 	//char *new_play = (char*) play;
	// 	/// @todo Replace this with something better!
	// 	register_best_play (play, "");
	// }

	round_t curr_round = dv_get_round(dv);
	location_t best_move;

	//find all shortest path to all city for each hunter
	int path_lenght[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_PLAYERS - 1; i++)
	{
		location_t curr_location = dv_get_location(dv, i);
		findAllPathBFS(dv->world_map, curr_location, path_lenght[i]);
	}

	//select min length path from each hunter
	int min_lenght[NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		min_lenght[i] = minFour(path_lenght[PLAYER_LORD_GODALMING][i], path_lenght[PLAYER_DR_SEWARD][i], path_lenght[PLAYER_VAN_HELSING][i], path_lenght[PLAYER_MINA_HARKER][i]);

	// for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
	// 	printf("%s %d %d\n", location_get_name(i), min_lenght[i],dv->world_map->degree_without_rail[i]);

	if (curr_round == 0)
	{
		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		int max_lenght = -1;
		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		{
			if (min_lenght[i] > max_lenght)
			{
				max_lenght = min_lenght[i];
				best_move = i;
			}
			else if (min_lenght[i] == max_lenght)

				if (dv->world_map->degree_without_rail[i] > dv->world_map->degree_without_rail[best_move])
					best_move = i;
		}
	}
	else
	{
		size_t n_locations;
		location_t *can_travel = dv_get_dests(dv, &n_locations, true, true);
		location_t real_path_trail[TRAIL_SIZE];
		location_t special_move[n_locations];
		dv_get_trail(dv, PLAYER_DRACULA, real_path_trail);
		// printf("%d n_locations\n", n_locations);

		// change special move to real location and remember it
		for (int i = 0; i < n_locations; i++)
		{
			//printf("%d %d\n", i, can_travel[i]);
			if (can_travel[i] == HIDE)
			{
				special_move[i] = HIDE;
				can_travel[i] = real_path_trail[0];
			}
			else if (can_travel[i] == DOUBLE_BACK_1 && curr_round != 1)
			{
				special_move[i] = DOUBLE_BACK_1;
				can_travel[i] = real_path_trail[0];
			}
			else if (can_travel[i] == DOUBLE_BACK_2 && curr_round != 2)
			{
				special_move[i] = DOUBLE_BACK_2;
				can_travel[i] = real_path_trail[1];
			}
			else if (can_travel[i] == DOUBLE_BACK_3 && curr_round != 3)
			{
				special_move[i] = DOUBLE_BACK_3;
				can_travel[i] = real_path_trail[2];
			}
			else if (can_travel[i] == DOUBLE_BACK_4 && curr_round != 4)
			{
				special_move[i] = DOUBLE_BACK_4;
				can_travel[i] = real_path_trail[3];
			}
			else if (can_travel[i] == DOUBLE_BACK_5 && curr_round != 5)
			{
				special_move[i] = DOUBLE_BACK_5;
				can_travel[i] = real_path_trail[4];
			}
			else
				special_move[i] = UNKNOWN;
			// printf("can_travel[i] %s\n",location_get_name(can_travel[i]));
		}

		int bestIdx = -1;
		int max_lenght = -1;
		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(can_travel[i]))
			{
				if (min_lenght[can_travel[i]] > max_lenght)
				{
					max_lenght = min_lenght[can_travel[i]];
					best_move = can_travel[i];
					bestIdx = i;
				}
				else if (min_lenght[can_travel[i]] == max_lenght)
					if (dv->world_map->degree_without_rail[can_travel[i]] > dv->world_map->degree_without_rail[best_move])
					{
						best_move = can_travel[i];
						bestIdx = i;
					}
			}
		}
		// printf("move to %s %d\n", location_get_name(best_move), best_move);

		// if real location is from special skill
		if (special_move[bestIdx] != UNKNOWN){
			// puts("spe");
			// printf("%d %d\n",special_move[bestIdx],bestIdx);
			best_move = special_move[bestIdx];
		}
			
	}

	// printf("move to %s %d\n", location_get_name(best_move), best_move);
	register_best_play(location_get_abbrev(best_move), "Dracula is moving!!!");
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
	while (curr != NULL)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue rep
	free(Q);
}

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

void findAllPathBFS(Map map, location_t src, int visited[])
{
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		visited[i] = -1;
	visited[src] = 0;
	Queue q = newQueue();
	QueueJoin(q, src);
	while (!QueueIsEmpty(q))
	{
		Item v = QueueLeave(q);
		for (map_adj *curr = map->connections[v]; curr != NULL; curr = curr->next)
		{
			if (visited[curr->v] == -1)
			{
				visited[curr->v] = visited[v] + 1;
				QueueJoin(q, curr->v);
			}
		}
	}
}

int minFour(int a, int b, int c, int d)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	if (d < min)
		min = d;
	return min;
}

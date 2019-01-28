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

#define HIGHEST_SCORE 100
#define LOWEST_SCORE -100

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
static bool can_go(location_t can_travel[], location_t city, int n_locations);

void findAllPathBFS(Map map, location_t src, int visited[], int path[]);

void decide_dracula_move(DraculaView dv)
{

	round_t curr_round = dv_get_round(dv);
	location_t best_move;

	//find all shortest path to all city for each hunter
	int path_lenght[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
	int path[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_PLAYERS - 1; i++)
	{
		location_t curr_location = dv_get_location(dv, i);
		findAllPathBFS(dv->world_map, curr_location, path_lenght[i], path[i]);
	}

	//select min length path from each hunter and initial score for each location
	int min_lenght[NUM_MAP_LOCATIONS];
	double location_score[NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
	{
		min_lenght[i] = minFour(path_lenght[PLAYER_LORD_GODALMING][i], path_lenght[PLAYER_DR_SEWARD][i], path_lenght[PLAYER_VAN_HELSING][i], path_lenght[PLAYER_MINA_HARKER][i]);
		location_score[i] = 0.0;
	}

	if (curr_round == 0)
	{
		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		{
			location_score[i] += min_lenght[i];
			location_score[i] += (dv->world_map->degree_without_rail[i] / 10.0);
		}

		double max_score = -999999999;
		int max_idx = -1;

		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		{
			if (location_score[i] > max_score)
			{
				max_score = location_score[i];
				max_idx = i;
			}
		}
		best_move = max_idx;
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

		//find max length of minimum distance from each hunter to dracula
		int max_lenght_of_minimum_distance = -1;
		for (int i = 0; i < n_locations; i++)
			if (valid_location_p(can_travel[i]) && min_lenght[can_travel[i]] > max_lenght_of_minimum_distance)
				max_lenght_of_minimum_distance = min_lenght[can_travel[i]];

		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(can_travel[i]))
			{
				location_t curr_location = can_travel[i];
				int dracula_blood = dv_get_health(dv, PLAYER_DRACULA);

				location_score[curr_location] += min_lenght[curr_location];
				location_score[curr_location] += (dv->world_map->degree_without_rail[curr_location] / 10.0);

				// the less of his blood, the less he want to go to the sea
				if (location_get_type(curr_location) == SEA)
					location_score[curr_location] = location_score[curr_location] * (dracula_blood / GAME_START_BLOOD_POINTS);

				// do not need to use special move if 3 <= max_lenght_of_minimum_distance < 6
				if (max_lenght_of_minimum_distance < 6 && max_lenght_of_minimum_distance >= 3)
				{
					if (special_move[i] != UNKNOWN)
					{
						location_score[curr_location] -= (HIGHEST_SCORE / 2);
					}
				}

				//go to sea
				if (max_lenght_of_minimum_distance <= 2 && location_get_type(curr_location) == SEA && dracula_blood > 2)
				{
					location_score[curr_location] += HIGHEST_SCORE;
				}

				//must not go to sea if life <= 2
				// printf("dracula_blood %d\n", dracula_blood);
				if (dracula_blood <= 2 && location_get_type(curr_location) == SEA)
				{

					location_score[curr_location] = LOWEST_SCORE;
				}

				// if near Castle Dracula and hunter is very far
				if (curr_location == CASTLE_DRACULA && max_lenght_of_minimum_distance > 3)
				{
					location_score[curr_location] += HIGHEST_SCORE;
				}
			}
		}

		//very close to hunter !! try to go near sea
		// if (max_lenght_of_minimum_distance <= 2)
		// {

		// 	int sea_length[NUM_MAP_LOCATIONS];
		// 	int sea_path[NUM_MAP_LOCATIONS];
		// 	location_t src = dv_get_location(dv, PLAYER_DRACULA);
		// 	findAllPathBFS(dv->world_map, src, sea_length, sea_path);

		// 	int min_sea = 9999999;
		// 	for (int j = 0; j < NUM_MAP_LOCATIONS; j++)
		// 	{
		// 		//find nearest sea with far from hunter

		// 		//find next city to dracula that nearest to sea
		// 		int next_city;
		// 		if (location_get_type(j) == SEA)
		// 		{
		// 			if(sea_length[j] > min_sea){

		// 			}
		// 		}

		// 		if (location_get_type(j) == SEA && sea_length[next_city] < min_sea && min_lenght[next_city] > 1 && can_go(can_travel, next_city, n_locations))
		// 		{
		// 			min_sea = sea_length[j];
		// 		}
		// 	}
		// }

		// printf("move to %s %d\n", location_get_name(best_move), best_move);

		double max_score = -999999;
		int max_idx = -1;


		// find best score location to move
		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(can_travel[i]) && location_score[can_travel[i]] > max_score)
			{
				max_score = location_score[can_travel[i]];
				max_idx = can_travel[i];
			}
			//if best score is eqaul find the location that is far from all hunter
			else if (valid_location_p(can_travel[i]) && location_score[can_travel[i]] == max_score)
			{

				int this_min = 99999999;
				for (int j = 0; j < NUM_PLAYERS - 1; j++)
				{

					if (min_lenght[can_travel[i]] > min_lenght[max_idx])
					{
						max_idx = can_travel[i];
					}
					else
					{
						// printf("hunter to %d %d\n", j, path_lenght[j][max_idx]);
						// printf("hunter to %d %d\n", j, path_lenght[j][can_travel[i]]);
						int new_lose = 0;
						int old_lose = 0;
						if (path_lenght[j][max_idx] == min_lenght[max_idx])
							old_lose++;
						if (path_lenght[j][can_travel[i]] == min_lenght[can_travel[i]])
							new_lose++;
						if (new_lose < old_lose)
							max_idx = can_travel[i];
					}
				}
			}
		}
		best_move = max_idx;
		// printf("move to %s %d\n", location_get_name(best_move), best_move);

		// if real location is from special skill
		for (int i = 0; i < n_locations; i++)
			if (best_move == can_travel[i])
				if (special_move[i] != UNKNOWN)
					best_move = special_move[i];

		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(can_travel[i]))
			{
				printf("%.2f %s\n", location_score[can_travel[i]], location_get_name(can_travel[i]));
			}
		}

		printf("max_lenght_of_minimum_distance: %d\n", max_lenght_of_minimum_distance);
	}

	//printf("best %s\t\t%.2f\n", location_get_name(best_move), location_score[best_move]);
	printf("move to %s %d\n", location_get_name(best_move), best_move);
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

void findAllPathBFS(Map map, location_t src, int visited[], int path[])
{
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
	{
		visited[i] = -1;
		path[i] = -1;
	}

	visited[src] = 0;
	path[src] = src;
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
				path[curr->v] = v;
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

static bool can_go(location_t can_travel[], location_t city, int n_locations)
{
	for (int i = 0; i < n_locations; i++)
		if (can_travel[i] == city)
			return true;
	return false;
}

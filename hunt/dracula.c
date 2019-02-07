////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

//2521 dryrun assign2hunt Makefile *.c *.h

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
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
static bool can_go(location_t dracula_can_travel[], location_t city, int n_locations);
static bool isDeadEnd(DraculaView dv, location_t trail[], location_t old_trail[], location_t location, location_t special_move);
void dv_get_old_trail(DraculaView dv, location_t trail[TRAIL_SIZE]);

void findAllPathBFS(Map map, location_t src, int visited[], int path[]);
void findAllPathBFSwithoutTrailAvoidHunter(Map map, location_t src, int visited[], int path[], int trail[], int all_hunter_dest[]);

void decide_dracula_move(DraculaView dv)
{

	round_t curr_round = dv_get_round(dv);
	location_t best_move;

	//find all shortest path to all city for each hunter
	int path_lenght[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
	int path[NUM_PLAYERS - 1][NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_PLAYERS - 1; i++)
	{
		location_t hunter_location = dv_get_location(dv, i);
		findAllPathBFS(dv->world_map, hunter_location, path_lenght[i], path[i]);
	}

	//select min length path from each hunter and initial score for each location
	int min_lenght[NUM_MAP_LOCATIONS];
	int sum_lenght[NUM_MAP_LOCATIONS];
	location_t furthest_place_from_hunter = UNKNOWN_LOCATION;
	int max_length_from_hunters = -1;
	double location_score[NUM_MAP_LOCATIONS];
	for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
	{
		min_lenght[i] = minFour(path_lenght[PLAYER_LORD_GODALMING][i], path_lenght[PLAYER_DR_SEWARD][i], path_lenght[PLAYER_VAN_HELSING][i], path_lenght[PLAYER_MINA_HARKER][i]);
		if (min_lenght[i] > max_length_from_hunters)
		{
			max_length_from_hunters = min_lenght[i];
			furthest_place_from_hunter = i;
		}
		sum_lenght[i] = path_lenght[PLAYER_LORD_GODALMING][i] + path_lenght[PLAYER_DR_SEWARD][i] + path_lenght[PLAYER_VAN_HELSING][i] + path_lenght[PLAYER_MINA_HARKER][i];
		location_score[i] = 0.0;
	}

	if (curr_round == 0)
	{
		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
		{
			if (location_get_type(i) == LAND)
			{
				location_score[i] += min_lenght[i];
				location_score[i] += (sum_lenght[i] * 1.0) / (NUM_PLAYERS - 1);
				location_score[i] += (dv->world_map->degree_without_rail[i] / 7.0);
			}
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
		location_t *dracula_can_travel = dv_get_dests(dv, &n_locations, true, true);
		location_t real_path_trail[TRAIL_SIZE];
		location_t old_path_trail[TRAIL_SIZE];
		location_t special_move[n_locations];
		location_t dracula_location = dv_get_location(dv, PLAYER_DRACULA);
		int dracula_distance_from_city[NUM_MAP_LOCATIONS];
		int dracula_path[NUM_MAP_LOCATIONS];
		int dracula_distance_from_huner[NUM_PLAYERS - 1];
		int minimum_distance_dracula_from_closest_hunter = 9999999;
		findAllPathBFS(dv->world_map, dracula_location, dracula_distance_from_city, dracula_path);

		location_t *hunter_can_travel[NUM_PLAYERS - 1];
		int hunter_n_location[NUM_PLAYERS - 1];
		int num_dest_of_hunter[NUM_MAP_LOCATIONS];

		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
			num_dest_of_hunter[i] = 0;

		for (int i = 0; i < NUM_PLAYERS - 1; i++)
		{
			hunter_can_travel[i] = dv_get_dests_player(dv, &hunter_n_location[i], i, true, true, true);
			for (int j = 0; j < hunter_n_location[i]; j++)
				if (valid_location_p(hunter_can_travel[i][j]))
					num_dest_of_hunter[hunter_can_travel[i][j]]++;
			location_t hunter_location = dv_get_location(dv, i);
			dracula_distance_from_huner[i] = dracula_distance_from_city[hunter_location];
			if (dracula_distance_from_huner[i] < minimum_distance_dracula_from_closest_hunter)
				minimum_distance_dracula_from_closest_hunter = dracula_distance_from_huner[i];
		}

		bool is_Calculate[NUM_MAP_LOCATIONS] = {false};
		bool is_Castle_Dracula_In_Trail = false;
		bool can_HIDE = false;
		dv_get_trail(dv, PLAYER_DRACULA, real_path_trail);
		dv_get_old_trail(dv, old_path_trail);
		// printf("%d n_locations\n", n_locations);

		int distance_from_furthest_place_from_hunter[NUM_MAP_LOCATIONS];
		int max_distance_from_furthest_place_from_hunter = -99999;
		int path_from_furthest_place_from_hunter[NUM_MAP_LOCATIONS];
		findAllPathBFSwithoutTrailAvoidHunter(dv->world_map, furthest_place_from_hunter, distance_from_furthest_place_from_hunter, path_from_furthest_place_from_hunter, real_path_trail, num_dest_of_hunter);
		for (int i = 0; i < NUM_MAP_LOCATIONS; i++)
			if (distance_from_furthest_place_from_hunter[i] > max_distance_from_furthest_place_from_hunter)
				max_distance_from_furthest_place_from_hunter = distance_from_furthest_place_from_hunter[i];

		// change special move to real location and remember it
		for (int i = 0; i < n_locations; i++)
		{
			//printf("%d %d\n", i, dracula_can_travel[i]);
			if (dracula_can_travel[i] == HIDE)
			{
				special_move[i] = HIDE;
				dracula_can_travel[i] = real_path_trail[0];
				can_HIDE = true;
			}
			else if (dracula_can_travel[i] == DOUBLE_BACK_1 && curr_round != 1)
			{
				special_move[i] = DOUBLE_BACK_1;
				dracula_can_travel[i] = real_path_trail[0];
			}
			else if (dracula_can_travel[i] == DOUBLE_BACK_2 && curr_round != 2)
			{
				special_move[i] = DOUBLE_BACK_2;
				dracula_can_travel[i] = real_path_trail[1];
			}
			else if (dracula_can_travel[i] == DOUBLE_BACK_3 && curr_round != 3)
			{
				special_move[i] = DOUBLE_BACK_3;
				dracula_can_travel[i] = real_path_trail[2];
			}
			else if (dracula_can_travel[i] == DOUBLE_BACK_4 && curr_round != 4)
			{
				special_move[i] = DOUBLE_BACK_4;
				dracula_can_travel[i] = real_path_trail[3];
			}
			else if (dracula_can_travel[i] == DOUBLE_BACK_5 && curr_round != 5)
			{
				special_move[i] = DOUBLE_BACK_5;
				dracula_can_travel[i] = real_path_trail[4];
			}
			else
				special_move[i] = UNKNOWN;
			// printf("dracula_can_travel[i] %s\n",location_get_name(dracula_can_travel[i]));
		}

		for (int i = 0; i < TRAIL_SIZE; i++)
			if (real_path_trail[i] == CASTLE_DRACULA)
				is_Castle_Dracula_In_Trail = true;

		//find max length of minimum distance from each hunter to dracula
		int max_lenght_of_minimum_distance = -1;
		int min_lenght_of_minimum_distance = 9999999;
		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(dracula_can_travel[i]) && min_lenght[dracula_can_travel[i]] > max_lenght_of_minimum_distance)
				max_lenght_of_minimum_distance = min_lenght[dracula_can_travel[i]];
			if (valid_location_p(dracula_can_travel[i]) && min_lenght[dracula_can_travel[i]] < min_lenght_of_minimum_distance)
				min_lenght_of_minimum_distance = min_lenght[dracula_can_travel[i]];
		}

		// select start move with maximum min length path and have hightest degree (number of adjacent city by road and sea)
		for (int i = 0; i < n_locations; i++)
		{
			if (valid_location_p(dracula_can_travel[i]) && !is_Calculate[dracula_can_travel[i]])
			{
				location_t curr_location = dracula_can_travel[i];
				is_Calculate[dracula_can_travel[i]] = true;
				int dracula_blood = dv_get_health(dv, PLAYER_DRACULA);

				// if can place vampire
				if (curr_round % 13 == 0 && location_get_type(curr_location) == LAND)
				{
					location_score[curr_location] += 1;
				}

				location_score[curr_location] += min_lenght[curr_location];
				location_score[curr_location] += (sum_lenght[curr_location] * 1.0) / (NUM_PLAYERS - 1);
				location_score[curr_location] += (dv->world_map->degree_without_rail[curr_location] / 10.0);
				if (distance_from_furthest_place_from_hunter[curr_location] > 0)
					location_score[curr_location] += ((max_distance_from_furthest_place_from_hunter - distance_from_furthest_place_from_hunter[curr_location]) * 10.0) / max_distance_from_furthest_place_from_hunter;

				// printf("%s %.2f\n", location_get_name(curr_location), location_score[curr_location]);

				// printf("%s %.2f\n", location_get_name(curr_location), location_score[curr_location]);

				// do not need to use special move if 3 <= max_lenght_of_minimum_distance < 6
				if (min_lenght_of_minimum_distance < 6 && min_lenght_of_minimum_distance >= 4 && special_move[i] != UNKNOWN)
					location_score[curr_location] -= (HIGHEST_SCORE / 2.0);

				//if hunter is very near to dracula go to sea
				if (minimum_distance_dracula_from_closest_hunter < 2 && location_get_type(curr_location) == SEA && dracula_blood > 2)
				{
					location_score[curr_location] += HIGHEST_SCORE;
				}

				// printf("%s %.2f\n", location_get_name(curr_location), location_score[curr_location]);

				if (curr_location == CASTLE_DRACULA)
				{
					// if near Castle Dracula and hunter is very far
					if (min_lenght[curr_location] > 3 && sum_lenght[curr_location] > 15)
						if (!is_Castle_Dracula_In_Trail || (is_Castle_Dracula_In_Trail && can_HIDE))
							location_score[curr_location] += HIGHEST_SCORE;
						// don't go to castle
						else if (min_lenght[curr_location] < 3 || is_Castle_Dracula_In_Trail)
							location_score[curr_location] -= HIGHEST_SCORE;
				}

				if (isDeadEnd(dv, real_path_trail, old_path_trail, curr_location, special_move[i]))
				{
					if (min_lenght[CASTLE_DRACULA] > 3 && sum_lenght[CASTLE_DRACULA] > 15 && !is_Castle_Dracula_In_Trail)
					{
						location_score[curr_location] += HIGHEST_SCORE;
					}

					else
						location_score[curr_location] -= HIGHEST_SCORE;
				}

				// if place is land and hunter is at that place, not go to it.
				if (location_get_type(curr_location) == LAND && min_lenght[curr_location] == 0)
				{

					location_score[curr_location] -= (HIGHEST_SCORE * sum_lenght[curr_location]);
				}

				// minus score if location is have to use special skill
				if (special_move[i] != UNKNOWN)
					location_score[curr_location] -= minimum_distance_dracula_from_closest_hunter;

				// if location is hunter destination
				if (location_get_type(curr_location) == LAND)
				{
					location_score[curr_location] -= (HIGHEST_SCORE * num_dest_of_hunter[curr_location]);
				}

				// printf("%s %.2f\n", location_get_name(curr_location), location_score[curr_location]);
				// the less of his blood, the less he want to go to the sea
				if (location_get_type(curr_location) == SEA && dracula_blood <= GAME_START_BLOOD_POINTS && location_score[curr_location] > 0)
					location_score[curr_location] = location_score[curr_location] * (dracula_blood * 1.0 / GAME_START_BLOOD_POINTS);

				// no need to go to the sea if very far
				if (minimum_distance_dracula_from_closest_hunter > 3 && location_get_type(curr_location) == SEA)
					location_score[curr_location] -= (HIGHEST_SCORE / 2.0);

				//must not go to sea if life <= 2
				if (dracula_blood <= 2 && location_get_type(curr_location) == SEA)
					location_score[curr_location] = LOWEST_SCORE;
			}
		}

		double max_score = -999999;
		int max_idx = -1;

		// find best score location to move
		for (int i = 0; i < n_locations; i++)
		{
			// printf("-> %d %d %.3f %.3f\n",i,dracula_can_travel[i],max_score,location_score[dracula_can_travel[i]]);

			if (valid_location_p(dracula_can_travel[i]) && location_score[dracula_can_travel[i]] > max_score)
			{
				max_score = location_score[dracula_can_travel[i]];
				max_idx = dracula_can_travel[i];
			}
			//if best score is eqaul find the location that is far from all hunter

			else if (valid_location_p(dracula_can_travel[i]) && fabs(location_score[dracula_can_travel[i]] - max_score) < 0.01)
			{
				int this_min = 99999999;
				for (int j = 0; j < NUM_PLAYERS - 1; j++)
				{
					if (min_lenght[dracula_can_travel[i]] > min_lenght[max_idx])
					{
						max_idx = dracula_can_travel[i];
					}
					else
					{
						// printf("hunter to %d %d\n", j, path_lenght[j][max_idx]);
						// printf("hunter to %d %d\n", j, path_lenght[j][dracula_can_travel[i]]);
						int new_lose = 0;
						int old_lose = 0;
						if (path_lenght[j][max_idx] == min_lenght[max_idx])
							old_lose++;
						if (path_lenght[j][dracula_can_travel[i]] == min_lenght[dracula_can_travel[i]])
							new_lose++;
						if (new_lose < old_lose)
							max_idx = dracula_can_travel[i];
					}
				}
			}
			else if (dracula_can_travel[i] != max_idx && valid_location_p(dracula_can_travel[i]) && valid_location_p(max_idx) && location_get_type(dracula_can_travel[i]) == LAND && location_get_type(max_idx) == SEA && fabs(location_score[dracula_can_travel[i]] - max_score) <= 0.2)
			{
				max_idx = dracula_can_travel[i];
			}
		}
		best_move = max_idx;
		printf("move to %s %d\n", location_get_name(best_move), best_move);

		// if real location is from special skill
		for (int i = 0; i < n_locations; i++)
			if (best_move == dracula_can_travel[i])
				if (special_move[i] != UNKNOWN)
					best_move = special_move[i];

		for (int i = 0; i < n_locations; i++)
			if (valid_location_p(dracula_can_travel[i]))
				printf("%.2f %s\n", location_score[dracula_can_travel[i]], location_get_name(dracula_can_travel[i]));

		printf("max_lenght_of_minimum_distance: %d\n", max_lenght_of_minimum_distance);
		free(dracula_can_travel);
		for (int i = 0; i < NUM_PLAYERS - 1; i++)
			free(hunter_can_travel[i]);
	}

	//printf("best %s\t\t%.2f\n", location_get_name(best_move), location_score[best_move]);
	printf("move to %s %d\n", location_get_name(best_move), best_move);
	register_best_play(location_get_abbrev(best_move), "Dracula is moving!!!");
}

static bool isDeadEnd(DraculaView dv, location_t trail[], location_t old_trail[], location_t location, location_t special_move)
{
	int n_location;
	location_t *adj_location;
	int this_special_move = special_move;
	adj_location = check_for_connections_0_round(dv->world_map, location, true, true, &n_location);
	if (this_special_move == UNKNOWN)
	{
		for (int i = 0; i < TRAIL_SIZE - 1; i++)
		{
			if (old_trail[i] == HIDE || (old_trail[i] >= DOUBLE_BACK_1 && old_trail[i] <= DOUBLE_BACK_5))
			{
				return false;
			}
		}
	}
	else if (this_special_move == HIDE)
	{
		for (int i = 0; i < TRAIL_SIZE - 2; i++)
		{
			if (old_trail[i] >= DOUBLE_BACK_1 && old_trail[i] <= DOUBLE_BACK_5)
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	for (int i = 0; i < n_location; i++)
	{
		bool is_deadend = false;
		for (int j = 0; j < TRAIL_SIZE; j++)
		{
			if (adj_location[i] == trail[j])
			{
				is_deadend = true;
			}
		}
		if (!is_deadend)
		{
			return false;
		}
	}

	return true;
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

void findAllPathBFSwithoutTrailAvoidHunter(Map map, location_t src, int visited[], int path[], int trail[], int all_hunter_dest[])
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
			bool not_in_trail = true;
			for (int i = 0; i < TRAIL_SIZE; i++)
				if (trail[i] == curr->v)
					not_in_trail = false;

			if (visited[curr->v] == -1 && not_in_trail && all_hunter_dest[curr->v] == 0)
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

static bool can_go(location_t dracula_can_travel[], location_t city, int n_locations)
{
	for (int i = 0; i < n_locations; i++)
		if (dracula_can_travel[i] == city)
			return true;
	return false;
}

void dv_get_old_trail(DraculaView dv, location_t trail[TRAIL_SIZE])
{
	assert(dv != NULL);

	DLListNode *curr = dv->player_path[PLAYER_DRACULA]->last;
	for (size_t i = 0; i < TRAIL_SIZE; i++)
	{
		if (i < dv->player_path[PLAYER_DRACULA]->nitems)
		{
			trail[i] = curr->location;
			curr = curr->prev;
		}
		else
			trail[i] = -1;
	}
}


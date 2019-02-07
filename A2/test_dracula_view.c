////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_dracula_view.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dracula_view.h"

int main(void)
{
	do
	{ ////////////////////////////////////////////////////////////////
		puts("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		DraculaView dv = dv_new(trail, messages);

		assert(dv_get_round(dv) == 0);
		assert(dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(dv_get_score(dv) == GAME_START_SCORE);
		assert(dv_get_location(dv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts("passed");
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Test for basic functions, "
			 "just before Dracula's first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new(trail, messages);

		/*assert(dv_get_round(dv) == 0);
		assert(dv_get_location(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(dv_get_location(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(dv_get_location(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(dv_get_location(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(dv_get_location(dv, PLAYER_DRACULA) == UNKNOWN_LOCATION);
		assert(dv_get_health(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);*/

		puts("passed");
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED... SAO.... HCD.... MAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", ""};
		DraculaView dv = dv_new(trail, messages);

		assert(dv_get_round(dv) == 1);
		assert(dv_get_location(dv, PLAYER_DRACULA) == GENEVA);
		assert(dv_get_health(dv, PLAYER_LORD_GODALMING) == 5);
		assert(dv_get_health(dv, PLAYER_DRACULA) == 30);
		assert(dv_get_location(dv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		dv_get_trail(dv, PLAYER_DRACULA, history);
		assert(history[0] == GENEVA);
		assert(history[2] == UNKNOWN_LOCATION);

		dv_get_trail(dv, PLAYER_LORD_GODALMING, history);
		assert(history[0] == GENEVA);
		assert(history[1] == STRASBOURG);
		assert(history[2] == UNKNOWN_LOCATION);

		dv_get_trail(dv, PLAYER_DR_SEWARD, history);
		assert(history[0] == ATLANTIC_OCEAN);
		assert(history[1] == ATLANTIC_OCEAN);
		assert(history[2] == UNKNOWN_LOCATION);

		puts("passed");
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Test for Dracula leaving minions");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"};
		DraculaView dv = dv_new(trail, messages);

		int nT, nV;
		dv_get_locale_info(dv, EDINBURGH, &nT, &nV);
		assert(nT == 0 && nV == 1);
		dv_get_locale_info(dv, MANCHESTER, &nT, &nV);
		assert(nT == 1 && nV == 0);
		assert(dv_get_location(dv, PLAYER_DRACULA) == MANCHESTER);

		location_t history[TRAIL_SIZE];
		dv_get_trail(dv, PLAYER_DRACULA, history);
		assert(history[0] == MANCHESTER);
		assert(history[1] == EDINBURGH);
		assert(history[2] == UNKNOWN_LOCATION);

		dv_get_trail(dv, PLAYER_MINA_HARKER, history);
		assert(history[0] == STRASBOURG);
		assert(history[1] == STRASBOURG);
		assert(history[2] == GENEVA);
		assert(history[3] == UNKNOWN_LOCATION);

		puts("passed");
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Checking Galatz road connections");
		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		DraculaView dv = dv_new(trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player(
			dv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert(n_edges == 5);
		assert(seen[GALATZ]);
		assert(seen[CONSTANTA]);
		assert(seen[BUCHAREST]);
		assert(seen[KLAUSENBURG]);
		assert(seen[CASTLE_DRACULA]);

		puts("passed");
		free(edges);
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Checking Ionian Sea sea connections");
		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		DraculaView dv = dv_new(trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player(
			dv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert(n_edges == 7);
		assert(seen[IONIAN_SEA]);
		assert(seen[BLACK_SEA]);
		assert(seen[ADRIATIC_SEA]);
		assert(seen[TYRRHENIAN_SEA]);
		assert(seen[ATHENS]);
		assert(seen[VALONA]);
		assert(seen[SALONICA]);

		puts("passed");
		free(edges);
		dv_drop(dv);
	} while (0);

	do
	{ ////////////////////////////////////////////////////////////////
		puts("Checking Athens rail connections (none)");

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		DraculaView dv = dv_new(trail, messages);

		size_t n_edges;
		location_t *edges = dv_get_dests_player(
			dv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

		assert(n_edges == 1);
		assert(edges[0] == ATHENS);

		puts("passed");
		free(edges);
		dv_drop(dv);
	} while (0);

	do
	{
		puts("Samples from Game #0");
		char *trail =
			"GMN.... SPL.... HAM.... MPA.... DMA.V.. "
			"GLV.... SLO.... HNS.... MST.... DGR.V.. "
			"GLV.... SLO.... HNS.... MST.... DAL.V.. "
			"GLV.... SLO.... HNS.... MST.... DBA.V.. "
			"GLV.... SLO.... HNS.... MST.... DTO.V.. "
			"GLV.... SLO.... HNS.... MST.... DCF.V.. "
			"GLV.... SLO.... HNS.... MST.... DGE.T.. "
			"GLV.... SLO.... HNS.... MST.... DZU.T.. " 
			"GLV.... SLO.... HNS.... MST.... DMI.T.. "
			"GLV.... SLO.... HNS.... MST.... DMR.T.. "
			"GLV.... SLO.... HNS.... MST.... DMS.T..";
		player_message messages[] = {"", "", "", "", "", "", "", "", ""};
		DraculaView dv = dv_new(trail, messages);
		size_t n_locations;
		location_t *array = dv_get_dests (dv, &n_locations, true, true);
		for (int i=0;i<n_locations;i++){
			//printf("stuff in the array %s\n ",location_get_name(array[i]));
//
		}
	} while (0);

	do
	{
		puts("Samples from Game #1 PLAYER_VAN_HELSING DEAD AND DRACULA DEAD");
		char *trail =
			"GCA.... SCD.... HBI.... MGW.... DMA.V.. "
			"GMAVD.. SGA.... HRO.... MAO.... DALT... "
			"GMA.... SCN.... HTS.... MCA.... DGRT... "
			"GGRTD.. SBS.... HMS.... MGRD... DD1T... "
			"GCA.... SIO.... HALT... MCA.... DHIT... "
			"GCA.... STS.... HGRTTD.";
		player_message messages[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
		DraculaView dv = dv_new(trail, messages);

		assert(dv_get_round(dv) == 5);
		assert(dv_get_score(dv) == 355);

		assert(dv_get_health(dv, PLAYER_LORD_GODALMING) == 5); // REST 2 turn (+3) * 2 , DRACULA(-4) * 2 , TRAP(-2)
		assert(dv_get_health(dv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_VAN_HELSING) == 0); // TRAPS(-2) * 3 , DRACULA(-4)
		assert(dv_get_health(dv, PLAYER_MINA_HARKER) == 5); // DRACULA(-4)
		assert(dv_get_health(dv, PLAYER_DRACULA) == 0); 

		assert(dv_get_location(dv, PLAYER_LORD_GODALMING) == CADIZ);
		assert(dv_get_location(dv, PLAYER_DR_SEWARD) == TYRRHENIAN_SEA);
		assert(dv_get_location(dv, PLAYER_VAN_HELSING) == ST_JOSEPH_AND_ST_MARYS);
		assert(dv_get_location(dv, PLAYER_MINA_HARKER) == CADIZ);
		assert(dv_get_location(dv, PLAYER_DRACULA) == GRANADA);

		location_t start, end;
		dv_get_player_move(dv, PLAYER_LORD_GODALMING, &start, &end);
		assert(start == CADIZ && end == CADIZ);
		dv_get_player_move(dv, PLAYER_DR_SEWARD, &start, &end);
		assert(start == IONIAN_SEA && end == TYRRHENIAN_SEA);
		dv_get_player_move(dv, PLAYER_VAN_HELSING, &start, &end);
		assert(start == ALICANTE && end == GRANADA);
		dv_get_player_move(dv, PLAYER_MINA_HARKER, &start, &end);
		assert(start == GRANADA && end == CADIZ);
		dv_get_player_move(dv, PLAYER_DRACULA, &start, &end);
		assert(start == DOUBLE_BACK_1 && end == HIDE);

		int n_traps, n_vamps;
		dv_get_locale_info(dv, GRANADA, &n_traps, &n_vamps);
		assert(n_traps == 0 && n_vamps == 0);

		location_t history[TRAIL_SIZE];

		dv_get_trail(dv, PLAYER_LORD_GODALMING, history);
		assert(history[0] == CADIZ);
		assert(history[1] == CADIZ);
		assert(history[2] == GRANADA);
		assert(history[3] == MADRID);
		assert(history[4] == MADRID);
		assert(history[5] == CADIZ);

		dv_get_trail(dv, PLAYER_DR_SEWARD, history);
		assert(history[0] == TYRRHENIAN_SEA);
		assert(history[1] == IONIAN_SEA);
		assert(history[2] == BLACK_SEA);
		assert(history[3] == CONSTANTA);
		assert(history[4] == GALATZ);
		assert(history[5] == CASTLE_DRACULA);

		dv_get_trail(dv, PLAYER_VAN_HELSING, history);

		assert(history[0] == GRANADA);
		assert(history[1] == ALICANTE);
		assert(history[2] == MEDITERRANEAN_SEA);
		assert(history[3] == TYRRHENIAN_SEA);
		assert(history[4] == ROME);
		assert(history[5] == BARI);

		dv_get_trail(dv, PLAYER_MINA_HARKER, history);
		assert(history[0] == CADIZ);
		assert(history[1] == GRANADA);
		assert(history[2] == CADIZ);
		assert(history[3] == ATLANTIC_OCEAN);
		assert(history[4] == GALWAY);
		assert(history[5] == UNKNOWN_LOCATION);

		dv_get_trail(dv, PLAYER_DRACULA, history);
		assert(history[0] == GRANADA);
		assert(history[1] == GRANADA);
		assert(history[2] == GRANADA);
		assert(history[3] == ALICANTE);
		assert(history[4] == MADRID);
		assert(history[5] == UNKNOWN_LOCATION);

		size_t n_edges;
		location_t *edges;
		bool seen[NUM_MAP_LOCATIONS];

		edges = dv_get_dests_player(dv, &n_edges, PLAYER_DR_SEWARD, true, true, true);
		for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++)
			seen[i] = false;
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert(n_edges == 7);
		assert(seen[TYRRHENIAN_SEA]);
		assert(seen[MEDITERRANEAN_SEA]);
		assert(seen[IONIAN_SEA]);
		assert(seen[NAPLES]);
		assert(seen[ROME]);
		assert(seen[CAGLIARI]);
		assert(seen[GENOA]);
		free(edges);
		edges = dv_get_dests_player(dv, &n_edges, PLAYER_DRACULA, true, false, true);
		for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++)
			seen[i] = false;
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;
		assert(n_edges == 2);
		assert(!seen[ALICANTE]);
		assert(!seen[MADRID]);
		assert(seen[CADIZ]);
		assert(seen[GRANADA]);
		free(edges);
		puts("passed");
		dv_drop(dv);
	} while (0);

	do
	{

		puts("Samples from Game #2 : DRACULA REST IN CASTLE");
		char *trail =
			"GMN.... SPL.... HAM.... MPA.... DGA.V.. "
			"GLV.... SLO.... HNS.... MST.... DHIT... "
			"GIR.... SPL.... HAO.... MZU.... DCDT... "
			"GSW.... SLO.... HNS.... MFR.... DKLT... "
			"GLV.... SPL.... HAO.... MZU.... DBCT... "
			"GSW.... SLO.... HNS.... MMR....";
		player_message messages[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
		DraculaView dv = dv_new(trail, messages);

		assert(dv_get_round(dv) == 5);
		assert(dv_get_score(dv) == GAME_START_SCORE - dv_get_round(dv));

		assert(dv_get_health(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(dv_get_health(dv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);

		assert(dv_get_health(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS + LIFE_GAIN_CASTLE_DRACULA);

		assert(dv_get_location(dv, PLAYER_LORD_GODALMING) == SWANSEA);
		assert(dv_get_location(dv, PLAYER_DR_SEWARD) == LONDON);
		assert(dv_get_location(dv, PLAYER_VAN_HELSING) == NORTH_SEA);
		assert(dv_get_location(dv, PLAYER_MINA_HARKER) == MARSEILLES);
		assert(dv_get_location(dv, PLAYER_DRACULA) == BUCHAREST);

		location_t start, end;
		dv_get_player_move(dv, PLAYER_LORD_GODALMING, &start, &end);
		assert(start == LIVERPOOL && end == SWANSEA);
		dv_get_player_move(dv, PLAYER_DR_SEWARD, &start, &end);
		assert(start == PLYMOUTH && end == LONDON);
		dv_get_player_move(dv, PLAYER_VAN_HELSING, &start, &end);
		assert(start == ATLANTIC_OCEAN && end == NORTH_SEA);
		dv_get_player_move(dv, PLAYER_MINA_HARKER, &start, &end);
		assert(start == ZURICH && end == MARSEILLES);
		dv_get_player_move(dv, PLAYER_DRACULA, &start, &end);
		assert(start == KLAUSENBURG && end == BUCHAREST);

		int n_traps, n_vamps;
		dv_get_locale_info(dv, GALATZ, &n_traps, &n_vamps);
		assert(n_traps == 1 && n_vamps == 1);

		dv_get_locale_info(dv, CASTLE_DRACULA, &n_traps, &n_vamps);
		assert(n_traps == 1 && n_vamps == 0);

		dv_get_locale_info(dv, KLAUSENBURG, &n_traps, &n_vamps);
		assert(n_traps == 1 && n_vamps == 0);

		dv_get_locale_info(dv, BUCHAREST, &n_traps, &n_vamps);
		assert(n_traps == 1 && n_vamps == 0);

		location_t history[TRAIL_SIZE];

		dv_get_trail(dv, PLAYER_DRACULA, history);
		assert(history[0] == BUCHAREST);
		assert(history[1] == KLAUSENBURG);
		assert(history[2] == CASTLE_DRACULA);
		assert(history[3] == GALATZ);
		assert(history[4] == GALATZ);
		assert(history[5] == UNKNOWN_LOCATION);

		puts("passed");
		dv_drop(dv);
	} while (0);

	return EXIT_SUCCESS;
}


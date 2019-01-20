////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 0);

		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
        assert (gv_get_health (gv, PLAYER_VAN_HELSING) == GAME_START_HUNTER_LIFE_POINTS);
        assert (gv_get_health (gv, PLAYER_MINA_HARKER) == GAME_START_HUNTER_LIFE_POINTS);
        assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		assert (gv_get_score (gv) == GAME_START_SCORE);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 1);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);

		
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (gv_get_location (gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score (gv) == 365);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",  "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_location (gv, PLAYER_DRACULA) == GENEVA);
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5);
		assert (gv_get_health (gv, PLAYER_DRACULA) == 30);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == GENEVA);
		assert (gv_get_score (gv) == 365);
		
		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);
		assert (history[3] == UNKNOWN_LOCATION);
		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);
		assert (history[2] == UNKNOWN_LOCATION);
		assert (history[3] == UNKNOWN_LOCATION);
		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Dracula View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for connections");
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		do {
			puts ("Checking Galatz road connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				GALATZ, PLAYER_LORD_GODALMING, 0,
				true, false, false
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 5);
			assert (seen[GALATZ]);
			assert (seen[CONSTANTA]);
			assert (seen[BUCHAREST]);
			assert (seen[KLAUSENBURG]);
			assert (seen[CASTLE_DRACULA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Ionian Sea sea connections");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 7);
			assert (seen[IONIAN_SEA]);
			assert (seen[BLACK_SEA]);
			assert (seen[ADRIATIC_SEA]);
			assert (seen[TYRRHENIAN_SEA]);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);
			assert (seen[SALONICA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Athens rail connections (none)");
			size_t n_edges;
			location_t *edges = gv_get_connections (
				gv, &n_edges,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false
			);

			assert (n_edges == 1);
			assert (edges[0] == ATHENS);

			free (edges);
		} while (0);
		 do {
            puts ("Checking Athens road connections");
			size_t n_edges;
           location_t *edges  = gv_get_connections (gv, &n_edges,
                ATHENS, PLAYER_LORD_GODALMING, 0,
                true, true, false);

            assert (n_edges == 2);
           	bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 2);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);

            free (edges);
        } while (0);
		
		do {
            puts ("Checking Santander road connections");
			size_t n_edges;
           location_t *edges  = gv_get_connections (gv, &n_edges,
                SANTANDER, PLAYER_DR_SEWARD, 16,
                true, true, true);

            
           	bool seen[NUM_MAP_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++)
				seen[edges[i]] = true;

			assert (n_edges == 5);
			assert (seen[SANTANDER]);
			assert (seen[LISBON]);
			assert (seen[MADRID]);
			assert (seen[SARAGOSSA]);
			assert (seen[BAY_OF_BISCAY]);
            free (edges);
        } while (0);
		
		 do {
            puts ("Checking Galatz rail only connections up to 1 away");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                GALATZ, PLAYER_LORD_GODALMING, 17, false, true, false);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert(n_edges == 2);
            assert (seen[GALATZ]);
            assert (seen[BUCHAREST]);

            free (edges);
        } while (0);
		
		 do {
            puts ("Checking Galatz rail only connections up to 1 away_0");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                GALATZ, PLAYER_LORD_GODALMING, 17, true, true, false);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert (n_edges == 5);
            assert (seen[GALATZ]);
            assert (seen[CASTLE_DRACULA]);
			assert (seen[KLAUSENBURG]);
			assert (seen[BUCHAREST]);
			assert (seen[CONSTANTA]);
		
            free (edges);
        } while (0);


		 do {
            puts ("Checking Galatz Naples only connections up to 1 away_1");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                NAPLES, PLAYER_LORD_GODALMING, 1,true, true, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert(n_edges == 4);
            assert (seen[ROME]);
            assert (seen[NAPLES]);
			assert (seen[BARI]);
			assert (seen[TYRRHENIAN_SEA]);
            free (edges);
        } while (0);

		do {
            puts ("Checking Santander rail only connections up to 2 away");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                SANTANDER, PLAYER_LORD_GODALMING, 18, false, true, false);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert(n_edges == 5);
            assert (seen[SANTANDER]);
            assert (seen[MADRID]);
			assert (seen[SARAGOSSA]);
			assert (seen[LISBON]);
			assert (seen[ALICANTE]);

            free (edges);
        } while (0);

		do {
            puts ("Checking Santander rail only connections up to 2 away_0");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                SANTANDER, PLAYER_LORD_GODALMING, 18, false, false, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert (n_edges == 2);
            assert (seen[SANTANDER]);
           	assert (seen[BAY_OF_BISCAY]);


            free (edges);
        } while (0);


		do {
            puts ("Checking Santander rail only connections up to 2 away_1");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                SANTANDER, PLAYER_LORD_GODALMING, 18, true, true, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (size_t i = 0; i < n_edges; i++)
                seen[edges[i]] = true;

            assert(n_edges == 6);
            assert (seen[SANTANDER]);
            assert (seen[MADRID]);
			assert (seen[SARAGOSSA]);
			assert (seen[LISBON]);
			assert (seen[ALICANTE]);
			assert (seen[BAY_OF_BISCAY]);

            free (edges);
        } while (0);
		do {
            puts ("Checking Galatz rail only connections up to 3 away_0");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                GALATZ, PLAYER_LORD_GODALMING, 19, false, true, false);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (int i = 0; i < n_edges; i++){
                seen[edges[i]] = true;

			}
            assert (n_edges == 6);
            assert (seen[GALATZ]);
            assert (seen[BUCHAREST]);
            assert (seen[SZEGED]);
            assert (seen[CONSTANTA]);
            assert (seen[BELGRADE]);
            assert (seen[BUDAPEST]);

            free (edges);
        } while (0);
		
		do {
            puts ("Checking Galatz rail only connections up to 3 away_1");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                GALATZ, PLAYER_MINA_HARKER, 20, true, true, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (int i = 0; i < n_edges; i++){
                seen[edges[i]] = true;

			}
			
            assert (n_edges == 8);
            assert (seen[GALATZ]);
            assert (seen[BUCHAREST]);
            assert (seen[SZEGED]);
            assert (seen[CONSTANTA]);
            assert (seen[BELGRADE]);
            assert (seen[BUDAPEST]);
			assert (seen[CASTLE_DRACULA]);
			assert (seen[KLAUSENBURG]);	
            free (edges);
        } while (0);


		do {
            puts ("Checking Galatz rail only connections up to 3 away_2");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                LE_HAVRE, PLAYER_VAN_HELSING, 29, false, true, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (int i = 0; i < n_edges; i++){
                seen[edges[i]] = true;
				//printf("%s\n",location_get_name (edges[i]));

			}
			//printf("%d\n",n_edges);
            assert (n_edges == 8);
            assert (seen[PARIS]);
            assert (seen[LE_HAVRE]);
            assert (seen[BORDEAUX]);
            assert (seen[SARAGOSSA]);
            assert (seen[MARSEILLES]);
            assert (seen[BRUSSELS]);
			assert (seen[ENGLISH_CHANNEL]);
			assert (seen[COLOGNE]);
            free (edges);
        } while (0);

		do {
            puts ("Checking draculas connections_0");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                LE_HAVRE, PLAYER_DRACULA, 31, true, true, true);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (int i = 0; i < n_edges; i++){
                seen[edges[i]] = true;
				//printf("%s\n",location_get_name (edges[i]));

			}
			//printf("%d\n",n_edges);
            assert (n_edges == 5);
            assert (seen[PARIS]);
            assert (seen[LE_HAVRE]);
        

            assert (seen[NANTES]);
            assert (seen[BRUSSELS]);
			assert (seen[ENGLISH_CHANNEL]);
	
            free (edges);
        } while (0);	
		
		do {
            puts ("Checking draculas connections_no_hospitals");
			size_t n_edges;
            location_t *edges = gv_get_connections (gv, &n_edges,
                SZEGED, PLAYER_DRACULA, 100000, true, false, false);

            bool seen[NUM_MAP_LOCATIONS] = {false};
            for (int i = 0; i < n_edges; i++){
                seen[edges[i]] = true;
				//printf("%s\n",location_get_name (edges[i]));

			}
			//printf("%d\n",n_edges);
            assert (n_edges == 5);
            assert (seen[BUDAPEST]);
            assert (seen[ZAGREB]);
        

            assert (seen[BELGRADE]);
            assert (seen[KLAUSENBURG]);
			assert (seen[SZEGED]);
	
            free (edges);
        } while (0);	

		
		puts ("passed");
		gv_drop (gv);
	} while (0);
 //// Extra blackbox test cases
	  do {
        puts("Checking for potential errors based on pastPlays");
        char *trail = "GMN.... SPL.... HAM.... MPA.... DC?.V.. " 
                "GLV.... SLO.... HNS.... MST.... DC?T... GIR....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);
		assert(gv_get_player (gv) == PLAYER_DR_SEWARD);
		for(int i = PLAYER_LORD_GODALMING; i < PLAYER_DRACULA; i++){
           assert(gv_get_health(gv, i) == GAME_START_HUNTER_LIFE_POINTS);
        }
		assert(gv_get_score(gv) == GAME_START_SCORE-2);
       
        assert(gv_get_location(gv, PLAYER_LORD_GODALMING) == IRISH_SEA);
        assert(gv_get_location(gv, PLAYER_DR_SEWARD) == LONDON);
        assert(gv_get_location(gv, PLAYER_VAN_HELSING) == NORTH_SEA);
        assert(gv_get_location(gv, PLAYER_MINA_HARKER) == STRASBOURG);
        assert(gv_get_location(gv, PLAYER_DRACULA) == CITY_UNKNOWN);

        assert(gv_get_round(gv) == 2);
       
        location_t history[TRAIL_SIZE];
        gv_get_history(gv, PLAYER_LORD_GODALMING, history);
        assert(history[0] == IRISH_SEA);
        assert(history[1] == LIVERPOOL);
        assert(history[2] == MANCHESTER);
        assert(history[3] == UNKNOWN_LOCATION);
        assert(history[4] == UNKNOWN_LOCATION);
        assert(history[5] == UNKNOWN_LOCATION);
        
        gv_get_history(gv, PLAYER_DR_SEWARD, history);
        assert(history[0] == LONDON);
        assert(history[1] == PLYMOUTH);
        assert(history[2] == UNKNOWN_LOCATION);
        assert(history[3] == UNKNOWN_LOCATION);
        assert(history[4] == UNKNOWN_LOCATION);
        assert(history[5] == UNKNOWN_LOCATION);
        
        gv_get_history(gv, PLAYER_VAN_HELSING, history);
        assert(history[0] == NORTH_SEA);
        assert(history[1] == AMSTERDAM);
        assert(history[2] == UNKNOWN_LOCATION);
        assert(history[3] == UNKNOWN_LOCATION);
        assert(history[4] == UNKNOWN_LOCATION);
        assert(history[5] == UNKNOWN_LOCATION);
        
        gv_get_history(gv, PLAYER_MINA_HARKER, history);
        assert(history[0] == STRASBOURG);
        assert(history[1] == PARIS);
        assert(history[2] == UNKNOWN_LOCATION);
        assert(history[3] == UNKNOWN_LOCATION);
        assert(history[4] == UNKNOWN_LOCATION);
        assert(history[5] == UNKNOWN_LOCATION);

        gv_get_history(gv, PLAYER_DRACULA, history);
        assert(history[0] == CITY_UNKNOWN);
        assert(history[1] == CITY_UNKNOWN);
        assert(history[2] == UNKNOWN_LOCATION);
        assert(history[3] == UNKNOWN_LOCATION);
        assert(history[4] == UNKNOWN_LOCATION);
        assert(history[5] == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	  } while(0);
	

	do {
        puts("Testing with a more complicated trail");
        char *trail = 
            "GED.... SGE.... HZU.... MCA.... DCF.V.. "
            "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
            "GLO.... SMR.... HCF.... MMA.... DTOT... "
            "GPL.... SMS.... HMR.... MGR.... DBAT... "
            "GLO.... SBATD.. HMS.... MMA.... DSRT... "
            "GPL.... SSJ.... HBA.... MGR.... DALT... "
            "GPL.... SSJ.... HBA.... MGR.... DMAT... "
            "GLO.... SBE.... HMS.... MMATD..";

       	player_message messages[] = 
        {"", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", ""};
        
        GameView gv = gv_new(trail, messages);
        assert(gv);
        assert(gv_get_round(gv) == 7);
        assert(gv_get_score(gv) == GAME_START_SCORE -7*SCORE_LOSS_DRACULA_TURN
                - SCORE_LOSS_HUNTER_HOSPITAL);
        //printf("the lord_health is %d\n",(gv_get_health(gv, PLAYER_LORD_GODALMING)));
        assert(gv_get_health(gv, PLAYER_LORD_GODALMING)== GAME_START_HUNTER_LIFE_POINTS);
        assert(gv_get_health(gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
        assert(gv_get_health(gv, PLAYER_VAN_HELSING)== GAME_START_HUNTER_LIFE_POINTS);
        assert(gv_get_health(gv, PLAYER_MINA_HARKER)== GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER
                - LIFE_LOSS_TRAP_ENCOUNTER);
        assert(gv_get_health(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 3*LIFE_LOSS_HUNTER_ENCOUNTER);

        assert(gv_get_location(gv, PLAYER_LORD_GODALMING) == LONDON);
        assert(gv_get_location(gv, PLAYER_DR_SEWARD) == BELGRADE);
        assert(gv_get_location(gv, PLAYER_VAN_HELSING) == MEDITERRANEAN_SEA);
        assert(gv_get_location(gv, PLAYER_MINA_HARKER) == MADRID);
        assert(gv_get_location(gv, PLAYER_DRACULA) == MADRID);

        location_t history[TRAIL_SIZE];

        gv_get_history(gv, PLAYER_LORD_GODALMING, history);
        assert(history[0] == LONDON);
        assert(history[1] == PLYMOUTH);
        assert(history[2] == PLYMOUTH);
        assert(history[3] == LONDON);
        assert(history[4] == PLYMOUTH);
        assert(history[5] == LONDON);

        gv_get_history(gv, PLAYER_DR_SEWARD, history);
        assert(history[0] == BELGRADE);
        assert(history[1] == SARAJEVO);
        assert(history[2] == SARAJEVO);
        assert(history[3] == BARCELONA);
        assert(history[4] == MEDITERRANEAN_SEA);
        assert(history[5] == MARSEILLES);

        gv_get_history(gv, PLAYER_VAN_HELSING, history);
        assert(history[0] == MEDITERRANEAN_SEA);
        assert(history[1] == BARCELONA);
        assert(history[2] == BARCELONA);
        assert(history[3] == MEDITERRANEAN_SEA);
        assert(history[4] == MARSEILLES);
        assert(history[5] == CLERMONT_FERRAND);
        
        gv_get_history(gv, PLAYER_MINA_HARKER, history);
        assert(history[0] == MADRID);
        assert(history[1] == GRANADA);
        assert(history[2] == GRANADA);
        assert(history[3] == MADRID);
        assert(history[4] == GRANADA);
        assert(history[5] == MADRID);
        
        gv_get_history(gv, PLAYER_DRACULA, history);
        assert(history[0] == MADRID);
        assert(history[1] == ALICANTE);
        assert(history[2] == SARAGOSSA);
        assert(history[3] == BARCELONA);
        assert(history[4] == TOULOUSE);
        assert(history[5] == BORDEAUX);

     
		puts ("passed");
		gv_drop (gv);
    } while(0);


	do {
		puts("Testing with a more complicated trail");
	char *trail = 
            "GED.... SGE.... HZU.... MCA.... DCF.V.. "
            "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
            "GLO.... SMR.... HCF.... MMA.... DTOT.. "
            "GPL.... SMS.... HMR.... MGR.... DBAT... "
            "GLO.... SBATD.. HMS.... MMA.... DSRT... "
            "GPL.... SSJ.... HBA.... MGR.... DALT... "
            "GPL.... SSJ.... HBA.... MGR.... DMAT... "
            "GLO.... SBE.... HMS.... MMATD..";

       	player_message messages[] = 
     {"", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", "", "", 
            "", "", "", ""};			



	}while(0);
	return EXIT_SUCCESS;
}

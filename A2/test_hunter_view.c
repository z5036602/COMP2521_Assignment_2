////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_hunter_view.c: test the HunterView ADT
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

#include "hunter_view.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");
		char *trail = "";
		player_message messages[] = {};
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_LORD_GODALMING);
		assert (hv_get_round (hv) == 0);
		assert (hv_get_health (hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (hv_get_score (hv) == GAME_START_SCORE);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_LORD_GODALMING);
		assert (hv_get_round (hv) == 1);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (hv_get_location (hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (hv_get_location (hv, PLAYER_VAN_HELSING) == ZURICH);
		assert (hv_get_location (hv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (hv_get_location (hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (hv_get_health (hv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");
		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_location (hv, PLAYER_DRACULA) == GENEVA);
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 5);
		assert (hv_get_health (hv, PLAYER_DRACULA) == 30);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		hv_get_trail (hv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		hv_get_trail (hv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go" };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == 0);
		assert (hv_get_location (hv, PLAYER_DRACULA) == DOUBLE_BACK_1);
		assert (hv_get_health (hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Galatz road connections");
		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++)
			seen[edges[i]] = true;

		assert (n_edges == 5);
		assert (seen[GALATZ]);
		assert (seen[CONSTANTA]);
		assert (seen[BUCHAREST]);
		assert (seen[KLAUSENBURG]);
		assert (seen[CASTLE_DRACULA]);

		puts ("passed");
		free (edges);
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Ionian Sea sea connections");
		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
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

		puts ("passed");
		free (edges);
		hv_drop (hv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Athens rail connections (none)");

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		HunterView hv = hv_new (trail, messages);

		size_t n_edges;
		location_t *edges = hv_get_dests_player (
			hv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

		assert (n_edges == 1);
		assert (edges[0] == ATHENS);

		puts ("passed");
		free (edges);
		hv_drop (hv);
	} while (0);
    
//    /////////////////////////////////////////////////////////
//   -----------------written test----------------------------
//   /////////////////////////////////////////////////////////

 
    do { //////////////////////////////////////////////////
		puts ("Test1 for two trap that hunter encounter");

		char *trail =
			"GMU.... SCF.... HBC.... MED.... DC?.V.. " //SZ
			"GVI.... SMR.... HCN.... MNS.... DBDT... " //BD
			"GZA.... SMS.... HVR.... MHA.... DBDT... " //BD HIDE
			"GBDTTD. SST.... HSO.... MLI.... DC?T... " //VI
			"GBD.... ";

		player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            ""
        };

		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_DR_SEWARD);
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 4);
		assert (hv_get_health (hv, PLAYER_DRACULA) == 30);
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == BUDAPEST);
		assert (hv_get_score (hv) == 366 - 4);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == BUDAPEST);
		assert (history[1] == BUDAPEST);
		assert (history[2] == ZAGREB);
		assert (history[3] == VIENNA);
		assert (history[4] == MUNICH);

		hv_get_trail (hv, PLAYER_DR_SEWARD, history);
		assert (history[0] == STRASBOURG);
		assert (history[1] == MEDITERRANEAN_SEA);
		assert (history[2] == MARSEILLES);
		assert (history[3] == CLERMONT_FERRAND);

		puts ("passed");
		hv_drop (hv);

	} while (0);

	do {////////////////////////////////////////////////////////
		puts ("Test2 for when a hunter died");
		
		char *trail  = 
			"GZU.... SMU.... HNU.... MBD.... DC?.V.. " //CF
			"GGE.... SMI.... HFR.... MZA.... DMRT... " //MR
			"GMRTD.. SMRD... HST.... MMU.... DS?.... " //CF
			"GMS.... SMS.... HGE.... MMI.... DC?T... " //BA
			"GBATD.. SCG.... HZU... ";

		player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", ""
        };

		HunterView hv = hv_new (trail, messages);

		assert (hv_get_player (hv) == PLAYER_MINA_HARKER);
		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 0);
		assert (hv_get_health (hv, PLAYER_DRACULA) == 8);
		
		assert (hv_get_location (hv, PLAYER_LORD_GODALMING) == ST_JOSEPH_AND_ST_MARYS);
		assert (hv_get_score (hv) == 366 - 4 - 6);

		location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_LORD_GODALMING, history);
		/*
		int i = 0;
		while (i < TRAIL_SIZE) {
		    char* loc = location_get_name (history[i]);
		    printf("%s\n", loc);
		    i++;
		}
		*/
		assert (history[0] == BARCELONA);
		assert (history[1] == MEDITERRANEAN_SEA);

		puts ("passed");
		hv_drop (hv);

	} while (0);
	
	do {////////////////////////////////////////////////////////
		puts ("Test3 for immature vampire mature and when hunter health full rest");
		
		char *trail  = 
			"GZU.... SMU.... HNU.... MBD.... DC?.V.. " //CF 	0
			"GGE.... SMI.... HFR.... MZA.... DMRT... " //MR 	1
			"GMRTD.. SMRD... HST.... MMU.... DS?.... " //MS 	2
			"GMS.... SMS.... HGE.... MMI.... DC?T... " //BA 	3
			"GBATD.. SCG.... HZU.... MMR.... DD1.... " //MS  4 double back to sea 
			"GZA.... SNP.... HGE.... MPA.... DC?T... " //AL 	5
			"GMU.... SNP.... HMI.... MNA.... DC?T... " //MA 	6 vampire mature
			"GBR.... SNP.... HMI.... MBB.... DC?T.V. ";//SN     7
        player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "",
        };
		HunterView hv = hv_new (trail, messages);

		assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 9);
		assert (hv_get_health (hv, PLAYER_DR_SEWARD) == 9);
		//int s = hv_get_score (hv);
		//printf("s = %d\n", s);
		assert (hv_get_score (hv) == 366 - 13 - 6 - 8);
		int h = hv_get_health (hv, PLAYER_DRACULA);
		//printf("%d\n", h);
		assert (hv_get_health (hv, PLAYER_DRACULA) == 8);

		puts ("passed");
		hv_drop (hv);
		
	} while (0);
	//Test for encountering Dracula and hunter history
//passed
//Test for Dracula doubling back at sea, and losing blood points (Hunter View)
//passed
//Test for Dracula doubling back at sea, and losing blood points (Dracula View)
//passed
//Test for connections
//Checking Galatz road connections
//Checking Ionian Sea sea connections

	
	do {////////////////////////////////////////////////////////
		puts ("Test4 for double back when it's dracula castle");

		char *trail  = 
			"GKL.... SMS.... HBO.... MAO.... DC?.V.. " //SO
			"GBC.... STS.... HCF.... MEC.... DBCT... " //BC
			"GBCTD.. SIO.... HGE.... MLE.... DC?T... " //GA
			"GVR.... SVA.... HZU.... MPA.... DCD.... " //CD
			"GCN.... SSJ.... HMU.... MST.... DC?T... " //KL
			"GGA.... SJM.... HVI.... MNU.... DC?T... " //BE
			"GBC.... SBETD.. HBD.... MSZ.... DD3...." ;//CD

		player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
        };
        HunterView hv = hv_new (trail, messages);
        int h = hv_get_health (hv, PLAYER_DRACULA);
        assert (hv_get_health (hv, PLAYER_DRACULA) == 40);
        //assert (hv_get_location (hv, PLAYER_DRACULA) == CASTLE_DRACULA);

        location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		/*int i = 0;
		while (i < TRAIL_SIZE) {
		    char* loc = location_get_name (history[i]);
		    printf("%s\n", loc);
		    i++;
		}*/
		//assert (history[0] == CASTLE_DRACULA);
		assert (history[1] == CITY_UNKNOWN);
		assert (history[3] == CASTLE_DRACULA);

		puts ("passed");
		hv_drop (hv);

	} while (0);
	
	 do { //////////////////////////////////////////////////
		puts ("Test5 for three trap that hunter encounter");

		char *trail =
			"GMU.... SCF.... HBC.... MED.... DC?.V.. " //SZ
			"GVI.... SMR.... HCN.... MNS.... DC?T... " //BD
			"GZA.... SMS.... HVR.... MHA.... DHIT... " //BD HIDE
			"GZA.... SCG.... HSO.... MBR.... DD2T... "
			"GBDTTTD ";
	    
	    player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            ""
        };
	    HunterView hv = hv_new (trail, messages);
	    
	    assert (hv_get_health (hv, PLAYER_LORD_GODALMING) == 0);
	    
	    assert (hv_get_score (hv) == 366 - 4 - 6); 
	    
	    puts ("passed");
		hv_drop (hv);
	} while (0);
	
	do {////////////////////////////////////////////////////////
	    puts ("Test6 for double back to hide");
	    
	    char *trail = 
	        "GCF.... SVE.... HBD.... MLE.... DC?.V.. "
	        "GPA.... SGO.... HKL.... MBU.... DCDT... "
	        "GST.... SVE.... HBC.... MCO.... DC?T... "
	        "GZU.... SFL.... HCN.... MLI.... DD2T... ";
	    
	    player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",            
        }; 
        
        HunterView hv = hv_new (trail, messages);
        int i = hv_get_health (hv, PLAYER_DRACULA);
        assert (hv_get_health (hv, PLAYER_DRACULA) == 60);
        
        assert (hv_get_location (hv, PLAYER_DRACULA) == CASTLE_DRACULA);
        
        location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		
		assert (history[0] == DOUBLE_BACK_2);
		assert (history[2] == CASTLE_DRACULA);
		puts ("passed");
        
    } while (0);
    
    do {/////////////////////////////////////////////////////////
        puts("Test7 for double back to tp");
        
        char *trail = 
            "GCF.... SVE.... HBD.... MLE.... DC?.V.. "
            "GPA.... SGO.... HKL.... MBU.... DTPT... "
            "GST.... SVE.... HBC.... MCO.... DC?T... "
            "GIOTD.. SKL.... HMA.... MNEDVTT DC?T... "
            "GSO.... SSW.... HVA.... MNE.... DD3.... ";
            
         player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", ""           
        };
        
        HunterView hv = hv_new (trail, messages);
        int i = hv_get_health (hv, PLAYER_DRACULA);
        assert (hv_get_health (hv, PLAYER_DRACULA) == 40);
        assert (hv_get_score (hv) == 366 -5 );
        assert (hv_get_health (hv,PLAYER_MINA_HARKER) == 4);
        
        location_t history[TRAIL_SIZE];
		hv_get_trail (hv, PLAYER_DRACULA, history);
		
		assert (history[0] == DOUBLE_BACK_3);
		assert (history[3] == TELEPORT);
		puts ("passed");
        
    } while (0);
    
    do {////////////////////////////////////////////////////////
        puts ("Test8 for double back to sea");
        
        char *trail = 
            "GKL.... SMS.... HBO.... MAO.... DC?.V.. " //SO
			"GBC.... STS.... HCF.... MEC.... DS?T... " //BC
			"GBCTD.. SIO.... HGE.... MLE.... DC?T... " //GA
			"GVR.... SVA.... HZU.... MPA.... DCD.... " //CD
			"GCN.... SSJ.... HMU.... MST.... DC?T... " //KL
			"GGA.... SJM.... HVI.... MNU.... DC?T... " //BE
			"GBC.... SBETD.. HBD.... MSZ.... DD5...." ;//CD
	    
	    player_message messages[] =
        {	"", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
            "", "", "", "", "",
        };
        
        HunterView hv = hv_new (trail, messages);
        int i = hv_get_health (hv, PLAYER_DRACULA);
        //printf ("%d\n", i);
        assert (hv_get_health (hv, PLAYER_DRACULA) == 40 - 2 - 10 - 10 - 2 + 10);
		puts ("passed");
        
    } while (0);
	    	return EXIT_SUCCESS;
}

/*
 * Tutorial tuebanjo15 Bernice Chen
 * Name: Stephanie Chua & Louis Cheung
 * runAI.c
 * Runs the game with interactive player instead of an AI
 * Compile with gcc -Wall -Werror -O -o runAI runAI.c Game.c
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 #include "Game.h"

//Maximum values of items
#define MAX_KPIPTS 150
#define MAX_G08S 8
#define MAX_STU_TYPES 6
#define NO_PLAYERS 3

action decideAction (Game g);
action actionPrompt (Game g);
static int rollDice (void);

int main (int argc, char * argv[]) {

	int disciplines[19] = {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, \
		STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, \
		STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, \
		STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, \
		STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS};

	int dice[19] = {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5};

	Game g = newGame (disciplines, dice);
	action a;
	int score = 0; //This stores the KPI points of current player 
	int curPlayer = NO_ONE; 
	int curTurn = 0; 
	int playerKPI = 0;
	int numStudents[MAX_STU_TYPES];
	int legal = 0;
	int diceScore = 0; 
	int spinoffDice = 0;
	//int discipline = 0;
	int counter = 0; 

	printf ("New game created, variables are initialized.\n");
	printf ("Let the game begin!\n\n");

	while (score < MAX_KPIPTS) {
		
		//To simulate an actual real life dice, we throw two dice 
		diceScore = rollDice();
		throwDice (g, diceScore);
		curTurn = getTurnNumber (g);
		curPlayer = getWhoseTurn (g);
		printf ("Turn number: %d\n", curTurn);
		printf ("Player %d's turn. Rolling dice.\n", curPlayer);
		printf ("The two dice rolled a %d! \n", diceScore);

		printf ("You currently have: \n");
		counter = 0;
		while (counter < MAX_STU_TYPES) {
			numStudents[counter] = getStudents (g, curPlayer, counter);
			printf ("    %d: %d \n", counter, numStudents[counter]);
			counter ++;
		}

		a = decideAction (g);
		printf ("You chose move %d.\n", a.actionCode);
		legal = isLegalAction (g, a);

		while (a.actionCode != PASS) {

			while (legal != TRUE) {
				printf ("The chosen action is illegal, try again.\n");
				a = decideAction (g);
				printf ("You chose move %d.\n", a.actionCode);
				legal = isLegalAction (g, a);
				printf ("Legal here is.. %d.\n", legal);
			}

			if (a.actionCode == START_SPINOFF) {
				//Ensures dice returns 1, 2 or 3.
				spinoffDice = (rand()%3 + 1);
				if (spinoffDice == 1) {
					a.actionCode = OBTAIN_IP_PATENT;
				} else {
					a.actionCode = OBTAIN_PUBLICATION;
				}
			}
 
			makeAction (g, a);
			printf("Action %d made!\n", a.actionCode);

			printf ("Your current stats:\n");
			printf ("    ARC Grants: %d\n", getARCs (g, curPlayer));
			printf ("    No of Campuses: %d\n", getCampuses (g, curPlayer));
			printf ("    No of G08s: %d\n", getGO8s (g, curPlayer));
			printf ("    No of Pubs: %d\n", getPublications (g, curPlayer));
			printf ("    No of IP Patents: %d\n", getIPs (g, curPlayer));
			printf ("    No of Students: \n");

			counter = 0;
			while (counter < MAX_STU_TYPES) {
				numStudents[counter] = getStudents (g, curPlayer, counter);
				printf ("       %d: %d \n", counter, numStudents[counter]);
				counter ++;
			}

			playerKPI = getKPIpoints (g, curPlayer);
			printf ("Player %d KPI Score: %d\n\n", curPlayer, score);
		}

		printf ("Next player's turn.\n\n");

	}

	printf ("Congratulations, player %d won.\n", curPlayer);
	printf ("Game completed. Disposing game.\n");
	
	disposeGame (g);

	return EXIT_SUCCESS;
}

action actionPrompt (Game g) {

	char *position = NULL;
	action a;

	printf ("Choose an action from the following:\n");
	printf ("Enter 0 - PASS\n");
	printf ("Enter 1 - Build a Campus\n");
	printf ("Enter 2 - Upgrade to a G08!\n");
	printf ("Enter 3 - Get an ARC.\n");
	printf ("Enter 4 - Start a business spinoff~\n");
	printf ("Enter 7 - Retrain your students.\n Input: ");
	scanf ("%d\n", &a.actionCode);
	printf ("You chose %d\n", a.actionCode);

	if (a.actionCode == OBTAIN_ARC 
		|| a.actionCode == BUILD_CAMPUS
		|| a.actionCode == BUILD_GO8) {
		printf ("Please input the position of your structure: ");
		scanf ("%s", position);
		strcpy (a.destination, position);
	} else if (a.actionCode == RETRAIN_STUDENTS) {
		printf ("Retraining Students initiated!\n");
		printf ("Enter 0 - THD......\n");
		printf ("Enter 1 - BPS\n");
		printf ("Enter 2 - BQN\n");
		printf ("Enter 3 - MJ");
		printf ("Enter 4 - MTV\n");
		printf ("Enter 7 - MONEY $$$\n Input: ");
		printf ("Retraining from: "); 
		scanf ("%d\n", &a.disciplineFrom);
		printf ("Retraining to: ");
		scanf ("%d\n", &a.disciplineTo);
	}
	return a;
}

//Function to roll two dices
static int rollDice (void) {

	int diceValue = 0;

	//Ensures that the dice will be from 1 to 6
	diceValue = (rand()%6 + 1) + (rand()%6 + 1); 

	return diceValue;
}


action decideAction (Game g) {

        int me = getWhoseTurn (g);
        
        //Number of students resources
        int engineer = getStudents (g, me, STUDENT_BPS);
        int scientist = getStudents (g, me, STUDENT_BQN);
        int jobOwner = getStudents (g, me, STUDENT_MJ);
        int tvStar = getStudents (g, me, STUDENT_MTV);
        int moneyMaker = getStudents (g, me, STUDENT_MMONEY);
        
        //Exchange rate - Retraining Conditions
        //Last argument unnecessary as only disciplineFrom is important
        int costSci = getExchangeRate (g, me, STUDENT_BQN, STUDENT_MJ);
        int costEng = getExchangeRate (g, me, STUDENT_BPS, STUDENT_MJ);
        int costJob = \
            getExchangeRate (g, me, STUDENT_MJ, STUDENT_BPS);
        int costTv = getExchangeRate (g, me, STUDENT_MTV, STUDENT_BPS);
        int costMon = \
            getExchangeRate (g, me, STUDENT_MMONEY, STUDENT_BPS);
            
        //Possible actions to do:
        //Spinoffs -> 1 MJ + 1 MTV + 1 MMNY
        //Build ARCs -> 1 BPS + 1 BQN
        action nextAction;
        if (tvStar > costTv || jobOwner > costJob \
            || moneyMaker > costMon || scientist >= costSci \
            || engineer >= costEng) {
            // > to keep some MMONEY and MJOBS and MTVs,
            // to use for future spinoffs
            // >= to get rid of all BPS and BQNs
            
            nextAction.actionCode = RETRAIN_STUDENTS;
            //determine which student to retrain to
            if (jobOwner <= tvStar && jobOwner <= moneyMaker) {
                nextAction.disciplineTo = STUDENT_MJ;
            } else if (tvStar <= jobOwner && tvStar <= moneyMaker) {
                nextAction.disciplineTo = STUDENT_MTV;
            } else {
                nextAction.disciplineTo = STUDENT_MMONEY;
            }
            
            //Find out which student to retrain from
            /*Order is 
              MJ -> MTV -> MMONEY -> BQN -> BPS
              this ensures that BQN/BPS are retrained first*/ 
            if (jobOwner > costJob) {
                nextAction.disciplineFrom = STUDENT_MJ;
            } else if (tvStar > costTv) {
                nextAction.disciplineFrom = STUDENT_MTV;
            } else if (moneyMaker > costMon) {
                nextAction.disciplineFrom = STUDENT_MMONEY;
            } else if (scientist >= costSci) {
                nextAction.disciplineFrom = STUDENT_BQN;
            } else {
                nextAction.disciplineFrom = STUDENT_BPS;
            }
            
        } else if ((jobOwner * tvStar * moneyMaker) > 0) {
            //create a spinoff
            nextAction.actionCode = START_SPINOFF;
        } else {
            //no resources so end turn
            nextAction.actionCode = PASS;
        }
        
        return nextAction;
}



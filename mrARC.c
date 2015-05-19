//Tries to build ARCs when possible and make the longest ARC
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
 
#include "Game.h"
#include "mechanicalTurk.h"

//paths to Campus starting point
#define FROM_CAMPUS_A1 ""
#define FROM_CAMPUS_A2 "RLRLRLRLRLL"
#define FROM_CAMPUS_B1 "RRLRLL"
#define FROM_CAMPUS_B2 "LRLRLRRLRLR"
#define FROM_CAMPUS_C1 "RRLRLLRLRLL"
#define FROM_CAMPUS_C2 "LRLRLR"
#define NO_PATH " "
#define FAILURE 1


//Checks for NO_PATH i.e. " ".
static int failPath (char *path);
//Finds a possible ARC and returns " " if there are none.
static char *possiblePath (Game g, char *posRoute, int me);
//Returns the path for the ARC to bew built.
static char *destination (Game g, int me);

//just for checking that AI compiles
int main (int argc, char *argv[]) {
    int disciplines[] = {STUDENT_BQN, STUDENT_MMONEY, STUDENT_MJ, 
                STUDENT_MMONEY, STUDENT_MJ, STUDENT_BPS, STUDENT_MTV, 
                STUDENT_MTV, STUDENT_BPS,STUDENT_MTV, STUDENT_BQN, 
                STUDENT_MJ, STUDENT_BQN, STUDENT_THD, STUDENT_MJ, 
                STUDENT_MMONEY, STUDENT_MTV, STUDENT_BQN, STUDENT_BPS };
    int dice[] = {9,10,8,12,6,5,3,11,3,11,4,6,4,7,9,2,8,10,5};
    Game g = newGame (disciplines, dice);
    action a = decideAction (g);
    makeAction (g, a);
    disposeGame (g);
    return EXIT_SUCCESS;
}

action decideAction (Game g) {

    int me = getWhoseTurn (g);

    //Resources
    int engineer = getStudents (g, me, STUDENT_BPS);
    int scientist = getStudents (g, me, STUDENT_BQN);
    int jobOwner = getStudents (g, me, STUDENT_MJ);
    int tvStar = getStudents (g, me, STUDENT_MTV);
    int moneyMaker = getStudents (g, me, STUDENT_MMONEY);

    //Retraining conditions 
    //Last argument of getExchangeRate is useless.
    int costJobs = 
        getExchangeRate (g, me, STUDENT_MJ, STUDENT_BPS);
    int costTv = getExchangeRate (g, me, STUDENT_MTV, STUDENT_BPS);
    int costMoney = 
        getExchangeRate (g, me, STUDENT_MMONEY, STUDENT_BPS);
    int costSci = getExchangeRate (g, me, STUDENT_BQN, STUDENT_MTV);
    int costEng = 
        getExchangeRate (g, me, STUDENT_BPS, STUDENT_MMONEY);

    //Possible actions depending on resources
    action nextAction;
    //possible ARC position
    strcpy (nextAction.destination, destination (g, me));

    /*Build an ARC if there enough resources and a space to build 
      an ARC exists exists.*/
    if (engineer * scientist > 0 &&  
        (failPath (nextAction.destination) != FAILURE)) {

        nextAction.actionCode = OBTAIN_ARC;

    /*Retrain students into BPS or BQN in order to build an ARC.*/
    } else if ((tvStar >= costTv || jobOwner >= costJobs 
               || moneyMaker >= costMoney) && 
               (failPath (nextAction.destination) != FAILURE)) {

        nextAction.actionCode = RETRAIN_STUDENTS;
            
        //Determine which student to retrain from. 
        if (tvStar >= costTv) {

            nextAction.disciplineFrom = STUDENT_MTV;

        } else if (jobOwner >= costJobs) {

            nextAction.disciplineFrom = STUDENT_MJ;

        } else {

            nextAction.disciplineFrom = STUDENT_MMONEY;

        }

        //Determine which student to retrain to.
        if (engineer > scientist) {

            nextAction.disciplineTo = STUDENT_BQN;

        } else {

            nextAction.disciplineTo = STUDENT_BPS;

        }

    /*Start a spin off if ther are enough resources but not if
      if there is an ARC which can be built later on.*/    
    } else if (((jobOwner * tvStar * moneyMaker) > 0) &&
               (failPath (nextAction.destination) == FAILURE)) {

        nextAction.actionCode = START_SPINOFF;

    /*Retrain students in order to start a spin off.*/    
    } else if ((tvStar > costTv || jobOwner > costJobs 
               || moneyMaker > costMoney || scientist >= costSci 
               || engineer >= costEng) && 
               (failPath (nextAction.destination) == FAILURE)) {

            nextAction.actionCode = RETRAIN_STUDENTS;

            //Determine which student to retrain to.
            if (jobOwner <= tvStar && jobOwner <= moneyMaker) {
            
                nextAction.disciplineTo = STUDENT_MJ;
            
            } else if (tvStar <= jobOwner && tvStar <= moneyMaker) {
            
                nextAction.disciplineTo = STUDENT_MTV;
            
            } else {
            
                nextAction.disciplineTo = STUDENT_MMONEY;
            
            }
            
            //Find out which student to retrain from.
            /*Order is 
              BQN -> BPS -> MTV -> MMONEY -> MJ
              This ensures that BQN/BPS are retrained first.*/ 
            if (scientist >= costSci) {
            
                nextAction.disciplineFrom = STUDENT_BQN;
            
            } else if (engineer >= costEng) {
            
                nextAction.disciplineFrom = STUDENT_BPS;
            
            } else if (tvStar > costTv) {
            
                nextAction.disciplineFrom = STUDENT_MTV;
            
            } else if (moneyMaker > costMoney) {
            
                nextAction.disciplineFrom = STUDENT_MMONEY;
            
            } else {
            
                nextAction.disciplineFrom = STUDENT_MJ;
            
            }


    /*If no resources or no requirements are fufilled for other actions
      so end turn.*/    
    } else {

        nextAction.actionCode = PASS;
    }

        return nextAction;
}

static char *destination (Game g, int me) {

    char *route = malloc (sizeof (char) * 150);
    char *altRoute1 = malloc (sizeof (char) * 150);
    char *altRoute2 = malloc (sizeof (char) * 150);

    //Routes for different starting campuses
    if (me == UNI_A) {

        strcpy (altRoute1, FROM_CAMPUS_A1);
        strcpy (altRoute2, FROM_CAMPUS_A2);        

    } else if (me == UNI_B) {

        strcpy (altRoute1, FROM_CAMPUS_B1);
        strcpy (altRoute2, FROM_CAMPUS_B2);

    } else {

        strcpy (altRoute1, FROM_CAMPUS_C1);
        strcpy (altRoute2, FROM_CAMPUS_C2);        

    }

    /*Find a vacant ARC from campus 1.*/
    strcpy (route, possiblePath (g, altRoute1, me));

    /*Check if the ARC exists.*/
    if (failPath (route)) {

        /*Special case where you need to go backwards 
          from campus 1 for a vacant ARC.*/
        strcat (altRoute1, "B");
        strcpy (route, possiblePath (g, altRoute1, me));

        /*Check if a vacant ARC exists.*/
        if (failPath (route)) {

            /*Check if there is a vacant ARC from campus 2.*/
            strcpy (route, possiblePath (g, altRoute2, me));

            /*Check that it exists.*/
            if (failPath (route)) {

                /*Special case as above.*/
                strcat (altRoute2, "B");
                strcpy (route, possiblePath (g, altRoute2, me));
                /*No need to check if it exists because there are 
                  no other possible ARCs for longest ARC.*/

            }

        }

    }
    
    free (altRoute1);
    free (altRoute2);

    return route;
}

static char *possiblePath (Game g, char *posRoute, int me) {

    char *temp1 = malloc (sizeof (char) * 150);
    char *temp2 = malloc (sizeof (char) * 150);
    int combinations = 0;
    int counter = 0;

    while (getARC(g, posRoute) != VACANT_ARC && combinations < 150) {

        /*Finding other ARCs owned by this AI and trying to build 
          from them.*/
        while (getARC (g, temp1) != me && counter < 2) {

            strcpy (temp1, posRoute);

            //Checking left
            if ((counter % 2) == 0) {

                strcat (temp1, "L");

            //Checking right
            } else {

                strcat (temp1, "R");

            } 

            /*Increase counter so that it will end if both cases
              have either vacant ARCs or there no possible ARCs.*/ 
            counter ++;

        }

        //Checking that none of the ARCs were owned by AI.
        if (getARC (g, temp1) != me) {

            //Reset temps to previous path.
            strcpy (temp1, posRoute);
            strcpy (temp2, posRoute);
            strcat (temp1, "L");
            strcat (temp2, "R");

            /*Checking if left path is vacant*/ 
            if (getARC (g, temp1) == VACANT_ARC) {

                strcpy (posRoute, temp1);

            /*Checking if right path is vacant*/    
            } else if (getARC (g, temp2) == VACANT_ARC) {

                strcpy (posRoute, temp2);

            /*End the loop if no route exists.*/
            } else {

                combinations = 150;

            }

        /*Reiterate loop if ARC is owned by AI.*/    
        } else {

            counter = 0;
            strcpy (posRoute, temp1);
            /*If conbinations >= 150 then path does not exist
              or it would be over the max path limit.*/
            combinations ++;

        }

    }

    //Return that no path exists.
    if (combinations == 150) {

        strcpy (posRoute, NO_PATH);

    }

    free (temp1);
    free (temp2);

    return posRoute;
}

static int failPath (char *path) {

    return (strcmp (path, NO_PATH) == 0);
}

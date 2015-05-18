// Knowledge Island AI - mrRetrain.c
// ***Tries to retrain students so that it can start spin offs
// By Louis Cheung & Stephanie Chua

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
 
#include "Game.h"
#include "mechanicalTurk.h"

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
              BQN -> BPS -> MTV -> MMONEY -> MJ
              this ensures that BQN/BPS are retrained first*/ 
            if (scientist >= costSci) {
                nextAction.disciplineFrom = STUDENT_BQN;
            } else if (engineer >= costEng) {
                nextAction.disciplineFrom = STUDENT_BPS;
            } else if (tvStar > costTv) {
                nextAction.disciplineFrom = STUDENT_MTV;
            } else if (moneyMaker > costMon) {
                nextAction.disciplineFrom = STUDENT_MMONEY;
            } else {
                nextAction.disciplineFrom = STUDENT_MJ;
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

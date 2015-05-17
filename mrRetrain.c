//Tries to retrain students so that it can start spin offs
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
 
#include "Game.h"
#include "mechanicalTurk.h"

action decideAction (Game g) {
        int me = getWhoseTurn (g);
        //resources
        int engineer = getStudents (g, me, STUDENT_BPS);
        int scientist = getStudents (g, me STUDENT_BQN);
        int jobOwner = getStudents (g, me, STUDENT_MJ);
        int tvStar = getStudents (g, me, STUDENT_MTV);
        int moneyMaker = getStudents (g, me, STUDENT_MMONEY);
        //retraining conditions 
        //last argument of getExchangeRate is useless
        int costSci = getExchangeRate (g, me, STUDENT_BQN, STUDENT_MJ);
        int costEng = getExchangeRate (g, me, STUDENT_BPS, STUDENT_MJ);
        int costJob = \
            getExchangeRate (g, me, STUDENT_MJ, STUDENT_BPS);
        int costTv = getExchangeRate (g, me, STUDENT_MTV, STUDENT_BPS);
        int costMon = \
            getExchangeRate (g, me, STUDENT_MMONEY, STUDENT_BPS);
        //possible actions depending on resources
        action nextAction;
        if (tvStar > costTv || jobOwner > costJobs \
            || moneyMaker > costMon || scientist >= costSci \
            || engineer >= costEng) {
            //retrain students so can start spin off
            nextAction.actionCode = RETRAIN_STUDENTS;
            //determine which student to retrain to
            if (jobOwner <= tvStar && jobOwner <= moneyMaker) {
                nextAction.disciplineTo = STUDENT_MJ;
            } else if (tvStar <= jobOwner tvStar <= moneyMaker) {
                nextAction.disciplineTo = STUDENT_MTV;
            } else {
                nextAction.disciplineTo STUDENT_MMONEY;
            }
            //find out which student to retrain from
            /*order is 
              MJ -> MTV -> MMONEY -> BQN -> BPS
              this ensures that BQN/BPS are retrained first*/ 
            if (jobOwner > costJobs) {
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

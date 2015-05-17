/*
 *  Mr Pass.  Brain the size of a planet!
 *
 *  Proundly Created by Richard Buckland
 *  Share Freely Creative Commons SA-BY-NC 3.0. 
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
#include "Game.h"
#include "mechanicalTurk.h"

action decideAction (Game g) {
    int me = getWhoseTurn (g);
    int job = getStudents (g, me, STUDENT_MJ);
    int tv = getStudents (g, me, STUDENT_MTV);
    int money = getStudents (g, me, STUDENT_MMONEY);
    action nextAction;
    /*if there are not enogh resources then it just passes.
      only 1 MJ, 1 MTV and 1 MMONEY is needed to start a spin off
      hence multyplying the numbers of each together can determine
      if there are enough. i.e. 1 MJ, 2 MTV, 2 MMONEY -> 4 thus 
      can start spin off. 3 MJ, 6 MTV, 0 MMONEY -> 0 thus can't 
      start a spin off*/  
    if ((job * tv * money) > 0) {
           nextAction.actionCode = START_SPINOFF;
    } else {
        nextAction.actionCode = PASS;
    }
    return nextAction;
}


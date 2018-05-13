//#pragma once
#include <vector>
//#include <iostream>

bool detectionCombat(HWND dofusScreen, bool & finCombat, bool & finTour);
bool detectionDebutCombat(HWND dofusScreen, bool & modeCreature, bool & modeTactique);
bool detectionPositions(HWND dofusScreen, POINT & posJoueur, POINT & posEnnemi);
std::vector<POINT> detectionDeplacement(HWND dofusScreen);
std::vector<POINT> detectionSort(HWND dofusScreen);

std::vector<POINT> detectionPopUps(HWND dofusScreen);

bool findNearestPoint(HWND dofusScreen, POINT posEnnemi, std::vector<POINT> posArray, POINT & nearestPoint, int threshold);
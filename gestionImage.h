//#pragma once
//#include <vector>
//#include <iostream>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd);

bool detectionCombat(HWND dofusScreen, bool & finCombat, bool & finTour);
bool detectionDebutCombat(HWND dofusScreen, bool & modeCreature, bool & modeTactique);
bool detectionPositions(HWND dofusScreen, POINT & posJoueur, POINT & posEnnemi);
vector<POINT> detectionDeplacement(HWND dofusScreen);
vector<POINT> detectionSort(HWND dofusScreen);

std::vector<POINT> detectionPopUps(HWND dofusScreen);

bool findNearestPoint(HWND dofusScreen, POINT posEnnemi, vector<POINT> posArray, POINT & nearestPoint, int threshold);
#include <iostream>
#include <vector>
#include <windows.h>
#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionImage.h"

using namespace std;

bool gestionPopUps(HWND dofusScreen)
{
	POINT nearestPoint;
	POINT pos1, pos2, pos3;

	pos1.x = 288;		// invite groupe, tout �a ...
	pos1.y = 471;

	pos2.x = 770;		// demande �change, passage de niveau, tout �a ...
	pos2.y = 449;

	vector<POINT> posPopUps = detectionPopUps(dofusScreen);

	if (findNearestPoint(dofusScreen, pos1, posPopUps, nearestPoint, 0)) {			// Invite groupe, tout �a ... :)

		if (abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) < 200) {

		//	cout << "weigh = " << abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1500);
		}
	}
	
	if (findNearestPoint(dofusScreen, pos2, posPopUps, nearestPoint, 0)) {			// Demande �change, tout �a ...

		if (abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) < 200) {

			cout << "weigh = " << abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1500);
		}
	}


	return true;
}
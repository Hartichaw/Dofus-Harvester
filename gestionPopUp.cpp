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

	pos1.x = 288;		// invite groupe, tout ça ...
	pos1.y = 471;

	pos2.x = 770;		// demande échange, passage de niveau, tout ça ...
	pos2.y = 449;

	vector<POINT> posPopUps = detectionPopUps(dofusScreen);

	if (findNearestPoint(dofusScreen, pos1, posPopUps, nearestPoint, 0)) {			// Invite groupe, tout ça ... :)

		if (abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) < 200) {

		//	cout << "weigh = " << abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1500);
		}
	}
	
	if (findNearestPoint(dofusScreen, pos2, posPopUps, nearestPoint, 0)) {			// Demande échange, tout ça ...

		if (abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) < 200) {

			cout << "weigh = " << abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1500);
		}
	}


	return true;
}
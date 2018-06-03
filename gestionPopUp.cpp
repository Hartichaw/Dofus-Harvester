#include <iostream>
#include <vector>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionImage.h"

using namespace std;
using namespace cv;

vector<POINT> detectionPopUps(HWND dofusScreen)	// détecte la grille de déplacement
{
	Mat imageBGR;
	imageBGR = hwnd2mat(dofusScreen);

	Sleep(500);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(0, 221, 181), Scalar(10, 241, 201), 100, 1257, 100, 694, 500, 1, false);

	return posArray;
}

bool gestionPopUps(HWND dofusScreen)
{
	POINT nearestPoint;
	POINT pos1, pos2;

	pos1.x = 288;		// invite groupe, tout ça ...
	pos1.y = 471;

	pos2.x = 770;		// demande échange, passage de niveau, tout ça ...
	pos2.y = 449;

	vector<POINT> posPopUps = detectionPopUps(dofusScreen);

	if (findNearestPoint(pos1, posPopUps, nearestPoint, 0, 1)) {			// Invite groupe, tout ça ... :)

		if (abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) < 200) {

		//	cout << "weigh = " << abs(pos1.x - nearestPoint.x) + abs(pos1.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1000);
			SetCursorPos(0, 0);
			Sleep(500);
		}
	}
	
	if (findNearestPoint(pos2, posPopUps, nearestPoint, 0, 1)) {			// Demande échange, tout ça ...

		if (abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) < 200) {

		//	cout << "weigh = " << abs(pos2.x - nearestPoint.x) + abs(pos2.y - nearestPoint.y) << endl;
			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1000);
			SetCursorPos(0, 0);
			Sleep(500);
		}
	}


	return true;
}
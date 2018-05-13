#include <iostream>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "gestionDeplacement.h"

using namespace std;
using namespace cv;

/****************** CAPTURE ECRAN ******************/

Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	cvtColor(src, src, CV_BGRA2BGR);
	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

/****************** DETECTION COULEUR ******************/

vector<POINT> detectionColorArray(Mat imageBGR, Scalar lowerb, Scalar upperb, int x1, int x2, int y1, int y2, int threshold, bool DEBUG)
{
	Mat imageResizeBGR;
	Mat maskImage;
	Mat mask1, mask2, mask3;
	//int nbPixel;

	imageResizeBGR = imageBGR(Range(y1, y2), Range(x1, x2));
	//inRange(imageResizeBGR, lowerb, upperb, maskImage);

	inRange(imageResizeBGR, lowerb, upperb, mask1);
	int nbPixel = countNonZero(mask1);

	if (nbPixel < threshold) {			// if not enough pixel found
		vector<POINT> emptyVector(0);
		return emptyVector;
	}

	// MODIF

	Mat kernel_erode = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat kernel_dilate = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));

	// erosion/ dilatation, supprimes les petits pixels assimilies a du bruit
	morphologyEx(mask1, mask2, MORPH_ERODE, kernel_erode);
	morphologyEx(mask2, maskImage, MORPH_DILATE, kernel_dilate);

	// Modif

/// Find contours   
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(maskImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Point> posContours(hierarchy.size());
	
	int i;
	int idx = 0;
	unsigned int posX = 0, posY = 0;
	int j = 0;

	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		const vector<Point>& pt = contours[idx];	
		
		posX = 0; posY = 0;
		
		for (i = 0; i < pt.size(); i++) {
			posX += pt[i].x;
			posY += pt[i].y;
		}

		posContours[j].x = posX / pt.size();
		posContours[j].y = posY / pt.size();
		j++;
	}

	vector<POINT> posArray(posContours.size());

	for (int i = 0; i < posContours.size(); i++) {
		posArray[i].x = x1 + posContours[i].x;
		posArray[i].y = y1 + posContours[i].y + 22;
	}

	if (DEBUG) {
		cout << "Nb pixels = " << nbPixel << endl;
		cout << "num objects hierarchy : " << hierarchy.size() << endl;

		namedWindow("image resize", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
		imshow("image resize", imageResizeBGR); // Show our image inside it.
		namedWindow("mask", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
		imshow("mask", maskImage); // Show our image inside it.

		/// Draw contours
		RNG rng(12345);
		Mat drawing = Mat::zeros(maskImage.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}
		imshow("Result window", drawing);
		waitKey(0);
	}


	return posArray;
}

bool detectionColorPos(Mat imageBGR, Scalar lowerb, Scalar upperb, int x1, int x2, int y1, int y2, POINT & posCible, int threshold, bool DEBUG)
{
	Mat imageResizeBGR;
	Mat maskImage;
	int nbPixel;

	imageResizeBGR = imageBGR(Range(y1, y2), Range(x1, x2));
	inRange(imageResizeBGR, lowerb, upperb, maskImage);

	nbPixel = countNonZero(maskImage);

	if (DEBUG) {
		cout << "Nb pixels = " << nbPixel << endl;
		namedWindow("image resize", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
		imshow("image resize", imageResizeBGR); // Show our image inside it.
		namedWindow("mask", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
		imshow("mask", maskImage); // Show our image inside it.
		waitKey(0); // Wait for a keystroke in the window
	}

	if (nbPixel > threshold) {

		Mat locations;   // output, locations of non-zero pixels
		findNonZero(maskImage, locations);

		int posX = 0, posY = 0;

		for (int i = 0; i < locations.total(); i++) {
			posX += locations.at<Point>(i).x;
			posY += locations.at<Point>(i).y;
		}
		posX = posX / (int)locations.total();
		posY = posY / (int)locations.total();

		posCible.y = y1 + posY + 22;	// Correction de 22 sur l'axe Y
		posCible.x = x1 + posX;

		return true;
	}
	else {
		return false;
	}

}

/****************** DETECTION POINT LE PLUS PROCHE ******************/

bool findNearestPoint(HWND dofusScreen, POINT posEnnemi, vector<POINT> posArray, POINT & nearestPoint, int threshold)
{
	int i;

	if (posArray.size() == 0) {
		return false;
	}

	nearestPoint.x = 0;
	nearestPoint.y = 0;

	for (i = 0; i < posArray.size(); i++) {		// Déplacement le plus proche possible de l'ennemi

												// Si point plus proche
		if (abs(posArray[i].x - posEnnemi.x) + abs(posArray[i].y - posEnnemi.y) < abs(nearestPoint.x - posEnnemi.x) + abs(nearestPoint.y - posEnnemi.y)) {

			if (abs(posArray[i].x - posEnnemi.x) >= threshold && abs(posArray[i].y - posEnnemi.y) >= threshold) {	// on exclue les cases en diagonale (à 1 case) du mob
				nearestPoint = posArray[i];
			}
		}
	}

	return true;
}

/****************** DETECTION COMBAT ******************/

vector<POINT> detectionSort(HWND dofusScreen)	// détecte la grille de sort
{
	Mat imageBGR;
	imageBGR = hwnd2mat(dofusScreen);

	Sleep(500);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(143, 41, 24), Scalar(163, 61, 44), 278, 1257, 14, 694, 20, false);	

	return posArray;
}

vector<POINT> detectionDeplacement(HWND dofusScreen)	// détecte la grille de déplacement
{
	Mat imageBGR;
	imageBGR = hwnd2mat(dofusScreen);

	Sleep(500);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(60, 161, 112), Scalar(80, 181, 132), 278, 1257, 14, 694, 500, false);	
	
	return posArray;
}

bool detectionDebutCombat(HWND dofusScreen, bool & modeCreature, bool & modeTactique)
{
	Mat imageBGR = hwnd2mat(dofusScreen);
	POINT posCible;

	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 686, 775, posCible, 1000, false)) {	// detection Debut Combat
		
	//	Sleep(500);
	//	leftClick(posCible.x, posCible.y);	
		Sleep(500);

		// Détection du modre créature et tactique
		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1162, 1183, 775, 800, posCible, 20, false)) {	// detection mode tactique
	//		Sleep(500);
	//		leftClick(posCible.x, posCible.y);
	//		Sleep(500);
			modeTactique = true;
		}
		Sleep(500);
		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1186, 1207, 775, 800, posCible, 20, false)) {	// detection mode créature
	//		Sleep(500);
	//		leftClick(posCible.x, posCible.y);
	//		Sleep(500);
			modeCreature = true;
		}
	
	//	namedWindow("image", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
	//	imshow("image", imageBGR); // Show our image inside it.
	//	waitKey(0); // Wait for a keystroke in the window
		
		return true;
	}
	else {
		return false;
	}
}

bool detectionCombat(HWND dofusScreen, bool & finCombat, bool & finTour)
{
	Mat imageBGR;
	POINT posCible;

	imageBGR = hwnd2mat(dofusScreen);
	
	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 573, 999, 336, 601, posCible, 1500, false)) {	// fin combat
	//	Sleep(500);
	//	leftClick(posCible.x, posCible.y);
	//	Sleep(500);
		finCombat = true;
		return true;
	}

	Sleep(500);

	//if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 336, 775, posCible, 1000, false)) {	// fin tour
	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 686, 775, posCible, 1000, false)) {	// fin tour
	/*	Sleep(500);
		leftClick(posCible.x, posCible.y);
		Sleep(500);
		*/
		finTour = true;
	}

	return false;

	// debug :
//	namedWindow("fin combat", WINDOW_NORMAL/*WINDOW_AUTOSIZE*/); // Create a window for display.
//	imshow("fin combat", imageBGR); // Show our image inside it.
//	waitKey(0); // Wait for a keystroke in the window
	//system("pause");
}


bool detectionPositions(HWND dofusScreen, POINT & posJoueur, POINT & posEnnemi)	// détecte la pos du joueur et de l'ennemi
{
	Mat imageBGR;
	imageBGR = hwnd2mat(dofusScreen);
	
	Sleep(500);

	if (detectionColorPos(imageBGR, Scalar(200, 0, 0), Scalar(255, 20, 20), 278, 1257, 14, 694, posEnnemi, 50, false)) {	// ennemi
		
		Sleep(500);

		if (detectionColorPos(imageBGR, Scalar(0, 0, 200), Scalar(20, 20, 255), 278, 1257, 14, 694, posJoueur, 50, false)) {	// Joueur
			return true;
		}
	}

	return false;
}

/****************** DETECTION POP-UP ******************/

vector<POINT> detectionPopUps(HWND dofusScreen)	// détecte la grille de déplacement
{
	Mat imageBGR;
	imageBGR = hwnd2mat(dofusScreen);

	Sleep(500);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(0, 221, 181), Scalar(10, 241, 201), 100, 1257, 100, 694, 500, false);

	return posArray;
}

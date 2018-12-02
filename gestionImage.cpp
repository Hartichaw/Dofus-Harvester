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
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
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

vector<POINT> detectionColorArray(Mat imageBGR, Scalar lowerb, Scalar upperb, int x1, int x2, int y1, int y2, int threshold, int morphoOp, bool DEBUG)
{
	Mat imageResizeBGR;
	Mat maskImage;
	Mat mask1, mask2;

	imageResizeBGR = imageBGR(Range(y1, y2), Range(x1, x2));

	inRange(imageResizeBGR, lowerb, upperb, mask1);
	int nbPixel = countNonZero(mask1);

	if (nbPixel < threshold) {			// if not enough pixel found and not in debug mode

		if (DEBUG) {
			cout << "Pas assez de pixels detectes" << endl;
		}

		vector<POINT> emptyVector(0);
		return emptyVector;
	}

	// erosion/ dilatation, supprimes les petits pixels assimilies a du bruit
	Mat kernel_erode = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat kernel_dilate = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	if (morphoOp == 0) {
		maskImage = mask1;
	}
	else if (morphoOp == 1) {
		morphologyEx(mask1, maskImage, MORPH_ERODE, kernel_erode);
	}
	else if (morphoOp == 2) {
		morphologyEx(mask1, mask2, MORPH_ERODE, kernel_erode);
		morphologyEx(mask2, maskImage, MORPH_DILATE, kernel_dilate);
	}

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
		posArray[i].y = y1 + posContours[i].y + 22;		// Correction de 22 sur l'axe Y
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
		for (int i = 0; i < contours.size(); i++)
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

	return false;
}

/****************** DETECTION POINT LE PLUS PROCHE DE LA CIBLE ******************/

bool findNearestPoint(POINT posCible, vector<POINT> posArray, POINT & nearestPoint, int threshold, int detectMemeCase)
{
	if (posArray.size() == 0) {
		return false;
	}

	nearestPoint.x = 0;
	nearestPoint.y = 0;

	for (int i = 0; i < posArray.size(); i++) {		// Déplacement le plus proche possible de l'ennemi

		// Si point plus proche
		if (abs(posArray[i].x - posCible.x) + abs(posArray[i].y - posCible.y) < abs(nearestPoint.x - posCible.x) + abs(nearestPoint.y - posCible.y)) {

			if (abs(posArray[i].x - posCible.x) >= threshold && abs(posArray[i].y - posCible.y) >= threshold) {		// possibilité d'exclure les cases en diagonale de la cible en mettant threshold à 15
				nearestPoint = posArray[i];
			}
			else if (detectMemeCase == 1) {

				if (abs(posArray[i].x - posCible.x) <= 25 && abs(posArray[i].y - posCible.y) <= 15) {		// détection sur la même case que la cible (pratique pour les sorts)
					nearestPoint = posArray[i];
				}
			}
		}
	}

	return true;
}

/*
Mat getCursor(): Renvoie l'image du curseur courant sous forme d'openCV:MAT
*/
Mat getCursor()
{

	Mat OutputCursorImg; // matrice de  sortie

						 // On récupère l'environnement de la fenêtre
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// Création de la bitmap et du canvas dans lequel on va recuperer l'image.
	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, 256, 256);
	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

	// Get information about the global cursor.
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	GetCursorInfo(&ci);

	// information de l'icon (conversion HBM -> MAT)
	ICONINFO ii = { 0 };
	BITMAP bm;
	GetIconInfo(ci.hCursor, &ii);
	GetObject(ii.hbmMask, sizeof(BITMAP), &bm);

	// On transfère les données du curseur dans le canvas.
	DrawIcon(hdcMem, 0, 0, ci.hCursor);



	//Si les informmation remontées sont cohérentes
	if (bm.bmHeight > 0 && bm.bmWidth > 0)
	{

		Mat image(bm.bmHeight, bm.bmWidth, CV_8UC3);
		// On parcourt chaque pixel pour mettre à jour la matrice de sortie
		for (int i = 0; i < bm.bmHeight; i++) {
			for (int j = 0; j < bm.bmWidth; j++) {

				COLORREF c = GetPixel(hdcMem, i, j);

				image.at<cv::Vec3b>(i, j)[0] = (int)GetBValue(c);
				image.at<cv::Vec3b>(i, j)[1] = (int)GetGValue(c);
				image.at<cv::Vec3b>(i, j)[2] = (int)GetRValue(c);

			}
		}
		OutputCursorImg = image;
	}


	return OutputCursorImg;
}


/*
Mat imgProvider(): Renvoie l'image de la fenêtre passée en paramètre sous forme d'openCV:MAT
pressY : si mis à un, capture l'image en appuyant sur Y
*/
Mat imgProvider(int pressY, HWND dofusScreen) {

	if (pressY == 1) { PostMessage(dofusScreen, WM_KEYDOWN, 0x59, 0); } //press y
	Sleep(600);
	Mat desktopImgMAT = hwnd2mat(dofusScreen);
	if (pressY == 1) { PostMessage(dofusScreen, WM_KEYUP, 0x59, 0); } //release y

	return desktopImgMAT;
}

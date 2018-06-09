#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd);

vector<POINT> detectionColorArray(Mat imageBGR, Scalar lowerb, Scalar upperb, int x1, int x2, int y1, int y2, int threshold, int morphoOp, bool DEBUG);
bool detectionColorPos(Mat imageBGR, Scalar lowerb, Scalar upperb, int x1, int x2, int y1, int y2, POINT & posCible, int threshold, bool DEBUG);

bool findNearestPoint(POINT posCible, vector<POINT> posArray, POINT & nearestPoint, int threshold, int detectMemeCase);

/*
Mat getCursor(): Renvoie l'image du curseur courant sous forme d'openCV:MAT
*/
Mat getCursor();

/*
Mat imgProvider(): Renvoie l'image de la fenêtre passée en paramètre sous forme d'openCV:MAT
pressY : si mis à un, capture l'image en appuyant sur Y
*/
Mat imgProvider(int pressY, HWND dofusScreen);
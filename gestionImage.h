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
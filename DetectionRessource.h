#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Mat imgProvider(int pressY, HWND dofusScreen);
vector<vector<int>> scanRessource(HWND dofusScreen);
Mat getCursor();

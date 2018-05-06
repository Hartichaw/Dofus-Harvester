#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "DetectionRessource.h"

using namespace std;
using namespace cv;


int main(int argc, char* argv)
{

	char key = 'a';
	Mat image_src;
	const string filename = "C:/Users/cedri/Pictures/test_grid.png";
	vector<vector<int>> tabresultat;



	/*image_src = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

	if (!image_src.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}*/
	
	tabresultat = scanRessource();

	for (int i = 0; i<tabresultat.size(); i++)
	{
		cout << tabresultat[i][0] << "   " << tabresultat[i][1] << endl;
	}

	while (key != 'Q' && key != 'q') {
		key = cvWaitKey(10);
	}

}
#ifndef DEF_HARVESTMANAGER
#define DEF_HARVESTMANAGER

#include <string>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <windows.h>
#include <Winuser.h>

class harvestManager {

public:

	//Attributs
	const int DEBUG_DR = 0; // 0 pour ne pas display les images
	const std::string IMG_DIR = "C:/Users/cedri/Pictures/";

	/*
	Constructeur
	*/
	harvestManager(HWND hWindow)
	{
		curseurPaysan = cv::imread(std::string(IMG_DIR + "cursor_paysan.png"), CV_LOAD_IMAGE_COLOR);
		curseurPecheur = cv::imread(std::string(IMG_DIR + "cursor_pecheur.png"), CV_LOAD_IMAGE_COLOR);
		curseurBucheron = cv::imread(std::string(IMG_DIR + "cursor_bucheron.png"), CV_LOAD_IMAGE_COLOR);
		curseurAlchi = cv::imread(std::string(IMG_DIR + "cursor_alchi.png"), CV_LOAD_IMAGE_COLOR);

		std::cout << std::string(IMG_DIR + "cursor_alchi.png") << " / " << std::string(IMG_DIR + "cursor_bucheron.png") << std::endl;

		dofusScreen = hWindow;
	}
	POINT getRessourcePos();
private:

	//Attributs:

	HWND dofusScreen;
	cv::Mat curseurPaysan, curseurPecheur, curseurBucheron, curseurAlchi;
	const int SIZE_SEARCH_ZONE = 10;
	const int PRESS_Y = 1;
	const int DETECTION_THRESHOLD = 5;
	const int SIZE_RECT = 15;
	const cv::Scalar RED = cv::Scalar(5, 5, 255);
	const cv::Scalar BLUE = cv::Scalar(255, 5, 5);

	const float ROW_START_COEFF = 0.04074, ROW_STEP_COEFF = 0.04074, ROW_END_COEFF = 0.8055;
	const float COL_START_COEFF = 0.20520, COL_STEP_COEFF = 0.02227, COL_END_COEFF = 0.8437;

	bool checkCurseur(cv::Mat curseur);
};


#endif

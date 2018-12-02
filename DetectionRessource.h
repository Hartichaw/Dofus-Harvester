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


#ifndef DEF_HARVESTMANAGER
#define DEF_HARVESTMANAGER
class harvestManager {

public:

	//Attributs
	const int DEBUG_DR = 0; // 0 pour ne pas display les images
	const std::string IMG_DIR = "C:/Users/cedri/Pictures/";
	const int delay = 150;

	/*
	Constructeur
	*/
	harvestManager(HWND hWindow)
	{
		std::cout << "-----Chargement des templates de curseur-----" << std::endl;
		curseurPaysan = cv::imread(std::string(IMG_DIR + "cursor_paysan.png"), CV_LOAD_IMAGE_COLOR);
		std::cout << IMG_DIR + "cursor_paysan.png" << std::endl;
		curseurPecheur = cv::imread(std::string(IMG_DIR + "cursor_pecheur.png"), CV_LOAD_IMAGE_COLOR);
		std::cout << IMG_DIR + "cursor_pecheur.png" << std::endl;
		curseurBucheron = cv::imread(std::string(IMG_DIR + "cursor_bucheron.png"), CV_LOAD_IMAGE_COLOR);
		std::cout << IMG_DIR + "cursor_bucheron.png" << std::endl;
		curseurAlchi = cv::imread(std::string(IMG_DIR + "cursor_alchi.png"), CV_LOAD_IMAGE_COLOR);
		std::cout << IMG_DIR + "cursor_alchi.png"  << std::endl;
		//std::cout << std::string(IMG_DIR + "cursor_alchi.png") << " / " << std::string(IMG_DIR + "cursor_bucheron.png") << std::endl;

		dofusScreen = hWindow;
	}
	/*
	POINT scanRessource(): Renvoi un point représentant la position de la première ressource récoltable détectée
	*/
	POINT getRessourcePos(POINT lastPoint);
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


	/*
	checkCurseur(): true : curseur correspond à une ressource recoltable
	*/
	bool checkCurseur(cv::Mat curseur);

};
#endif

// Fonction diverses nécessaire à un fonctionnement harmonieux
bool sortPixels(std::vector<int>  vecA, std::vector<int> vecB);
int distPt(POINT ptA, POINT ptB);




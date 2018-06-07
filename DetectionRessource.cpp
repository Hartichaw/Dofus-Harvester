#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <windows.h>
#include <Winuser.h>

#include "gestionImage.h"

using namespace std;
using namespace cv;



//CONSTANTES A MODIFIER SI BESOIN
const int DEBUG_DR = 0; // 0 pour ne pas display les images

const String TEMPLATE_ICON_PAYSAN = "C:/Users/cedri/Pictures/cursor_paysan.png";
const String TEMPLATE_ICON_PECHEUR = "C:/Users/cedri/Pictures/cursor_pecheur.png";
const String TEMPLATE_ICON_BUCHERON = "C:/Users/cedri/Pictures/cursor_bucheron.png";
const String TEMPLATE_ICON_ALCHI = "C:/Users/cedri/Pictures/cursor_alchi.png";


//const String TEMPLATE_ICON_PAYSAN = "C:/opencv/cursor_paysan.png";

//const LPCSTR NAME_DOFUS_WINDOW = "Hartichaw - Dofus 2.46.15:0";
//const String TEMPLATE_ICON_PAYSAN = "C:/Users/cedri/Pictures/cursor_paysan.png";



//CONSTANTES A PAS TOUCHER (sauf si tu sais ce que tu fais )
const int SIZE_SEARCH_ZONE = 10;
const int PRESS_Y = 1;
const int DETECTION_THRESHOLD = 5;
const int SIZE_RECT = 15;
const Scalar RED = Scalar(5, 5, 255);
const Scalar BLUE = Scalar(255, 5, 5);

const float ROW_START_COEFF = 0.04074, ROW_STEP_COEFF = 0.04074, ROW_END_COEFF = 0.8055;
const float COL_START_COEFF = 0.20520, COL_STEP_COEFF = 0.02227, COL_END_COEFF = 0.8437;



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

	if(pressY == 1){PostMessage(dofusScreen, WM_KEYDOWN, 0x59, 0);} //press y
	Sleep(600);
	Mat desktopImgMAT = hwnd2mat(dofusScreen);
	if (pressY == 1) {PostMessage(dofusScreen, WM_KEYUP, 0x59, 0); } //release y

	return desktopImgMAT;
}


/*
checkCurseur(): true : curseur correspond à une ressource recoltable
*/
bool checkCurseur(Mat curseur)
{
	Mat diffPaysan, diffPecheur, diffBucheron, diffAlchi;

	const String TEMPLATE_ICON_PAYSAN = "C:/Users/cedri/Pictures/cursor_paysan.png";
	const String TEMPLATE_ICON_PECHEUR = "C:/Users/cedri/Pictures/cursor_pecheur.png";
	const String TEMPLATE_ICON_BUCHERON = "C:/Users/cedri/Pictures/cursor_bucheron.png";
	const String TEMPLATE_ICON_ALCHI = "C:/Users/cedri/Pictures/cursor_alchi.png";


	Mat curseurPaysan = imread(TEMPLATE_ICON_PAYSAN, CV_LOAD_IMAGE_COLOR);
	Mat curseurPecheur = imread(TEMPLATE_ICON_PECHEUR, CV_LOAD_IMAGE_COLOR);
	Mat curseurBucheron = imread(TEMPLATE_ICON_BUCHERON, CV_LOAD_IMAGE_COLOR);
	Mat curseurAlchi = imread(TEMPLATE_ICON_ALCHI, CV_LOAD_IMAGE_COLOR);

	// on verifie que les curseur aient la meme dimension
	if (curseur.rows != curseurPaysan.rows || curseur.cols != curseurPaysan.cols)
		return false;

	subtract(curseurPaysan, curseur, diffPaysan);
	subtract(curseurPecheur, curseur, diffPecheur);
	subtract(curseurBucheron, curseur, diffBucheron);
	subtract(curseurAlchi, curseur, diffAlchi);

		
	if((int)sum(sum(diffPaysan))[0] == 0)
		return true;

	if ((int)sum(sum(diffPecheur))[0] == 0)
		return true;

	if ((int)sum(sum(diffBucheron))[0] == 0)
		return true;

	if ((int)sum(sum(diffAlchi))[0] == 0)
		return true;

	return false;



}
/*
POINT scanRessource(): Renvoi un point représentant la position d'une ressource récoltable
*/
POINT scanRessource(HWND dofusScreen)
{

	/*
	VARIABLES
	*/


	//	Mat Img = imgProvider(PRESS_Y, dofusScreen); // pressing y

	Mat baseImg = imgProvider(0, dofusScreen);
	Mat Img = imgProvider(PRESS_Y, dofusScreen); // pressing y

												 // Variables nécessaires a la creation de la grille de points
	int ROW_NB = Img.rows;
	int COL_NB = Img.cols;

	int ROW_START = int(ROW_NB*ROW_START_COEFF);
	int	ROW_STEP = int(ROW_NB*ROW_STEP_COEFF);
	int	ROW_END = int(ROW_NB*ROW_END_COEFF);

	int COL_START = int(COL_NB * COL_START_COEFF);
	int	COL_STEP = int(COL_NB * COL_STEP_COEFF);
	int	COL_END = int(COL_NB * COL_END_COEFF);

	int scoredifference;

	//Matrices pour stocker les resultats intermediraires
	Mat  rsz_img, thresh_img, morpho_output, mask, displayImg, regionCurseur;
	

	//Point en sortie
	POINT output_pt;
	POINT null_output; null_output.x = -1; null_output.y = -1;

	//Tableau de sortie pour stocker les coordonnees des ressources
	vector<vector<int>>  tabRessources, outputTab;
	vector<int> RessCoordinates(2);

	// seuils de couleurs necessaires pour detourer les ressources
	//Scalar lower_color = Scalar(160, 190, 200);
	//Scalar upper_color = Scalar(248, 245, 235);
	Scalar lower_color = Scalar(1, 1, 1);
	Scalar upper_color = Scalar(255, 255, 255);

	// kernels des operations morphologiques
	Mat kernel_erode = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));
	Mat kernel_dilate = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));


	/*
	PROCESSING
	*/


	// on seuille l'image pour ne garder que les couleurs proches du contour des ressources
	// en sortie: img binaire, 1=couleur du contour , 0 sinon
	subtract(Img, baseImg, thresh_img);
	//cvtColor(Img, Img, CV_BGR2GRAY);
	inRange(thresh_img, lower_color, upper_color, thresh_img);

	// erosion/ dilatation, supprimes les petits pixels assimilies a du bruit

	morphologyEx(thresh_img, mask, MORPH_ERODE, kernel_erode);
	morphologyEx(mask, morpho_output, MORPH_DILATE, kernel_dilate);

	//Detection de la possition des ressources
	// on projette une grille sur la map et on regarde les points de la grille qui sont proche des ressources
	for (int row = ROW_START; row < ROW_END; row = row + ROW_STEP)
	{
		for (int col = COL_START; col < COL_END; col = col + COL_STEP)

		{

			//Pour chaque point de la grille on regarde si il y a des pixel à 1 dans le voisinage
			int rowTopLeft = row - SIZE_SEARCH_ZONE; if (rowTopLeft < 0) { rowTopLeft = 0; }
			int colTopLeft = col - SIZE_SEARCH_ZONE; if (colTopLeft < 0) { colTopLeft = 0; }
			int rowBottomRight = row + SIZE_SEARCH_ZONE; if (rowBottomRight > morpho_output.rows) { rowTopLeft = morpho_output.rows; }
			int colBottomRight = col + SIZE_SEARCH_ZONE; if (colBottomRight > morpho_output.cols) { rowTopLeft = morpho_output.cols; }
			Mat ROI = morpho_output(Range(rowTopLeft, rowBottomRight), Range(colTopLeft, colBottomRight));
			int score = countNonZero(ROI);

			if (score > DETECTION_THRESHOLD)
			{
				//Si on a détecté un point on l'enregistre
				RessCoordinates[0] = row;
				RessCoordinates[1] = col;
				tabRessources.push_back(RessCoordinates);
			}

			// afficher dans image
			//if (DEBUG == 1) { circle(morpho_output, Point(col, row), 3, BLUE); }
			//if (DEBUG == 1) { circle(Img, Point(col, row), 3, BLUE); }
		}
	}


	// Controle des points trouvés pour ne garder que les ressoruces récoltables
	for (int i = 0; i<tabRessources.size(); i++)
	{
		output_pt.x = tabRessources[i][1];
		output_pt.y = tabRessources[i][0] + 22;
		
		SetCursorPos(0, 0);
		Sleep(10);
		SetCursorPos(output_pt.x, output_pt.y);
		Sleep(100);
		regionCurseur = getCursor();
		//imwrite(string{ "C:/Users/cedri/Pictures/curseur/Cursor" + to_string(i) + ".png"}, regionCurseur);
		if (checkCurseur(regionCurseur))
		{
			//cout << "pssage pushback // x = " << tabRessources[i][1] << "    y = " << tabRessources[i][0] << endl;
			return output_pt;
		}

		
	}

	// Windows creation in debug mode
	if (DEBUG_DR == 1)
	{

		for (int i = 0; i<outputTab.size(); i++)
		{
			rectangle(Img, Point(outputTab[i][1] - SIZE_RECT, outputTab[i][0] - SIZE_RECT), Point(outputTab[i][1] + SIZE_RECT, outputTab[i][0] + SIZE_RECT), RED, 4);
		}
		namedWindow("Provided image", WINDOW_NORMAL);
		//imwrite("C:/Users/cedri/Pictures/Desktop_capture.png", Img);
		resize(Img, displayImg, Size(0, 0), 0.5, 0.5);
		imshow("Provided image", displayImg);
		//imwrite("C:/Users/cedri/Pictures/Bot-Img_and_grid.png", displayImg);

		namedWindow("Morpho Output", WINDOW_NORMAL);
		resize(morpho_output, displayImg, Size(0, 0), 0.5, 0.5);
		imshow("Morpho Output", displayImg);
		//imwrite("C:/Users/cedri/Pictures/Bot-gray.png", displayImg);


	}


	return null_output;



}


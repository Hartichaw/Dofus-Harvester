#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <windows.h>
#include <Winuser.h>
#include <algorithm>
#include "gestionImage.h"
#include "DetectionRessource.h"
#include "gestionDeplacement.h"

using namespace std;
using namespace cv;


/*
sortPixels : trie le tableau en fonction de la distance avec le point de référence
*/
bool sortPixels(vector<int>  vecA, vector<int> vecB)
{
	return (vecA[2] < vecB[2]);
}

/*
distPt distance entre deux POINT
*/
int distPt(POINT ptA, POINT ptB)
{
	return int(abs(ptA.x - ptB.x) + abs(ptA.y - ptB.y));
}

	
/*
POINT scanRessource(): Renvoi un point représentant la position de la première ressource récoltable détectée
*/
POINT harvestManager::getRessourcePos(POINT lastPoint)
{

	/*
	VARIABLES
	*/

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

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
	vector<int> RessCoordinates(3); //[x,y,distance avec lastpoint]

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
				RessCoordinates[2] = distPt(POINT{col,row}, lastPoint);
				tabRessources.push_back(RessCoordinates);
			}

			// afficher dans image
			//if (DEBUG == 1) { circle(morpho_output, Point(col, row), 3, BLUE); }
			//if (DEBUG == 1) { circle(Img, Point(col, row), 3, BLUE); }
		}
	}

	// on trie les points pour vérifier les points proches en premier
	std::sort(tabRessources.begin(), tabRessources.end(), sortPixels);
	
	
	// Controle des points trouvés pour ne garder que les ressoruces récoltables
	for (int i = 0; i<tabRessources.size(); i++)
	{
		output_pt.x = tabRessources[i][1];
		output_pt.y = tabRessources[i][0] + 22;

		SetCursorPos(0, 0);
		Sleep(delay);
		SetCursorPos(output_pt.x, output_pt.y);
		Sleep(delay);
		regionCurseur = getCursor();
		
		// on verifie que les curseur aient la meme dimension pour les comparer
		if (regionCurseur.rows == curseurPaysan.rows && regionCurseur.cols == curseurPaysan.cols)
		{
			
			if (checkCurseur(regionCurseur))
			{
				SetConsoleTextAttribute(hConsole, 3);
				if(DEBUG_DR) 
					cout << "Ressource found:  x = " << tabRessources[i][1] << "    y = " << tabRessources[i][0] << endl;
				SetConsoleTextAttribute(hConsole, 15);
				return output_pt;
			}
			else
			{
				SetConsoleTextAttribute(hConsole, 4);
				if(DEBUG_DR) {
					cout << "incorrect:  x = " << tabRessources[i][1] << "    y = " << tabRessources[i][0] << endl;
					imwrite(string{ "debugCursor/Cursor" + to_string(i) + ".png" }, regionCurseur);
				}
				
				SetConsoleTextAttribute(hConsole, 15);
			}
		}
		else if (DEBUG_DR)
		{
			cout << "Les curseurs n'ont pas la même dimension: template(" << curseurPaysan.rows << ", " << curseurPaysan.cols << ")   extracted(" << regionCurseur.rows << ", " << regionCurseur.cols << ")" << endl;
			imwrite(string{ "debugCursor/Cursor" + to_string(i) + ".png" }, regionCurseur);
		}



	}
	

	// Windows creation in debug mode
	/*if (DEBUG_DR)
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


	}*/


	return null_output;



}

	

/*
checkCurseur(): true : curseur correspond à une ressource recoltable
*/
bool harvestManager::checkCurseur(Mat curseur)
{
	Mat diffPaysan, diffPecheur, diffBucheron, diffAlchi;

	subtract(curseurPaysan, curseur, diffPaysan);
	subtract(curseurPecheur, curseur, diffPecheur);
	subtract(curseurBucheron, curseur, diffBucheron);
	subtract(curseurAlchi, curseur, diffAlchi);


	if ((int)sum(sum(diffPaysan))[0] == 0)
		return true;

	if ((int)sum(sum(diffPecheur))[0] == 0)
		return true;

	if ((int)sum(sum(diffBucheron))[0] == 0)
		return true;

	if ((int)sum(sum(diffAlchi))[0] == 0)
		return true;

	return false;

}


void harvestManager::recolterRessourcesMap()
{

	// Détection des ressources

	POINT posRessources{ 0,0 };

	
	do		// Si aucune ressource n'est détectée
	{
		posRessources = getRessourcePos(posRessources);
		//cout << "ressource located:" << posRessources.x << "," << posRessources.y << endl;
		if (posRessources.x >= 0 && posRessources.y >= 0)
			leftClick(posRessources.x, posRessources.y);


		Sleep(4000);
		//gestionCombat(dofusScreen);
		//gestionPopUps(dofusScreen);
	} while (posRessources.x >= 0 && posRessources.y >= 0);

	if (DEBUG_DR) 
		cout << "plus de ressources sur la map" << endl;


}


void harvestManager::setDebugMode(bool mode)
{
	DEBUG_DR = mode;
}

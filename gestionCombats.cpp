#include <iostream>
#include <vector>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionImage.h"

#define COEFF_PO_SORT 55
#define COEFF_ZONE_SORT 30

using namespace std;
using namespace cv;


/****************** DETECTION COMBAT ******************/
// Certaines fonctions ne font pas grand chose mais c'est pratique pour se rep�rer avec le nom des fonctions ;)

vector<POINT> detectionSort(HWND dofusScreen)	// d�tecte la grille de sort
{
	Mat imageBGR = hwnd2mat(dofusScreen);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(143, 41, 24), Scalar(163, 61, 44), 278, 1257, 14, 694, 20, 1, false);

	return posArray;
}

vector<POINT> detectionDeplacement(HWND dofusScreen)	// d�tecte la grille de d�placement
{
	Mat imageBGR = hwnd2mat(dofusScreen);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(60, 161, 112), Scalar(80, 181, 132), 278, 1257, 14, 694, 500, 1, false);

	return posArray;
}

vector<POINT> detectionPosEnnemis(HWND dofusScreen)		// d�tecte la posiiton de plusieurs ennemis
{
	Mat	imageBGR = hwnd2mat(dofusScreen);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(200, 0, 0), Scalar(255, 20, 20), 278, 1257, 14, 733, 25, 0, false);

	return posArray;
}

bool detectionPosJoueur(HWND dofusScreen, POINT & posJoueur)	// d�tecte la pos du joueur
{
	Mat	imageBGR = hwnd2mat(dofusScreen);

	if (detectionColorPos(imageBGR, Scalar(0, 0, 200), Scalar(20, 20, 255), 278, 1257, 14, 733, posJoueur, 25, false)) {	// Joueur
		return true;
	}

	return false;
}

bool detectionDebutCombat(HWND dofusScreen, bool & modeCreature, bool & modeTactique)
{
	POINT posCible;
	Mat imageBGR = hwnd2mat(dofusScreen);

	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 686, 775, posCible, 1000, false)) {	// detection Debut Combat

		Sleep(500);
		imageBGR = hwnd2mat(dofusScreen);		// Re-capture de l'�cran pour �tre certain que l'�cran de combat soit totalement charg�

		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1162, 1183, 775, 800, posCible, 20, false)) {	// detection mode tactique
			modeTactique = true;
		}
		else {
			modeTactique = false;
		}

		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1186, 1207, 775, 800, posCible, 20, false)) {	// detection mode cr�ature

			modeCreature = true;
		}
		else {
			modeCreature = false;
		}
		return true;
	}

	return false;
}

bool detectionCombat(HWND dofusScreen, bool & finCombat, bool & finTour, POINT & posFinCombat)
{
	POINT posCible;
	Mat imageBGR = hwnd2mat(dofusScreen);

	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 603, 893, 449, 708, posFinCombat, 1500, false)) {	// fin combat
		finCombat = true;
		return true;
	}
	else {
		finCombat = false;
	}

	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 686, 775, posCible, 1000, false)) {	// fin tour
		finTour = true;
	}
	else {
		finTour = false;
	}

	return false;

}


/****************** GESTION COMBAT ******************/

bool gestionAttaque(HWND dofusScreen, int sort, int poSort, int zoneSort, POINT posJoueur, POINT posEnnemi)
{
	POINT nearestPoint;
	POINT posAnnulSort = { 100, 100 };

	if (abs(posJoueur.x - posEnnemi.x) + abs(posJoueur.y - posEnnemi.y) < poSort * COEFF_PO_SORT) {	// Si on est � c�t� du monstre on essaie de taper

		switch (sort) {			// S�lection du sort

		case 0: leftClick(798, 751); break;	// sort 0 = CAC
		case 1: leftClick(827, 757); break;
		case 2: leftClick(866, 755); break;
		case 3: leftClick(900, 754); break;
		case 4: leftClick(933, 754); break;
		case 5: leftClick(966, 754); break;
		default: break;
		}

		Sleep(1000);

		vector<POINT> posSort = detectionSort(dofusScreen);

		if (findNearestPoint(posEnnemi, posSort, nearestPoint, 15, 1)) {		// 15 pour �liminer les cases en diagonale du monstre
			
			if (abs(posEnnemi.x - nearestPoint.x) + abs(posEnnemi.y - nearestPoint.y) < COEFF_ZONE_SORT * zoneSort) {		// Si on peut taper, on tape !!!
				leftClickResPos(nearestPoint, 2000);
				return true;			// return true si on a frapp� l'ennemi
			}
		}

		leftClickResPos(posAnnulSort, 300);		// Si imposible d'attaquer on click en 100, 100 pour annuler le sort
	}

	return false;
}

bool gestionAction(HWND dofusScreen, int sort, int poSort, int zoneSort)
{
	// Param entr�e : sort=0 => CAC, sort=1 => sort 1, etc...
	//				  poSort = PO du sort dans le jeu
	//				  zoneSort=1 => pas de zone, zoneSort=2 => croix de 1 case, etc...

	// Algo de la fonction :
	// D�tection de la pos des ennemis et du joueur
	// Attaque si possible, puis return true
	// Sinon d�placement puis attaque si possible

	POINT posJoueur;
	POINT posEnnemi;
	POINT nearestPoint;

	if (!detectionPosJoueur(dofusScreen, posJoueur)) {	// d�tecte la pos du joueur
		return false;
	}

	//Sleep(200);
	vector<POINT> posEnnemis = detectionPosEnnemis(dofusScreen);		// d�tecte la pos des ennemis

	if (!findNearestPoint(posJoueur, posEnnemis, posEnnemi, 0, 1)) {		// d�tecte l'ennemi le plus proche
		return false;
	}

	if (gestionAttaque(dofusScreen, sort, poSort, zoneSort, posJoueur, posEnnemi)) {		// Si on peut attaquer sans bouger, on le fait et return true
		return true;	// Si attaque r�ussie, return true
	}
	
	vector<POINT> posDeplacement = detectionDeplacement(dofusScreen);						// Sinon on d�tecte la possibilit� de d�placement
		
	if (findNearestPoint(posEnnemi, posDeplacement, nearestPoint, 15, 0)) {		// si on peut se d�placer
	
		leftClickResPos(nearestPoint, (abs(posJoueur.x - nearestPoint.x) + abs(posJoueur.y - nearestPoint.y)) * 15 + 1500);	// D�lai d�placment perso
		posJoueur = nearestPoint;		// MAJ de la pos du joueur

		if (gestionAttaque(dofusScreen, sort, poSort, zoneSort, posJoueur, posEnnemi)) {		// Attaque
			return true;	// Si attaque r�ussie, return true
		}
	}

	return false;
}

// A modifier en fonction du besoin
void gestionTourJoueur(HWND dofusScreen)
{
	static int tour = 0;
	tour++;		// num�ro du tour

	Sleep(500);
	gestionAction(dofusScreen, 2, 5, 2);
	Sleep(2000);
	gestionAction(dofusScreen, 2, 5, 2);	
}


bool gestionCombat(HWND dofusScreen)
{
	bool finCombat = false, finTour = false;
	bool modeCreature = false, modeTactique = false;
	POINT posFinCombat;
	POINT posDebutCombat = { 1190, 764 };
	POINT posFinTour = { 1190, 764 };
	POINT posModeCreature = { 1195, 805 };
	POINT posModeTactique = { 1175, 805 };

	//cout << "fonction combat" << endl;

	if (detectionDebutCombat(dofusScreen, modeCreature, modeTactique))		// D�but de combat d�tect�
	{
//		cout << "Debut combat detecte" << endl;
		Sleep(1500);

		if (modeCreature) {
			leftClickResPos(posModeCreature, 1500);
		}

		if (modeTactique) {
			leftClickResPos(posModeTactique, 1500);
		}
		
		leftClickResPos(posDebutCombat, 1000);
		Sleep(2500);

		while (true)
		{
			detectionCombat(dofusScreen, finCombat, finTour, posFinCombat);

			if (finCombat)		// fin de combat d�tect�
			{
				Sleep(500);
				leftClickResPos(posFinCombat, 2000);
				return true;
			}
			else if (finTour)	// Si fin tour est d�tect�, alors c'est au tour du joueur
			{
				gestionTourJoueur(dofusScreen);
				leftClickResPos(posFinTour, 2000);
			}

			Sleep(2000);
		}
	}

	return false;
}



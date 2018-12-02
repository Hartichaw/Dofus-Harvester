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
#include "config.h"

#define COEFF_PO_SORT 55
#define COEFF_ZONE_SORT 30

using namespace std;
using namespace cv;


/****************** DETECTION COMBAT ******************/
// Certaines fonctions ne font pas grand chose mais c'est pratique pour se repérer avec le nom des fonctions ;)

vector<POINT> detectionSort(HWND dofusScreen)	// détecte la grille de sort
{
	Mat imageBGR = hwnd2mat(dofusScreen);

	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(143, 41, 24), Scalar(163, 61, 44), 278, 1257, 14, 694, 20, 1, false);

	return posArray;
}

vector<POINT> detectionDeplacement(HWND dofusScreen, int theme)	// détecte la grille de déplacement
{
	Scalar minBGR_theme0 = Scalar(60, 161, 112);	// Herbe amakna
	Scalar maxBGR_theme0 = Scalar(80, 181, 132);

	Scalar minBGR_theme1 = Scalar(43, 140, 101);	// Astrub
	Scalar maxBGR_theme1 = Scalar(63, 160, 121);

	Mat imageBGR = hwnd2mat(dofusScreen);

	if (theme == 0) {	// Amakna herbe
		vector<POINT> posArray = detectionColorArray(imageBGR, minBGR_theme0, maxBGR_theme0, 278, 1257, 14, 694, 500, 1, false);
		return posArray;
	}
	else {//if (theme == 1) {	// Astrub
		vector<POINT> posArray = detectionColorArray(imageBGR, minBGR_theme1, maxBGR_theme1, 278, 1257, 14, 694, 500, 1, false);
		return posArray;
	}
}

vector<POINT> detectionPosEnnemis(HWND dofusScreen)		// détecte la posiiton de plusieurs ennemis
{
	Mat	imageBGR = hwnd2mat(dofusScreen);

#if USER == CED
	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(200, 0, 0), Scalar(255, 60, 60), 278, 1257, 14, 733, 25, 0, false);
#elif USER == JU
	vector<POINT> posArray = detectionColorArray(imageBGR, Scalar(200, 0, 0), Scalar(255, 20, 20), 278, 1257, 14, 733, 25, 0, false);
#endif // USER == CED

	return posArray;
}

bool detectionPosJoueur(HWND dofusScreen, POINT & posJoueur)	// détecte la pos du joueur
{
	Mat	imageBGR = hwnd2mat(dofusScreen);

#if USER == CED
	if (detectionColorPos(imageBGR, Scalar(0, 0, 200), Scalar(70, 70, 255), 278, 1257, 14, 733, posJoueur, 25, false)) {	// Joueur
		return true;
	}
#elif USER == JU
	if (detectionColorPos(imageBGR, Scalar(0, 0, 200), Scalar(20, 20, 255), 278, 1257, 14, 733, posJoueur, 25, false)) {	// Joueur
		return true;
	}
#endif

	return false;
}

bool detectionDebutCombat(HWND dofusScreen, bool & modeCreature, bool & modeTactique)
{
	POINT posCible;
	Mat imageBGR = hwnd2mat(dofusScreen);

	if (detectionColorPos(imageBGR, Scalar(0, 230, 85), Scalar(50, 255, 215), 1094, 1295, 686, 775, posCible, 1000, false)) {	// detection Debut Combat

		Sleep(500);
		imageBGR = hwnd2mat(dofusScreen);		// Re-capture de l'écran pour être certain que l'écran de combat soit totalement chargé

		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1162, 1183, 775, 800, posCible, 20, false)) {	// detection mode tactique
			modeTactique = true;
		}
		else {
			modeTactique = false;
		}

		if (detectionColorPos(imageBGR, Scalar(150, 150, 150), Scalar(180, 180, 180), 1186, 1207, 775, 800, posCible, 20, false)) {	// detection mode créature

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

void detectionTheme(HWND dofusScreen, int & theme)
{
	POINT posCible;
	Mat imageBGR = hwnd2mat(dofusScreen);

	if (detectionColorPos(imageBGR, Scalar(107, 207, 192), Scalar(127, 227, 212), 278, 1257, 14, 733, posCible, 10000, false)) {	// Thème herbe amakna
		//couleur detectee (BGR) : 117, 217, 202
		// nb pixels detectés : 438697
		theme = 0;
	}
	else if (detectionColorPos(imageBGR, Scalar(92, 186, 188), Scalar(112, 206, 208), 278, 1257, 14, 733, posCible, 10000, false)) {	// Thème Astrub
	// couleur detectée : 102 196 198
		theme = 1;
	}
	else {
		cout << "Echec detection theme" << endl;
		theme = 0;		// Par défaut
	}
}

/****************** GESTION COMBAT ******************/

bool gestionAttaque(HWND dofusScreen, int sort, int poSort, int zoneSort, POINT posJoueur, POINT posEnnemi)
{
	POINT nearestPoint;
	POINT posAnnulSort = { 100, 100 };

	if (abs(posJoueur.x - posEnnemi.x) + abs(posJoueur.y - posEnnemi.y) < poSort * COEFF_PO_SORT) {	// Si on est à côté du monstre on essaie de taper

		switch (sort) {			// Sélection du sort

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

		if (findNearestPoint(posEnnemi, posSort, nearestPoint, 15, 1)) {		// 15 pour éliminer les cases en diagonale du monstre

			if (abs(posEnnemi.x - nearestPoint.x) + abs(posEnnemi.y - nearestPoint.y) < COEFF_ZONE_SORT * zoneSort) {		// Si on peut taper, on tape !!!
				leftClickResPos(nearestPoint, 2000);
				return true;			// return true si on a frappé l'ennemi
			}
		}

		leftClickResPos(posAnnulSort, 300);		// Si imposible d'attaquer on click en 100, 100 pour annuler le sort
	}

	return false;
}

bool gestionAction(HWND dofusScreen, int sort, int poSort, int zoneSort)
{
	// Param entrée : sort=0 => CAC, sort=1 => sort 1, etc...
	//				  poSort = PO du sort dans le jeu
	//				  zoneSort=1 => pas de zone, zoneSort=2 => croix de 1 case, etc...

	// Algo de la fonction :
	// Detection du thème
	// Détection de la pos des ennemis et du joueur
	// Attaque si possible, puis return true
	// Sinon déplacement puis attaque si possible

	POINT posJoueur;
	POINT posEnnemi;
	POINT nearestPoint;

	int theme;

	//cout << "gestion action" << endl;

	detectionTheme(dofusScreen, theme);

	if (!detectionPosJoueur(dofusScreen, posJoueur)) {	// détecte la pos du joueur
		cout << "echec pos joueur" << endl;
		return false;
	}

	cout << "Position joueur: " << posJoueur.x << "," << posJoueur.y << endl;

	//Sleep(200);
	vector<POINT> posEnnemis = detectionPosEnnemis(dofusScreen);		// détecte la pos des ennemis

	if (!findNearestPoint(posJoueur, posEnnemis, posEnnemi, 0, 1)) {		// détecte l'ennemi le plus proche
	//	cout << "echec pos ennemis" << endl;
		return false;
	}
	//cout << "succes pos ennemis" << endl;

	cout << "Position Monstre: " << posEnnemi.x << "," << posEnnemi.y << endl;

	if (gestionAttaque(dofusScreen, sort, poSort, zoneSort, posJoueur, posEnnemi)) {		// Si on peut attaquer sans bouger, on le fait et return true
	//	cout << "attaque reussie" << endl;
		return true;	// Si attaque réussie, return true
	}
	//cout << "attaque echouee" << endl;

	vector<POINT> posDeplacement = detectionDeplacement(dofusScreen, theme);						// Sinon on détecte la possibilité de déplacement

	if (findNearestPoint(posEnnemi, posDeplacement, nearestPoint, 15, 0)) {		// si on peut se déplacer

	//	cout << "on peut se deplacer" << endl;

		leftClickResPos(nearestPoint, (abs(posJoueur.x - nearestPoint.x) + abs(posJoueur.y - nearestPoint.y)) * 15 + 1500);	// Délai déplacment perso
		posJoueur = nearestPoint;		// MAJ de la pos du joueur

		if (gestionAttaque(dofusScreen, sort, poSort, zoneSort, posJoueur, posEnnemi)) {		// Attaque
			return true;	// Si attaque réussie, return true
		}
	}
	else {
		//		cout << "attaque reussie" << endl;
	}

	return false;
}

// A modifier en fonction du besoin
void gestionTourJoueur(HWND dofusScreen)
{
	static int tour = 0;
	tour++;		// numéro du tour

	Sleep(500);
	//bool gestionAction(HWND dofusScreen, int sort, int poSort, int zoneSort)
	gestionAction(dofusScreen, 6, 5, 2);
	Sleep(2000);
	gestionAction(dofusScreen, 6, 5, 2);
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

	if (detectionDebutCombat(dofusScreen, modeCreature, modeTactique))		// Début de combat détecté
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

		int theme;
		detectionTheme(dofusScreen, theme);

		while (true)
		{
			detectionCombat(dofusScreen, finCombat, finTour, posFinCombat);

			if (finCombat)		// fin de combat détecté
			{
				Sleep(500);
				leftClickResPos(posFinCombat, 2000);
				return true;
			}
			else if (finTour)	// Si fin tour est détecté, alors c'est au tour du joueur
			{
				gestionTourJoueur(dofusScreen);
				leftClickResPos(posFinTour, 2000);
			}

			Sleep(2000);
		}
	}

	return false;
}
/*
---------IA COMBAT---------
*/

//Renvoie la position du monstre le plus proche
void IAcombat::enemyDetection()
{}
//Déplacement besoin
void IAcombat::move()
{}
//Attaque si possible
void IAcombat::attack()
{}
//Fini le tour et le combat si possible
void IAcombat::endTurn()
{}
//Permet de savoir si on est en combat ou non
bool IAcombat::inCombat()
{
	return true;
}
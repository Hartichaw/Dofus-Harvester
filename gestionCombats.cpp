#include <iostream>
#include <vector>
#include <windows.h>
#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionImage.h"

using namespace std;

bool gestionTourJoueur(HWND dofusScreen)
{
	// Détection de la pos des ennemis
	// Détection de la possibilité de déplacement su joueur
	// Déplacement si nécessaire

	POINT posJoueur;
	POINT posEnnemi;
	POINT nearestPoint;

	//int i;

	static int cptDebug = 0;

	cptDebug++;

	cout << "cpt = " << cptDebug << endl;

	if (!detectionPositions(dofusScreen, posJoueur, posEnnemi)) {	// détecte la pos du joueur et de l'ennemi
		//return false;
	}

	cout << abs(posJoueur.x - posEnnemi.x) << "     " << abs(posJoueur.y - posEnnemi.y) << endl;

	if (abs(posJoueur.x - posEnnemi.x) > 50 || abs(posJoueur.y - posEnnemi.y) > 50) {		// Si on est pas à côté de l'ennemi, on se déplace

		vector<POINT> posDeplacement = detectionDeplacement(dofusScreen);
		
		cout << "ici 1 " << endl;

		if (!findNearestPoint(dofusScreen, posEnnemi, posDeplacement, nearestPoint, 11)) {
			cout << "non fin :(" << endl;
			Sleep(500);
			leftClick(1190, 764);	// clic fin tour
			return false;
		}

		cout << "Nearest point : " << nearestPoint.x << "  " << nearestPoint.y << endl;
		Sleep(500);
		leftClick(nearestPoint.x, nearestPoint.y);
		Sleep(500);
		SetCursorPos(100, 100);
		Sleep(3000);
	}
	Sleep(2000);

	// Attaque
	//int KEY = 0x22;
	//PostMessage(dofusScreen, WM_KEYDOWN, 0x22, 0);
	Sleep(200);
	leftClick(865, 748);					// Les touches ne marchent pas, pourquoi ???
	//PostMessage(dofusScreen, WM_KEYUP, 0x22, 0);
	Sleep(200);

	vector<POINT> posSort = detectionSort(dofusScreen);

	if (findNearestPoint(dofusScreen, posEnnemi, posSort, nearestPoint, 0)) {

		cout << "weigh = " << abs(posEnnemi.x - nearestPoint.x) + abs(posEnnemi.y - nearestPoint.y) << endl;

		if (abs(posEnnemi.x - nearestPoint.x) + abs(posEnnemi.y - nearestPoint.y) < 25) {

			Sleep(500);
			leftClick(nearestPoint.x, nearestPoint.y);
			Sleep(1500);
		}
	}
	else {
		Sleep(500);
		leftClick(1190, 764);	// clic fin tour
		return false;
	}


/*	// pour debug :
	for (i = 0; i < posDeplacement.size(); i++) {
		cout << "X = " << posDeplacement[i].x << "   Y = " << posDeplacement[i].y << endl;
		SetCursorPos(posDeplacement[i].x, posDeplacement[i].y);
		Sleep(300);
	}
	cout << "Xe = " << posEnnemi.x << "   Ye = " << posEnnemi.y << endl;
	cout << "Xj = " << posJoueur.x << "   Yj = " << posJoueur.y << endl;
	cout << "Xj-Xe = " << posJoueur.x - posEnnemi.x << "   Yj-Ye = " << posJoueur.y - posEnnemi.y << endl;

	*/


	Sleep(1000);
	leftClick(1190, 764);	// clic fin tour

	return true;
}

bool gestionCombat(HWND dofusScreen)
{
	bool finCombat = false, finTour = false;
	bool modeCreature = false, modeTactique = false;
	DWORD t1, t2;
	
	cout << "fonction combat" << endl;

	if (detectionDebutCombat(dofusScreen, modeCreature, modeTactique))		// Début de combat détecté
	{
//		cout << "Debut combat detecte" << endl;
		//Sleep(1000);

		// Clic mode créature et tactique si besoin
		if (modeCreature) {
			leftClick(1195, 805);
			Sleep(1000);
		}
		if (modeTactique) {
			leftClick(1175, 805);
			Sleep(1000);
		}
		
		leftClick(1190, 764);	// clic Prêt combat
		
		Sleep(500);		
		SetCursorPos(100, 100);
		Sleep(500);

		while (true)
		{
			detectionCombat(dofusScreen, finCombat, finTour);

			if (finCombat)		// fin de combat détecté
			{
				//Sleep(2000);
				leftClick(786, 497);
				Sleep(500);
				SetCursorPos(100, 100);
				Sleep(1500);
				cout << "Fin de combat detecte" << endl;
				return true;
			}
			else if (finTour)	// Si fin tour est détecté, alors c'est au tour su joueur
			{
				Sleep(500);
				//t1 = GetTickCount();
				gestionTourJoueur(dofusScreen);
				//t2 = GetTickCount();
				//cout << "Temps execution : " << t2 - t1 << " ms" << endl;
				Sleep(500);
				SetCursorPos(0, 0);
				Sleep(2500);
			}

			Sleep(2000);
		}
	}
	return false;
}



#include <iostream>
#include <windows.h>
#include <vector>
#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionPopUp.h"
#include "DetectionRessource.h"


using namespace std;


void recolterRessourcesMap(HWND dofusScreen)
{
    // Récolte toutes les ressources sur la map
    // S'il n'y a aucune ressource sur la map on sort immédiatement de la fonction

	// Détection des ressources

	vector<vector<int>> posRessources = scanRessource(dofusScreen);


	/*vector<POINT> posRessources(5);		// Simulation des pos des ressources

	posRessources[0].x = 605;
	posRessources[0].y = 465;

	posRessources[1].x = 649;
	posRessources[1].y = 490;

	posRessources[2].x = 562;
	posRessources[2].y = 490;

	posRessources[3].x = 509;
	posRessources[3].y = 524;

	posRessources[4].x = 505;
	posRessources[4].y = 558;

	*/

	if (posRessources.size() != 0)			// Si aucune ressource n'est détectée
	{
		for (int i = 0; i < posRessources.size(); i++) {

			leftClick(posRessources[i][1], posRessources[i][0]+22);

			if (i == 0) {				// Délai à la 1ere ressource pour le le perso ait le temps de se rapproher de la ressource
				Sleep(1500);
			}

			gestionCombat(dofusScreen);
			gestionPopUps(dofusScreen);

			Sleep(500);
		}

	/*	for (int i = 0; i < posRessources.size(); i++) {

			Sleep(500);		// Delai de 2 seconde par ressources à la fin
			gestionCombat(dofusScreen);
			gestionPopUps(dofusScreen);
		}
		*/
		

	}

}


void recolterChampsBonta(HWND dofusScreen)
{
    // Trajet démarre au zaap champs de cania
    int i, j;

    moveMapRight();
    moveMapUp();

    for (i=0 ; i<3 ; i++) {
        recolterRessourcesMap(dofusScreen);
        moveMapLeft();
    }
	recolterRessourcesMap(dofusScreen);

    moveMapLeft();
    moveMapUp();

    for (i=0 ; i<5 ; i++) {
        recolterRessourcesMap(dofusScreen);
        moveMapRight();
    }
	recolterRessourcesMap(dofusScreen);

    for (i=0 ; i<4 ; i++) {
        moveMapRight();
    }
	moveMapUp();

	for (i = 0; i < 10; i++) {
		recolterRessourcesMap(dofusScreen);
		moveMapLeft();
	}
    moveMapUp();

    // -31, -40
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 10; i++) {
			recolterRessourcesMap(dofusScreen);
			moveMapRight();
		}
		recolterRessourcesMap(dofusScreen);
		moveMapUp();

		for (i = 0; i < 10; i++) {
			recolterRessourcesMap(dofusScreen);
			moveMapLeft();
		}
		recolterRessourcesMap(dofusScreen);
		moveMapUp();
	}
}

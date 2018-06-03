#include <iostream>
#include <windows.h>
#include <vector>
#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionPopUp.h"
#include "DetectionRessource.h"

#include "gestionImage.h"


using namespace std;


void recolterRessourcesMap(HWND dofusScreen)
{
    // Récolte toutes les ressources sur la map
    // S'il n'y a aucune ressource sur la map on sort immédiatement de la fonction

	// Détection des ressources

	POINT posRessources;



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

	do		// Si aucune ressource n'est détectée
	{
		posRessources = scanRessource(dofusScreen);
		cout << "ressource located:" << posRessources.x << "," << posRessources.y << endl;
		if (posRessources.x >= 0 && posRessources.y >= 0)
		{
			leftClick(posRessources.x, posRessources.y);
		}
		
		Sleep(3000);
	}while (posRessources.x >= 0 && posRessources.y >= 0);

	cout << "plus de ressources sur la map" << endl;
	

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

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

	// Détection des ressources

	POINT posRessources{0,0};

	harvestManager recolteur = harvestManager(dofusScreen);
	do		// Si aucune ressource n'est détectée
	{
		posRessources = recolteur.getRessourcePos(posRessources);
		//cout << "ressource located:" << posRessources.x << "," << posRessources.y << endl;
		if (posRessources.x >= 0 && posRessources.y >= 0)
			leftClick(posRessources.x, posRessources.y);
		
		
		Sleep(4000);
		//gestionCombat(dofusScreen);
		//gestionPopUps(dofusScreen);
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
        //recolterRessourcesMap(dofusScreen);
        moveMapLeft();
    }
	//recolterRessourcesMap(dofusScreen);

    moveMapLeft();
    moveMapUp();

    for (i=0 ; i<5 ; i++) {
        //recolterRessourcesMap(dofusScreen);
        moveMapRight();
    }
	//recolterRessourcesMap(dofusScreen);

    for (i=0 ; i<4 ; i++) {
        moveMapRight();
    }
	moveMapUp();

	for (i = 0; i < 10; i++) {
		//recolterRessourcesMap(dofusScreen);
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

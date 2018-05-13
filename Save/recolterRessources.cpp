
#include <windows.h>
#include "gestionDeplacement.h"





void recolterRessourcesMap()
{
    // Récolte toutes les ressources sur la map
    // S'il n'y a aucune ressource sur la map on sort immédiatement de la fonction
}




void recolterChampsBonta()
{
    // Trajet démarre au zaap champs de cania
    int i, j;

    moveMapRight();
    moveMapUp();

    for (i=0 ; i<3 ; i++) {
        recolterRessourcesMap();
        moveMapLeft();
    }
	recolterRessourcesMap();

    moveMapLeft();
    moveMapUp();

    for (i=0 ; i<5 ; i++) {
        recolterRessourcesMap();
        moveMapRight();
    }
	recolterRessourcesMap();

    for (i=0 ; i<4 ; i++) {
        moveMapRight();
    }
	moveMapUp();

	for (i = 0; i < 10; i++) {
		recolterRessourcesMap();
		moveMapLeft();
	}
    moveMapUp();

    // -31, -40
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 10; i++) {
			recolterRessourcesMap();
			moveMapRight();
		}
		recolterRessourcesMap();
		moveMapUp();

		for (i = 0; i < 10; i++) {
			recolterRessourcesMap();
			moveMapLeft();
		}
		recolterRessourcesMap();
		moveMapUp();
	}
}

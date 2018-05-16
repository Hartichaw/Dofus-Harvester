#include <iostream>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "gestionDeplacement.h"
#include "gestionCombats.h"
#include "gestionInterface.h"
#include "recolterRessources.h"
#include "gestionImage.h"
#include "gestionPopUp.h"

// Test Git !!!!!!!!!!!!

using namespace std;
using namespace cv;


bool init(HWND & hWnd)
{	
	Sleep(1000);

    hWnd = FindWindowA(NULL, "Sac-Lerieur - Dofus 2.46.15:0");
	if (hWnd == NULL)
    {
        cout << "Dofus n'est pas ouvert" << endl;
        system("pause");
        return false;
    }

    // Dofus est ouvert
    SetForegroundWindow(hWnd);
    SetCursorPos(0, 0); // X, Y
    Sleep(2000);

    return true;
}


int main(/*int argc, char** argv*/)
{
	HWND dofusScreen;
	DWORD t1, t2;

	if (!init(dofusScreen)) {
		return -1;
	}
//	cout << "avant detection combat" << endl;

//	bool modeCreature = false, modeTactique = false;
//	detectionDebutCombat(dofusScreen, modeCreature, modeTactique);

	//cout << "après detection combat" << endl;


	//vector<POINT> vec = detectionPopUps(dofusScreen);

	
	while (1)
	{
	
		gestionCombat(dofusScreen);
	
		t1 = GetTickCount();
		if (gestionPopUps(dofusScreen)) {

		}
		t2 = GetTickCount();	
		cout << "Temps d'execution gestion pop-ups : " << t2 - t1 << endl;

		Sleep(1000);
	}
	
	//cout << "après detection combat" << endl;
   // trajetAstrub();
    //trajetBanqueAstrub();
    //gestionBanqueAstrub();  // Décharge tous l'inventaire dans la banque

	/*trajetChampsBonta();

    recolterChampsBonta();
	*/
 
	cout << "Fini !" << endl;

    return 0;

}

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

using namespace std;
using namespace cv;

//const LPCSTR NAME_DOFUS_WINDOW = "Hartichaw - Dofus 2.46.14:3";
const LPCSTR NAME_DOFUS_WINDOW = "Sac-Lerieur - Dofus 2.46.15:0";

bool init(HWND & hWnd)
{	
	Sleep(1000);

    hWnd = FindWindowA(NULL, NAME_DOFUS_WINDOW);
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

    //trajetAstrub();
    //trajetBanqueAstrub();
    //gestionBanqueAstrub();  // D�charge tous l'inventaire dans la banque
	
//	trajetChampsBonta();

 //   recolterChampsBonta(dofusScreen);
	

	recolterRessourcesMap(dofusScreen);

	waitKey(0);

 
	/*	// Debug gestion combat et pop-up
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

	*/


	cout << "Fini !" << endl;

    return 0;

}

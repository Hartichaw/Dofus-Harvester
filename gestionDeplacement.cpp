
#include <windows.h>

/** FONCTIONS GENERALES **/

void leftClick(int x, int y)
{
    SetCursorPos(x, y);
	Sleep(50);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(70);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void leftClickResPos(POINT pt, int delay)	// Click gauche puis reset de la position du curseur à (0, 0) et délai
{
	leftClick(pt.x, pt.y);
	Sleep(delay / 2);
	SetCursorPos(0, 0);
	Sleep(delay / 2);
}


/** CLIC HAVRE SAC **/

void clickHavreSac()
{
    leftClick(1393,778);
    Sleep(2000);
}

void clickZaapHavreSac()
{
    leftClick(622,210);
    Sleep(1000);
}

void doubleClickAstrub()
{
    leftClick(718,250);
    Sleep(50);
    leftClick(718,250);
    Sleep(3500);
}

void doubleClickChampsBonta()
{
    leftClick(694,323);
    Sleep(50);
    leftClick(694,323);
    Sleep(3500);
}

/** CLIC ASTRUB **/

void clickBankAstrub()
{
    leftClick(901,310);
    Sleep(5000);
}

/** CHANGEMENT MAP **/

void moveMapUp()
{
    leftClick(747,27);
    Sleep(6500);
}

void moveMapLeft()
{
    leftClick(161,446);
    Sleep(6500);
}

void moveMapDown()
{
    leftClick(820,727);
    Sleep(6500);
}

void moveMapRight()
{
    leftClick(1406,415);
    Sleep(6500);
}

/** TRAJETS **/

void trajetAstrub()
{
    clickHavreSac();
    clickZaapHavreSac();
    doubleClickAstrub();

}

void trajetBanqueAstrub()
{
    moveMapLeft();
    clickBankAstrub();

}

void trajetChampsBonta()
{
    clickHavreSac();
    clickZaapHavreSac();
    doubleClickChampsBonta();
}


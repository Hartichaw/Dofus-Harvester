
#include <windows.h>
#include "gestionDeplacement.h"

void gestionBanqueAstrub()
{
    leftClick(794,303);     // Banquier
    Sleep(2000);
    leftClick(715,605);     // Consulter compte
    Sleep(2000);
    leftClick(1011,120);    // Transferts avanc�s
    Sleep(2000);
    leftClick(1083,130);    // Transf�rer tous les objets
    Sleep(2000);
    leftClick(1246,90);     // Fermeture
    Sleep(3000);
}

void gestionNiveau()
{
    // Si passage niveau d�tect�
    // fermeture de la notification
}

void gestionInteractions()
{
    // Si une une demande d'�change ou de combat est d�tect�e
    // fermeture de la notification
}

void gestionNotifications()
{
    // Si une notification est d�tect�e
    // fermeture des notificaions de caca d'Ankama sur le c�t� gauche de l'interface
}

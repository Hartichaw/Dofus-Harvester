
#include <windows.h>
#include "gestionDeplacement.h"

void gestionBanqueAstrub()
{
    leftClick(794,303);     // Banquier
    Sleep(2000);
    leftClick(715,605);     // Consulter compte
    Sleep(2000);
    leftClick(1011,120);    // Transferts avancés
    Sleep(2000);
    leftClick(1083,130);    // Transférer tous les objets
    Sleep(2000);
    leftClick(1246,90);     // Fermeture
    Sleep(3000);
}

void gestionNiveau()
{
    // Si passage niveau détecté
    // fermeture de la notification
}

void gestionInteractions()
{
    // Si une une demande d'échange ou de combat est détectée
    // fermeture de la notification
}

void gestionNotifications()
{
    // Si une notification est détectée
    // fermeture des notificaions de caca d'Ankama sur le côté gauche de l'interface
}

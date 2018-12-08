#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

bool gestionCombat(HWND inpusScreen);

#ifndef DEF_IACOMBAT
#define DEF_IACOMBAT

class IAcombat
{
public:
	/* ----- Constructeurs ----- */
	// Le code dans un constructeur est ex�cut� � la d�claration de l'objet
	IAcombat(HWND inputScreen)
	{// mais pour l'instant on ex�cute rien � la d�claration
		dofusScreen = inputScreen;
	}

	/* ----- M�thodes Publiques ----- */
	// Les m�thodes publiques repr�sentes les diff�rentes actions que l'IA sera ammen�es � effectuer

	//Renvoie la position du monstre le plus proche
	void enemyDetection();
	//D�placement besoin
	void move();
	//Attaque si possible
	void attack();
	//Fini le tour et le combat si possible
	void endTurn();
	//Permet de savoir si on est en combat ou non
	bool inCombat();
	
	bool detectFinCombat(POINT & posFinCombat);

	bool detectTourJoueur();

	// Pr�pare au combat : mode cr�ature et tactique + choix de la position sur la map + clique sur pr�t
	void prepareCombat();

	void detectTheme();

	




private:

	/* ----- M�thodes Priv�es ----- */
	// Les m�thodes sont des fonctions "cach�es" qui sont utilisables par les actions/m�thodes publiques mais pas par l'utilisateur de l'IA

	/* ----- Attributs ----- */
	// donn�es que l'on doit sauvegarder pour pouvoir effectuer les actions/m�thodes
	/*
	/!\ un attribut ne doit jamais �tre public
	*/
	std::vector<POINT> gridMap;			// grille de combat
	std::vector<POINT> availableSpells;	// Position des sorts � utiliser

	POINT posJoueur;
	POINT posEnemy;

	int theme;

	/* ----- Constantes ----- */

	int COEFF_PO_SORT = 55;
	int COEFF_ZONE_SORT = 30;

	//themes
	cv::Scalar minBGR_theme0 = cv::Scalar(60, 161, 112);	// Herbe amakna
	cv::Scalar maxBGR_theme0 = cv::Scalar(80, 181, 132);
	cv::Scalar minBGR_theme1 = cv::Scalar(43, 140, 101);	// Astrub
	cv::Scalar maxBGR_theme1 = cv::Scalar(63, 160, 121);

	//Positions boutons divers
	POINT posDebutCombat = { 1190, 764 };
	POINT posFinTour = { 1190, 764 };
	POINT posModeCreature = { 1172, 805 };
	//POINT posModeTactique = { 1172, 805 };
	POINT posModeTactique = { 1148, 805 };

	HWND dofusScreen;
};

#endif

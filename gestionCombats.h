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
	// Le code dans un constructeur est exécuté à la déclaration de l'objet
	IAcombat(HWND inputScreen)
	{// mais pour l'instant on exécute rien à la déclaration
		dofusScreen = inputScreen;
	}

	/* ----- Méthodes Publiques ----- */
	// Les méthodes publiques représentes les différentes actions que l'IA sera ammenées à effectuer

	//Renvoie la position du monstre le plus proche
	void enemyDetection();
	//Déplacement besoin
	void move();
	//Attaque si possible
	void attack();
	//Fini le tour et le combat si possible
	void endTurn();
	//Permet de savoir si on est en combat ou non
	bool inCombat();
	
	bool detectFinCombat(POINT & posFinCombat);

	bool detectTourJoueur();

	// Prépare au combat : mode créature et tactique + choix de la position sur la map + clique sur prêt
	void prepareCombat();

	void detectTheme();

	




private:

	/* ----- Méthodes Privées ----- */
	// Les méthodes sont des fonctions "cachées" qui sont utilisables par les actions/méthodes publiques mais pas par l'utilisateur de l'IA

	/* ----- Attributs ----- */
	// données que l'on doit sauvegarder pour pouvoir effectuer les actions/méthodes
	/*
	/!\ un attribut ne doit jamais être public
	*/
	std::vector<POINT> gridMap;			// grille de combat
	std::vector<POINT> availableSpells;	// Position des sorts à utiliser

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

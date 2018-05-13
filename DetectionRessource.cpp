#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <windows.h>
#include <Winuser.h>



using namespace std;
using namespace cv;



/*test commit*/

//CONSTANTES A MODIFIER SI BESOIN
const int DEBUG = 1; // 0 pour ne pas display les images
const LPCSTR NAME_DOFUS_WINDOW = "Hartichaw - Dofus 2.46.14:3";
const String TEMPLATE_ICON_PAYSAN = "C:/Users/cedri/Pictures/cursor_paysan.png";

//CONSTANTES A PAS TOUCHER (sauf si tu sais ce que tu fais )

const int SIZE_SEARCH_ZONE = 10;
const int PRESS_Y = 1;
const int DETECTION_THRESHOLD = 5;
const int SIZE_RECT = 15;
const Scalar RED = Scalar(5, 5, 255);
const Scalar BLUE = Scalar(255, 5, 5);

const float ROW_START_COEFF = 0.04074, ROW_STEP_COEFF = 0.04074, ROW_END_COEFF = 0.8055;
const float COL_START_COEFF = 0.20520, COL_STEP_COEFF = 0.02227, COL_END_COEFF = 0.8437;

Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	cvtColor(src, src, CV_BGRA2BGR);
																									   
	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

Mat getCursor() // en chantier
{
	
	Mat OutputCursorImg; // matrice de  sortie

	// On récupère l'environnement de la fenêtre
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// Création de la bitmap et du canvas dans lequel on va recuperer l'image.
	HBITMAP hbmCanvas = CreateCompatibleBitmap(hdcScreen, 256, 256);
	HGDIOBJ hbmOld = SelectObject(hdcMem, hbmCanvas);

	// Get information about the global cursor.
	CURSORINFO ci;
	ci.cbSize = sizeof(ci);
	GetCursorInfo(&ci);

	// information de l'icon (conversion HBM -> MAT)
	ICONINFO ii = { 0 };
	BITMAP bm;
	GetIconInfo(ci.hCursor, &ii);
	GetObject(ii.hbmMask, sizeof(BITMAP), &bm);

	// On transfère les données du curseur dans le canvas.
	DrawIcon(hdcMem, 0, 0, ci.hCursor);
	
	

	//Si les informmation remontées sont cohérentes
	if (bm.bmHeight > 0 && bm.bmWidth > 0)
	{
		
		Mat image(bm.bmHeight, bm.bmWidth, CV_8UC3);
		// On parcourt chaque pixel pour mettre à jour la matrice de sortie
		for (int i = 0; i < bm.bmHeight; i++) {
			for (int j = 0; j < bm.bmWidth; j++) {

				COLORREF c = GetPixel(hdcMem, i, j);
				
				image.at<cv::Vec3b>(i, j)[0] = (int)GetBValue(c);
				image.at<cv::Vec3b>(i, j)[1] = (int)GetGValue(c);
				image.at<cv::Vec3b>(i, j)[2] = (int)GetRValue(c);

			}
		}
		OutputCursorImg = image;
	}


	return OutputCursorImg;
}

Mat imgProvider(int PRESS_Y=0) {

	//capture l'image de la fenêtre dofus
	//si PRESS_Y=1 on appuie sur y avant de prendre la capture
	
	HWND desktopImgHWND = FindWindowA(NULL, NAME_DOFUS_WINDOW);
	if (desktopImgHWND == NULL)
	{
		cout << "Dofus n'est pas ouvert" << endl;
		system("pause");
		
	}

	SetForegroundWindow(desktopImgHWND);
	if( PRESS_Y == 1){Sleep(100);PostMessage(desktopImgHWND, WM_KEYDOWN, 0x59, 0);} //press y
	Sleep(200);
	Mat desktopImgMAT = hwnd2mat(desktopImgHWND);
	if (PRESS_Y == 1) { Sleep(100); PostMessage(desktopImgHWND, WM_KEYUP, 0x59, 0); } //release y

	return desktopImgMAT;
}

vector<vector<int>> scanRessource()
{

	/*
		VARIABLES
	*/

	//Image recuperee de la fenetre dofus
	Mat Img = imgProvider(PRESS_Y); // pressing y


	// Variables nécessaires a la creation de la grille de points
	int ROW_NB = Img.rows;
	int COL_NB = Img.cols;

	int ROW_START = int(ROW_NB*ROW_START_COEFF);
	int	ROW_STEP = int(ROW_NB*ROW_STEP_COEFF);
	int	ROW_END = int(ROW_NB*ROW_END_COEFF);

	int COL_START = int(COL_NB * COL_START_COEFF);
	int	COL_STEP = int(COL_NB * COL_STEP_COEFF);
	int	COL_END = int(COL_NB * COL_END_COEFF);

	int scoredifference;

	//Matrices pour stocker les resultats intermediraires
	Mat  rsz_img, thresh_img, morpho_output, mask,displayImg, regionCurseur;
	Mat ImgRecolte = imread(TEMPLATE_ICON_PAYSAN, CV_LOAD_IMAGE_COLOR);

	//Tableau de sortie pour stocker les coordonnees des ressources
	vector<vector<int>>  tabRessources,outputTab;
	vector<int> RessCoordinates(2);
	
	// seuils de couleurs necessaires pour detourer les ressources
	Scalar lower_color = Scalar(160, 190, 200);
	Scalar upper_color = Scalar(248, 245, 235);

	// kernels des operations morphologiques
	Mat kernel_erode = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat kernel_dilate = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));


	/*
	PROCESSING
	*/

	
	// on seuille l'image pour ne garder que les couleurs proches du contour des ressources
	// en sortie: img binaire, 1=couleur du contour , 0 sinon
	inRange(Img, lower_color, upper_color, thresh_img); 

	// erosion/ dilatation, supprimes les petits pixels assimilies a du bruit
	morphologyEx(thresh_img, mask, MORPH_ERODE, kernel_erode);
	morphologyEx(mask, morpho_output, MORPH_DILATE, kernel_dilate);



	//Detection de la possition des ressources
	// on projette une grille sur la map et on regarde les points de la grille qui sont proche des ressources
	for(int row = ROW_START; row < ROW_END;  row = row + ROW_STEP)
	{
		for(int col = COL_START; col < COL_END; col = col + COL_STEP)

		{

			//Pour chaque point de la grille on regarde si il y a des pixel à 1 dans le voisinage
			int rowTopLeft = row - SIZE_SEARCH_ZONE; if (rowTopLeft < 0) { rowTopLeft = 0; }
			int colTopLeft = col - SIZE_SEARCH_ZONE; if (colTopLeft < 0) { colTopLeft = 0; }
			int rowBottomRight = row + SIZE_SEARCH_ZONE; if (rowBottomRight > morpho_output.rows) { rowTopLeft = morpho_output.rows; }
			int colBottomRight = col + SIZE_SEARCH_ZONE; if (colBottomRight > morpho_output.cols) { rowTopLeft = morpho_output.cols; }
			Mat ROI = morpho_output(Range(rowTopLeft, rowBottomRight), Range(colTopLeft, colBottomRight));
			int score = countNonZero(ROI);

			if (score > DETECTION_THRESHOLD)
			{
				//Si on a détecté un point on l'enregistre
				RessCoordinates[0]=row;
				RessCoordinates[1]=col;
				tabRessources.push_back(RessCoordinates);
			}

			// afficher dans image
			//if (DEBUG == 1) { circle(morpho_output, Point(col, row), 3, BLUE); }
			//if (DEBUG == 1) { circle(Img, Point(col, row), 3, BLUE); }
		}
	}

	
	// Controle des points trouvés pour ne garder que les ressoruces récoltables
	for (int i = 0; i<tabRessources.size(); i++)
	{

		SetCursorPos(tabRessources[i][1], tabRessources[i][0]);
		regionCurseur = getCursor();
		//imwrite(string{ "C:/Users/cedri/Pictures/curseur/Cursor" + to_string(i) + ".png"}, regionCurseur);

		if (ImgRecolte.rows == regionCurseur.rows && ImgRecolte.cols == regionCurseur.cols)
		{
			subtract(ImgRecolte, regionCurseur, regionCurseur);
			scoredifference = (int)sum(sum(regionCurseur))[0];

			
			if (scoredifference == 0)
			{
				outputTab.push_back(tabRessources[i]);
			}
		}
		else
		{
			cout << "Les curseurs n'ont pas la même dimension" << endl;
		}
		Sleep(200);
	}


	
	// Windows creation in debug mode
	if (DEBUG == 1)
	{

		for (int i = 0; i<outputTab.size(); i++)
		{
			rectangle(Img, Point(outputTab[i][1] - SIZE_RECT, outputTab[i][0] - SIZE_RECT), Point(outputTab[i][1] + SIZE_RECT, outputTab[i][0] + SIZE_RECT), RED, 4);
		}
		namedWindow("Provided image", WINDOW_NORMAL);
		//imwrite("C:/Users/cedri/Pictures/Desktop_capture.png", Img);
		resize(Img, displayImg, Size(0,0),0.5,0.5);
		imshow("Provided image", displayImg);
		//imwrite("C:/Users/cedri/Pictures/Bot-Img_and_grid.png", displayImg);

		namedWindow("Morpho Output", WINDOW_NORMAL);
		resize(morpho_output, displayImg, Size(0, 0), 0.5, 0.5);
		imshow("Morpho Output", displayImg);
		//imwrite("C:/Users/cedri/Pictures/Bot-gray.png", displayImg);
		
		
		
		

	}

	return outputTab;

	

}

/*

Bool isCursorBlue(Mat roi, threshold)
{

	Score = cv.CountNonZero(ROI)

		If score > threshold
			Return true
		Return false
}


void isHarvestable(vector &Tab_ressource)
{

for(  ressource in Tab_ressource)
{
//
SetCursor(row, col)

//capture de l'image du desktop
HWND desktopImgHWND = FindWindowA(NULL,"nom fenêtre");
Mat desktopImgMAT = hwnd2mat(desktopImgHWND)

// on zoom sur la zone du curseur
Mat regionCurseur = desktopImgMAT(range(row - 10, col - 10), range(row + 10, row - 10))

// Si le curseur et rouge et pas bleu,
if(not  scanPixNb(regionCurseur, THRESH_BLUE_CURSOR))
{removeRessource(Tab_ressource, Ressource)}
}
}





Main #######



FOR each map

DO

DO

Tab_ressource = scanRessource()

isHarvestable(&Tab_ressource) //met à jour la liste des ressources en ne laissant celles récoltables                   

IF Tab_ressource = empty

THEN

Break // on sort du while

harvest(Tab_ressource)

WHILE(1)

DONE*/
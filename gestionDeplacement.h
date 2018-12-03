#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>

void leftClick(int x, int y);
void leftClickResPos(POINT pt, int delay);

void moveMapUp();
void moveMapLeft();
void moveMapDown();
void moveMapRight();

//void ClickZaapHavreSac();
//void ClickHavreSac();

void trajetAstrub();
void trajetBanqueAstrub();
void trajetChampsBonta();

#ifndef DEF_NAVIGATOR
#define DEF_NAVIGATOR

class navigator
{
public:
	navigator()
	{}

	void travelCircuit(std::string name);
	bool nextMap();


private:

	int indexTrajet = 0;
	std::vector<char> trajet;
	std::map<std::string, std::vector<std::string>> travelMap;

};

#endif
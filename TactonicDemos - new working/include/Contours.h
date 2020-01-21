
#pragma once

#include <math.h>
#include <stdlib.h>
#include <fstream>
using namespace std;
#define MAXLOOPS 1000

class Contours
{
public:
	
int w, h;
int *tmp;
int nLoops;
int size[MAXLOOPS];
int X[MAXLOOPS][MAXLOOPS];
int Y[MAXLOOPS][MAXLOOPS];
int Z[MAXLOOPS][MAXLOOPS];
int D[MAXLOOPS][MAXLOOPS];

	Contours(void);
	~Contours(void);
	void init(int width, int height);
	void find(int *img, int cut);
	void createLoop(int img[], int tmp[], int x, int y, int cut, int loop);
	bool isValidLoop(int loop);
	void trimEdges(int img[]);
	void smooth();
	int xy(int x, int y);
	int getX(int loop, int i);
	int getY(int loop, int i);
	int getNLoops();
	int getLoopSize(int loop);
	int getZ(int loop, int i);
	int getD(int loop, int i);
	int* getX(int loop);
	int* getY(int loop);
};
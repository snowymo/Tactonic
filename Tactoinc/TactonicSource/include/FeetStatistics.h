#include <string>
#include <cmath>

const int nCols = 80;
const int nRows = 48;
struct CENTROID{
	int col;
	int row;
};

struct BOUND{
	int left;
	int right;
	int top;
	int bottom;
};

class FeetStatistics
{
public:
	FeetStatistics();
	~FeetStatistics();

	int* src;
	int* dst;
	int * data;

	BOUND bBothfeet;
	BOUND bLeft;
	BOUND bRight;
	CENTROID cLeft;
	CENTROID cRight;
	CENTROID cBothfeet;

	//void smoothData(int* src, int* dst);
	int get(int *src, int col, int row);
	void set(int *dst, int col, int row, int value);
	void computeCentroid(int* data, CENTROID &dst, int colLo, int colHi);
	void computeBounds(int* data, BOUND b, int colLo, int colHi);
};
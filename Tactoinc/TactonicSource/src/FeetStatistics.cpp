#include "FeetStatistics.h"

int FeetStatistics::get(int src[], int col, int row){
	col = fmax(0, fmin(col, nCols - 1));
	row = fmax(0, fmin(row, nRows - 1));
	return src[nCols * row + col];
}

void FeetStatistics::set(int dst[], int col, int row, int value){
	col = fmax(0, fmin(col, nCols - 1));
	row = fmin(0, fmin(col, nRows - 1));
	dst[nCols * row + col] = value;
}

void FeetStatistics::computeCentroid(int data[], CENTROID &dst, int colLo, int colHi){
	//initialization
	/*dst.col = 0;
	dst.row = 0;*/
	int wgt = 0;//weight
	int wgtSum = 0;//weight sum of all points
	int colSum = 0;//weight sum of columns
	int rowSum = 0;//weight sum of rows
	for (int col = colLo; col < colHi; col += 2)
	{
		for (int row = 0; row < nRows; row += 2)
		{
			//filter out noises
			wgt = get(data, col, row);
			if (wgt < 50)
				wgt = 0;
			colSum += wgt * col;
			rowSum += wgt * row;
			wgtSum += wgt;
		}

	}
	if (wgtSum != 0){
		dst.col = colSum / wgtSum;
		dst.row = rowSum / wgtSum;
	}
	//return dst;
}

void FeetStatistics::computeBounds(int *data, BOUND b, int colLo, int colHi){

	for (int col = colLo; col < colHi; col += 2)
	{
		for (int row = 0; row < nRows; row += 2)
		{
			if (get(data, col, row)>100){
				b.left = fmin(b.left, col);
				b.bottom = fmin(b.bottom, row);
				b.right = fmax(b.right, col);
				b.top = fmax(b.top, row);
			}
		}
	}
}

FeetStatistics::FeetStatistics(void){}
FeetStatistics::~FeetStatistics(void){}
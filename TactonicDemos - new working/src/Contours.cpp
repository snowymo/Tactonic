
#include "Contours.h"

int map[16] = { 0, 4, 3, 7, 9, 8, 177, 11, 14, 228, 2, 6, 13, 12, 1, 0 };

int Contours::getNLoops() { return nLoops; }
int Contours::getLoopSize(int loop) { return size[loop]; }
int Contours::getX(int loop, int i) { return X[loop][i]; }
int Contours::getY(int loop, int i) { return Y[loop][i]; }
int Contours::getZ(int loop, int i) { return Z[loop][i]; }
int Contours::getD(int loop, int i) { return D[loop][i]; }
int* Contours::getX(int loop) { return X[loop]; }
int* Contours::getY(int loop) { return Y[loop]; }

Contours::Contours(void) {
	nLoops = 0;
}

void Contours::init(int width, int height){
    w = width;
    h = height;
    tmp = new int[w * h];
}


   int Contours::xy(int x, int y) {
      x = (x + w) % w;
      y = (y + h) % h;
      return x + w * y;
   }


void Contours::smooth() {
    for (int loop = 0 ; loop < nLoops ; loop++) {
        int *_x = X[loop];
        int *_y = Y[loop];
        int n = size[loop];
        for (int i = 0 ; i < n ; i++) {
        int i0 = (i - 1 + n) % n;
        int i1 = (i + 1    ) % n;
        _x[i] = _x[i0] + _x[i] + _x[i] + _x[i1] >> 2;
        _y[i] = _y[i0] + _y[i] + _y[i] + _y[i1] >> 2;
        }
    }
}
void Contours::trimEdges(int img[]) {
    for (int y = 0 ; y < h ; y++)
        img[w * y] = img[xy(w-1, y)] = 0;
    for (int x = 0 ; x < w ; x++)
        img[x] = img[xy(x, h-1)] = 0;
}

bool Contours::isValidLoop(int loop) {
    for (int n = 0 ; n < getLoopSize(loop) ; n++) {
        int n2 = (n + 1) % getLoopSize(loop);
        int x1 = getX(loop, n ) >> 8;
        int y1 = getY(loop, n ) >> 8;
        int x2 = getX(loop, n2) >> 8;
        int y2 = getY(loop, n2) >> 8;
        if ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) > 100)
        return false;
    }
    return true;
}

void Contours::createLoop(int img[], int tmp[], int x, int y, int cut, int loop) {
    int m = tmp[xy(x, y)];
    int in = m & 3;

    int *_x = X[loop];
    int *_y = Y[loop];
    int *_z = Z[loop];
    int *_d = D[loop];

    int n = 0;
    for ( ; tmp[xy(x, y)] != 0 ; n++) {
        int i = xy(x, y);

        if ((tmp[i] & 15) == 0)
        tmp[i] >>= 4;

        int inOut = tmp[i] & 15;
        tmp[i] &= 0xf0;

        bool is12 = inOut == 12;

        int out = inOut >> 2;
        if (out == in)
        out = inOut & 3;

        int x0 = x + (out + 3 >> 1 & 1);
        int y0 = y + (out     >> 1    );

        int x1 = x + (out     >> 1    );
        int y1 = y + (out + 1 >> 1 & 1);

        int v0 = img[xy(x0, y0)], v1 = img[xy(x1, y1)];
        int t = (cut - v0 << 8) / (v1 == v0 ? 1 : v1 - v0);

        _x[n] = (x0 << 8) + t * (x1 - x0);
        _y[n] = (y0 << 8) + t * (y1 - y0);
        _d[n] = v1 - v0;

        x += out==1 ? -1 : out==3 ? 1 : 0;
        y += out==0 ? -1 : out==2 ? 1 : 0;

        m = tmp[xy(x, y)];
        in = (out & 1) > 0 ? 4 - out : 2 - out;
    }
    size[loop] = n;

    for (n = 0 ; n < size[loop] ; n++) {
        int n1 = (n + 1) % size[loop];
        int dx = _x[n1] - _x[n];
        int dy = _y[n1] - _y[n];
        x = _x[n] - 3 * dy >> 8;
        y = _y[n] + 3 * dx >> 8;
        if (x >= 0 && x < w && y >= 0 && y < h)
        _z[n] = img[xy(x, y)];
    }
}

void Contours::find(int *img, int cut) {
    trimEdges(img);
	
    for (int y = 0 ; y < h - 1 ; y++)
    for (int x = 0 ; x < w - 1 ; x++)
        tmp[xy(x, y)] = map[(img[xy(x, y)] > cut ? 1 : 0) | (img[xy(x+1, y)  ] > cut ? 2 : 0) |
                            (img[xy(x, y+1)] > cut ? 4 : 0) | (img[xy(x+1, y+1)] > cut ? 8 : 0) ];

    nLoops = 0;
    for (int i = 0 ; i < w*h ; i++)
        for ( ; nLoops < MAXLOOPS && tmp[i] != 0 ; nLoops++) {
        createLoop(img, tmp, i % w, i / w, cut, nLoops);
        if (! isValidLoop(nLoops))
            nLoops--;
        }
}



  

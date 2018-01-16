#ifndef __CALIBRATION_H_
#define __CALIBRATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <thread>
#include "Bitmap.h"
#include "fflog.h"


#ifndef __x86_64

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif


using namespace cv;
using namespace std;


#define THREADNUM 4

void thread_viwper(void *data);

class  Calibration{
	
public:
	Calibration();
	Calibration(double ang,int scw,int sch,int prew,int preh);
	~Calibration();
	void set(double ang,int scw,int sch,int prew,int preh);
	int  getViewPoint(int num,int LR);
	int processViewPoint();
	void setBitmapID(int id);
	void update();
	int saveProfile(int type);
	void Reset();
	void setBestAngle(double angle){mAngle = angle;}
private:

	int mCurrID;
	FILE *fp;
	FILE *fp_coordinate;
	int mBaseY;
	int mBaseView;
	double mAngle;
	int mScreenWidth;
	int mScreenHeight;
	int mPreviewWidth;
	int mPreviewHeight;
	double mWeight;
	vector< vector<int> >  mLines;
	vector<int> mAllVP;
	std::vector<Bitmap> mBitmap;
	void initBitmap();
	
};

#define PER_ANGLE  5
#define MAX_INTER_BITMAP 32

extern char PICPATH[256];

enum IMG_LR{
	IMG_L = 0,
	IMG_R,
};

struct ThreadData{
    int id;
    vector<Bitmap> Bdata;
    vector<int> bits;
};

void saveProfileFile(double angle,double gwidth,int cwidth,int cheight,uint32_t baseY,uint32_t baseView, uint32_t *data,int num);
#endif

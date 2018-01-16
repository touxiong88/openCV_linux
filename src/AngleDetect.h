#ifndef __ANGLE_DETECT_H_
#define __ANGLE_DETECT_H_

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

using namespace cv;
using namespace std;

void thread_viwper(void *data);

#define THREADNUM 4



class AngleDetect{

public:
	AngleDetect();
	~AngleDetect();
	AngleDetect(int mina,int maxa,int scw,int sch,int prew,int preh);
	void set(int mina,int maxa,int scw,int sch,int prew,int preh);
	int processAngle(int angle);
	void setAngle(int angle);
	void update();
	void Reset();
	double getBestAngle(){return bestAngle;}
private:

	int mCurrID;
	FILE *fp;
	double bestAngle;
	int mMinAngle;
	int mMaxAngle;
	int mScreenWidth;
	int mScreenHeight;
	int mPreviewWidth;
	int mPreviewHeight;
	double mWeight;

	vector<int> mAngles;
	vector<double> mScores;

	std::vector<Bitmap> mBitmap;
	void initBitmap();
};

#endif

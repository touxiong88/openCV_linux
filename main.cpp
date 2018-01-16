#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "./src/Calibration.h"

#include "./src/fflog.h"
#include "./src/AngleDetect.h"

using namespace std;
using namespace cv;
																						

std::time_t getTimeStamp() ;


int main(int argc, char **argv){
        
    LOGFI();
    time_t st = getTimeStamp();
    

    
    //system("rm -rf /home/xwl/pic/pic/* ");
    

    int process_num = 1;
    int mina = 17700;//17780
    int maxa = 17750;//17830
// /*   


    AngleDetect *anglede = new AngleDetect(mina,maxa,3264,2448,800,600);


	
    for(int i = mina; i <= maxa; i += PER_ANGLE){
        anglede->processAngle(i);
        process_num++;
    }

	double calbrationBestAngle = anglede->getBestAngle();
    LOGFI("calbrationBestAngle %f",calbrationBestAngle);
    Calibration *calib = new Calibration(calbrationBestAngle/1000.0,3264,2448,800,600);


	//calib->setBestAngle(calbrationBestAngle);
	
    for(int i = 0; i < MAX_INTER_BITMAP; i++){
        calib->getViewPoint(i,0);
        process_num++;
    }


   calib->processViewPoint();
   calib->saveProfile(0);
  // */  
    LOGFI("took: %ld ms \n",(getTimeStamp() - st)/process_num);

 
    sleep(1);
	
    delete anglede;
    delete calib;

    return 0;
}
// */

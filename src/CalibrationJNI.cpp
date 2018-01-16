#include <jni.h>

#include <fflog.h>

#include "Calibration.h"
#include "AngleDetect.h"


#ifdef __cplusplus
extern "C" {
#endif

static AngleDetect *anglede;
static Calibration *calib;



void JNICALL Java_com_moyan_calibration_Calibration_AngleInit(
    JNIEnv *env, jclass cls,
    int mina,int maxa,
    int screenw,int screenh,
    int prew,int preh){
    LOGFI();
    if(anglede != NULL)
        delete anglede;
    anglede = new AngleDetect(mina,maxa,screenw,screenh,prew,preh);
}


int JNICALL Java_com_moyan_calibration_Calibration_AngleDetect(
    JNIEnv *env, jclass cls,int angle){
     LOGFI();
    anglede->processAngle(angle);
}

void JNICALL Java_com_moyan_calibration_Calibration_SetAngle(
    JNIEnv *env, jclass cls,int angle){
     LOGFI();
    anglede->setAngle(angle);

}

void JNICALL Java_com_moyan_calibration_Calibration_AngleUpdate(
    JNIEnv *env, jclass cls){
    
    anglede->update();

}


void JNICALL Java_com_moyan_calibration_Calibration_AngleRelease(
    JNIEnv *env, jclass cls){
    LOGFI();
    if(anglede != NULL)
        delete anglede;
    anglede = NULL;
}

void JNICALL Java_com_moyan_calibration_Calibration_AngleReset(
    JNIEnv *env, jclass cls){
    anglede->Reset();
}




void JNICALL Java_com_moyan_calibration_Calibration_CalibInit(
    JNIEnv *env, jclass cls,
    double angle,
    int screenw,int screenh,
    int prew,int preh){

    if(calib != NULL)
        delete calib;
	
	
	double calbrationBestAngle = anglede->getBestAngle();
    calib = new Calibration(calbrationBestAngle/1000.0,screenw,screenh,prew,preh);


}


int JNICALL Java_com_moyan_calibration_Calibration_CalibDetect(
    JNIEnv *env, jclass cls,int id,int lor){
    
    return calib->getViewPoint(id,lor);
}

int JNICALL Java_com_moyan_calibration_Calibration_SaveProfile(
    JNIEnv *env, jclass cls,int save_type){
    
    calib->processViewPoint();
    calib->saveProfile(save_type);
}

void JNICALL Java_com_moyan_calibration_Calibration_CalibSetID(
    JNIEnv *env, jclass cls,int id){
    
    calib->setBitmapID(id);
}

void JNICALL Java_com_moyan_calibration_Calibration_CalibRelease(
    JNIEnv *env, jclass cls){
    LOGFI();
    if(calib != NULL)
        delete calib;
    calib = NULL;
    
}

void JNICALL Java_com_moyan_calibration_Calibration_CalibReset(
    JNIEnv *env, jclass cls){
    
    calib->Reset();
}

void JNICALL Java_com_moyan_calibration_Calibration_CalibUpdate(
    JNIEnv *env, jclass cls){
    
    calib->update();

}

#ifdef __cplusplus
}
#endif


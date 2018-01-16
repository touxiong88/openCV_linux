#include <fflog.h>
#include "Calibration.h"
#include "Bitmap.h"

void printData(double angle,double delt,uint8_t *data);
void calImage(int tid,Bitmap  &bitmap);


void thread_viwper(void *data){

    ThreadData  *tdata = (ThreadData*)data;

    vector<int> bits = tdata->bits;
    vector<Bitmap> Bdata = tdata->Bdata;
    
    for(int i = 0; i < bits.size(); i++){
        int idx = bits[i];
        calImage(tdata->id,Bdata[idx]);
    }
	LOGFI("thread %d quit",tdata->id);
    free(data);
}


void calImage(int tid,Bitmap  &bitmap){

    double angle = bitmap.angle;
    double delt_s = bitmap.deltS;
    uint8_t *data = bitmap.data;
    int full = bitmap.full;
    int width = bitmap.width;
    int height = bitmap.height;
    double inc_x = 2.857 / cos(angle *M_PI / 180);
    double int_x =  tan(angle * M_PI / 180);
    double per_inc_x = inc_x/320.0;
    LOGFI("[%d]angle:%3.3f,inc_x %3.6f,per_inc_x %3.3f,delt_s %3.3f,%d %d %d",tid,angle,inc_x,per_inc_x,delt_s,width,height,full);
  //  LOGFI("full %d",full);
    double pre_s[4] = {30,130,190,290};
	double gs1 = (double)pre_s[0]*per_inc_x;
	double gs2 = (double)pre_s[1]*per_inc_x;
	double gs3 = (double)pre_s[2]*per_inc_x;
	double gs4 = (double)pre_s[3]*per_inc_x;

//    LOGFI("gs: %3.3f %3.3f %3.3f %3.3f",gs1,gs2,gs3,gs4);
    double y_pos,delt,dis_test;
    int pos1,pos;

    double tmp;
    for (int i=0;i<height;i++){
        y_pos = i*int_x+delt_s;
        delt = fmod(y_pos,inc_x);
        pos1 = i*width;
        for(int j=0;j<width;j++){
            if(j > delt){
                dis_test = fmod((inc_x+j - delt),inc_x);
            }else{
                dis_test = delt - j;
            }
            pos = pos1 + (width - 1 - j);

            if(full == 0){
                if(dis_test < 1.0){
                    tmp = 1.0;
                }else{
                    tmp = 0.0;
                }
            }else{
                if(dis_test <= gs1){
                    tmp = (1-(dis_test+inc_x-gs4)/(inc_x+gs1-gs4));
                }else if(dis_test <= gs2){
                    tmp = 0.0;
                }else if(dis_test <= gs3){
                    tmp = (dis_test - gs2)/(gs3-gs2);
                }else if(dis_test <= gs4){
                    tmp = 1.0;
                }else{
                    tmp = (1-(dis_test-gs4)/(inc_x+gs1-gs4));
                }
            }
            data[pos] = tmp *255;
        }
    }


    printData(angle,delt_s,data);
}


void printData(double angle,double delt,uint8_t *data){

    char *str = (char*)malloc(1024);
    uint8_t *tmpbuffer = data;

    memset(str,0,1024);
    char buf[4] = {'\0'};
    for(int j=0;j<40;j++){
        sprintf(buf,"%2x ",tmpbuffer[j]);
        strcat(str,buf);
    }  
    LOGFI("buffer[%3.3f %3.3f]:%s",angle,delt,str);

    free(str);


}




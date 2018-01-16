#include "Calibration.h"
#include "AngleDetect.h"

#include <thread>
AngleDetect::AngleDetect(){

    set(17730,17830,1920,1080,800,600);
}

AngleDetect::AngleDetect(int mina,int maxa,int scw,int sch,int prew,int preh){ //java call this
   

    set(mina,maxa,scw,sch,prew,preh);
}

AngleDetect::~AngleDetect(){
    LOGFI();

	return ;
    int num = (mMaxAngle - mMinAngle)/PER_ANGLE+1;
    for(int i = 0; i < num; i++){
        free(mBitmap[i].data);
    }

    vector<Bitmap> tmp;
    mBitmap.swap(tmp);
    LOGFI();
}

void AngleDetect::set(int mina,int maxa,int scw,int sch,int prew,int preh){
    mMinAngle = mina;
    mMaxAngle = maxa;
    mScreenWidth = scw;
    mScreenHeight = sch;
    mPreviewWidth = prew;
    mPreviewHeight = preh;

#ifdef __x86_64
	
		fp = fopen("res.txt","w+");
#else
		fp = fopen("/sdcard/jiaozhun/res.txt","w+");
		
#endif

	
    LOGFI("mMinAngle: %d mMaxAngle %d mScreenWidth %d mScreenHeight%d mPreviewWidth%d mPreviewHeight%d",mMinAngle,mMaxAngle,mScreenWidth,mScreenHeight,mPreviewWidth,mPreviewHeight);

    initBitmap();
}

void AngleDetect::Reset(){
    mAngles.clear();
    mScores.clear();
    mCurrID = 0;
}



void AngleDetect::initBitmap(){
    LOGFI("%d %d",mMinAngle,mMaxAngle);
    
    ThreadData *adataA = new ThreadData();
    ThreadData *adataB = new ThreadData();

    int num = (mMaxAngle - mMinAngle)/PER_ANGLE+1;
    for(int i = 0; i < num; i++){
        Bitmap bitmap;
        mBitmap.push_back(bitmap);
        mBitmap[i].angle = (double)(mMinAngle + i * PER_ANGLE)/1000.0;
        mBitmap[i].deltS = 0;
        mBitmap[i].data = (uint8_t*)malloc(mScreenWidth*mScreenHeight);
        mBitmap[i].full = 0; 
        mBitmap[i].width = mScreenWidth;
        mBitmap[i].height = mScreenHeight;
    }


    LOGFI();


    for(int i = 0; i < num; i++){
        if(i%2 == 0)
            adataA->bits.push_back(i);  
        else
            adataB->bits.push_back(i); 
    }

    LOGFI();
    adataA->Bdata = mBitmap;
    adataA->id = 1;
    adataB->Bdata = mBitmap;
    adataB->id = 2;

    std::thread *m_thread1 = new  std::thread(&thread_viwper,adataA);
    std::thread *m_thread2 = new  std::thread(&thread_viwper,adataB);
    
    m_thread1->join();
    m_thread2->join();



}


int AngleDetect::processAngle(int angle){
      LOGFI("angle: %3.3f",(double)angle/1000.0);
      
      char path[256] = "\0";
      sprintf(path,"%s/calib-%d.jpg",PICPATH,angle);
      Mat img = imread(path,0);
    
      Size dsize = Size(mPreviewWidth,mPreviewHeight);
    
      Mat img2,img3;
      img2 = img;
      resize(img, img2,dsize);

      
      Scalar ss = cv::mean(img2);//Calculates an average (mean) of array elements.
      LOGFI("sum: %f,%f",ss.val[0],ss.val[1]);
    
    
      if(angle == mMinAngle){
    
          mWeight = ss.val[0];
      }
      
   //   img3 = img2 & img2 > mWeight;
    
      
      int height =  img2.rows;
      int width  =  img2.cols;
    
      Mat dstimg;
      threshold( img2, dstimg, mWeight, 255,CV_THRESH_BINARY);
    
      vector<vector<Point> > contoursB;  
      findContours(dstimg, contoursB, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
      int zoomx = 1;
      int zoomy = 1;
      
    
      double max_width = -1;
      for(size_t i = 0; i < contoursB.size(); i++)
      {   
          size_t count = contoursB[i].size();
          
          if( count < 6 )
              continue;
          Mat pointsf;
          Mat(contoursB[i]).convertTo(pointsf, CV_32F);
          RotatedRect box = fitEllipse(pointsf);
          
    
          if(box.size.width < 100)
              continue;
          if(box.size.width > 2000)
              continue;
    
          if((int)box.center.y > 0 && (int)box.center.y < height && (int)box.center.x > 0 && (int)box.center.x < width && (width - (int)box.center.x)>0)    {
              int centerx=(width -  (int)box.center.x);
              int centery= (int)box.center.y;
    
              if(max_width < box.size.width){
                  max_width = box.size.width;
              }
              
          }
    
      }

      if(max_width  < 0) max_width  = 1000;
    
      LOGFI("%d  score:%f",angle,max_width);
    
      mAngles.push_back(angle);
      mScores.push_back(max_width);
    
    //  imshow("test",img2);
    //  imshow("img3",img3);
    //  waitKey(0);
    
      if(angle == mMaxAngle){
    
          double minScore = 999.0;
          bestAngle = 17.78;
          for(int i = 0;i < mAngles.size(); i++){  
              LOGFI("%d %f",mAngles[i],mScores[i]); 
              if(minScore > mScores[i]){
                  minScore = mScores[i];
                  bestAngle = mAngles[i];
              }
          }
          LOGFI("bestAngle: %f",bestAngle);

          return (int)bestAngle;
      }
      LOGFI("QUIT");
    
      return 0;
}

void AngleDetect::setAngle(int angle){

    mCurrID = (angle - mMinAngle)/PER_ANGLE;

    LOGFI("angle %d,mCurrID %d",angle,mCurrID);
}

void AngleDetect::update(){
    
#ifndef __x86_64

    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE,
        mScreenWidth, mScreenHeight,
        0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
        mBitmap[mCurrID].data);
#endif  

}



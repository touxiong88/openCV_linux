#include "Calibration.h"


bool sort_f(int a,int b){
    return a < b;
}

Calibration::Calibration(){

    for(int i = 0; i < MAX_INTER_BITMAP; i++){

       vector<int> line;
       line.push_back(i);
       mLines.push_back(line);
    
    }
    set(17.78,1920,1080,800,600);
}

Calibration::Calibration(double ang,int scw,int sch,int prew,int preh){
   

    for(int i = 0; i < MAX_INTER_BITMAP; i++){

       vector<int> line;
       line.push_back(i);
       mLines.push_back(line);
    
    }
    set(ang,scw,sch,prew,preh);
}

Calibration::~Calibration(){

    LOGFI();

    for(int i = 0; i < MAX_INTER_BITMAP; i++){
        free(mBitmap[i].data);
    }

    vector<Bitmap> tmp;
    mBitmap.swap(tmp);

}


void Calibration::set(double ang,int scw,int sch,int prew,int preh){
    mBaseY = 300;
    mAngle = ang;
    mScreenWidth = scw;
    mScreenHeight = sch;
    mPreviewWidth = prew;
    mPreviewHeight = preh;

#ifdef __x86_64

    fp = fopen("res.txt","w+");
#else
    fp = fopen("/sdcard/jiaozhun/res.txt","w+");
    
#endif
    initBitmap();
}

void Calibration::initBitmap(){

//mBitmap;
    LOGFI("mPreviewSize %d %d",mPreviewWidth,mPreviewHeight);
     
    double inc_x = 3 / cos(mAngle *M_PI / 180);
    double per_delt = inc_x/MAX_INTER_BITMAP;

    LOGFI("mAngle %3.3f,per_delt %3.3f",mAngle,per_delt);

    for(int i = 0; i < MAX_INTER_BITMAP; i++){
        Bitmap bitmap;
        mBitmap.push_back(bitmap);
        mBitmap[i].angle = mAngle;
        mBitmap[i].deltS = per_delt * i;
        mBitmap[i].data = (uint8_t*)malloc(mScreenWidth*mScreenHeight);
        mBitmap[i].full = 1;
        mBitmap[i].width = mScreenWidth;
        mBitmap[i].height = mScreenHeight;
    }

    std::thread *m_thread[THREADNUM];
    int per_thread = MAX_INTER_BITMAP/THREADNUM;
    for(int i = 0; i < THREADNUM; i++){
        ThreadData *adataA = new ThreadData();
        adataA->id = i;
        adataA->Bdata = mBitmap;
        for(int j = 0; j < per_thread; j++){
            adataA->bits.push_back(per_thread*i+j);
        }
        m_thread[i] = new  std::thread(&thread_viwper,adataA);
    }


    for(int i = 0; i < THREADNUM; i++)
        m_thread[i]->join();
    
}

int  Calibration::getViewPoint(int num,int LR){

    LOGFI("%d %d",num,LR);

    char LOR = 'L';
    if(LR == IMG_R) LOR = 'R';

    char path[256] = "\0";
    sprintf(path,"%s/calib-%d%c.jpg",PICPATH,num,LOR);
    LOGFI("%s",path);
    Mat img = imread(path,0);//载入jpg图像并转为单通道
    if(img.empty()){
        LOGFI("cannot open  %s",path);
        return -1;
    }

    Size dsize = Size(mPreviewWidth,mPreviewHeight);
	
    Mat img2,img3;
    resize(img, img2,dsize,CV_INTER_LINEAR);//将img缩放到dsize存储到img2

    int previewHeight =  img2.rows;
    int previewWidth  =  img2.cols;
    int zoomx = 1;
    int zoomy = 1;
    
    Scalar ss = cv::mean(img2);//Calculates an average (mean) of array elements.
    LOGFI("sum: %f,%f",ss.val[0],ss.val[1]);


    if(num == 0 ){
         
        mWeight = ss.val[0]*2.2;
        
    }
    LOGFI("weight %f",mWeight);
    Mat dstimg;
    threshold( img2, dstimg, mWeight ,255,CV_THRESH_BINARY);//Applies an adaptive threshold to an array.

    sprintf(path,"%s/calib-%d%c_t1.jpg",PICPATH,num,LOR);
    imwrite(path,img2);//Saves an image to a specified file.

    sprintf(path,"%s/calib-%d%c_t2.jpg",PICPATH,num,LOR);
    imwrite(path,dstimg);

    vector<vector<Point> > contoursB;  
    findContours(dstimg, contoursB, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);//Finds contours in a binary image.

	sprintf(path,"%s/calib-%d%c_t3.jpg",PICPATH,num,LOR);
    imwrite(path,dstimg);
	
	//contoursB:Detected contours. Each contour is stored as a vector of points.
    Point center[contoursB.size()];
    Mat cimage = Mat::zeros(dstimg.size(), CV_8UC3);//format 

	sprintf(path,"%s/calib-%d%c_t4.jpg",PICPATH,num,LOR);
    imwrite(path,dstimg);

	LOGFI("pic:%d-%d contoursB.size %d",num,LR,(int)contoursB.size());
	for(size_t i = 0; i < contoursB.size(); i++)
	{	
//	     LOGFI();
		size_t count = contoursB[i].size();
		
		if( count < 6 )
			continue;
		Mat pointsf;
		Mat(contoursB[i]).convertTo(pointsf, CV_32F);//Converts an array to another data type with optional scaling.
		RotatedRect rotateRect = fitEllipse(pointsf);//Fits an ellipse around a set of 2D points.

	
		if( MAX(rotateRect.size.width, rotateRect.size.height) > MIN(rotateRect.size.width, rotateRect.size.height)*30 )
	        		continue;
		
		drawContours(cimage, contoursB, (int)i, Scalar::all(255), 1, 8);//Draws contours outlines or filled contours
	    sprintf(path,"%s/calib-%d%c%d_t6.jpg",PICPATH,num,LOR,(int)i);
        imwrite(path,cimage);

    	ellipse(cimage, rotateRect, Scalar(0,0,255), 1);//Draws a simple or thick elliptic arc or fills an ellipse sector.
	    sprintf(path,"%s/calib-%d%c%d_t7.jpg",PICPATH,num,LOR,(int)i);
        imwrite(path,cimage);    	
    	ellipse(cimage, rotateRect.center, rotateRect.size*0.5f, rotateRect.angle, 0, 360, Scalar(0,255,255), 1);
	    sprintf(path,"%s/calib-%d%c%d_t8.jpg",PICPATH,num,LOR,(int)i);
        imwrite(path,cimage);

		center[i].x= rotateRect.center.x;
		center[i].y= rotateRect.center.y;
		circle( cimage, rotateRect.center ,3 , CV_RGB(0,255,0),1, 8, 3 );//Draws a circle.
	    sprintf(path,"%s/calib-%d%c%d_t9.jpg",PICPATH,num,LOR,(int)i);
        imwrite(path,cimage);

		if( MAX(rotateRect.size.width, rotateRect.size.height) > MIN(rotateRect.size.width, rotateRect.size.height)*30 )
			continue;
        
		if(rotateRect.size.width < 15)
			continue;
		if(rotateRect.size.width > 2*240)
			continue;

		if((int)rotateRect.center.y > 0 && (int)rotateRect.center.y < previewHeight && (int)rotateRect.center.x > 0 && (int)rotateRect.center.x < previewWidth && (previewWidth - (int)rotateRect.center.x)>0)	{
			int centerx=(previewWidth -  (int)rotateRect.center.x)/zoomx;
			int centery= (int)rotateRect.center.y/zoomy;
			cout << "x:  "  <<centerx<<"  y:"<<centery<<endl;
			int resx =  centerx-(centery - mBaseY)*0.32;
                    if(fp) fprintf(fp,"%d ",resx);
                    mLines[num].push_back(resx);
            
		}
			
            Point2f vtx[4];
            rotateRect.points(vtx);		
            for( int j = 0; j < 4; j++ )
            	line(cimage, vtx[j], vtx[(j+1)%4], Scalar(0,255,0), 1);	 
		sprintf(path,"%s/calib-%d%c%d_t10.jpg",PICPATH,num,LOR,(int)i);
        imwrite(path,cimage);
	}

     if(fp){
        fprintf(fp,"\n");
        fflush(fp);
     }
    


}


int  Calibration::processViewPoint(){

    LOGFI("mLines.size()  %d",(int) mLines.size());
    int min_value =1000;
    int total = 0;

    vector<int> allVP;//all view piont
    for(int i = 0; i < mLines.size();i++){
        if(mLines[i][0] != i  || mLines[i].size() == 1){
            LOGFI("lines invaild!! %d",i);
            continue;
        }
		for(int j = 1; j < mLines[i].size();j++){
			LOGFI("lines[%d].datax[%d] %d, min_value %d",i,j,mLines[i][j] , min_value);
                    allVP.push_back(mLines[i][j]);
			if(mLines[i][j] < min_value){
				mBaseView = i;
				min_value = mLines[i][j];
			}
		}
	}
    LOGFI("allVP.size() %d ", (int)allVP.size());
    if(allVP.size() < MAX_INTER_BITMAP*1.5)  return -1;
    
    mBaseView = (mBaseView+8)%MAX_INTER_BITMAP;
    
    std::sort(allVP.begin(),allVP.end(),sort_f);
    
    for(int i = 0; i < allVP.size();i++)
        LOGFI("[%2d]: %3d",i,allVP[i]);
  
    double xielv = double(allVP[allVP.size() -1] - allVP[0])/(allVP.size()-1);
    LOGFI("mBaseView %d,xielv %3.3f",mBaseView,xielv);

    vector<int> addVP;
    mBaseView = mBaseView + MAX_INTER_BITMAP*2;
    for(double i = allVP[0]- xielv; i > 100; i -= xielv){
        LOGFI("add: %3.3f",i);
        addVP.push_back(i+0.5);
        mBaseView--;
    }

    for(int i = addVP.size() -1 ; i >= 0; i--){
        mAllVP.push_back(addVP[i]);
    }

    for(int i = 0 ; i < allVP.size(); i++){
        mAllVP.push_back(allVP[i]);
    }

    for(double i = mAllVP[mAllVP.size() -1] + xielv ; i < 800; i += xielv){
        mAllVP.push_back(i+0.5);
    }


    for(int i = 1; i < mAllVP.size();i++)
        LOGFI("[%2d]: %3d %3d",i,mAllVP[i],mAllVP[i] - mAllVP[i-1]);
    
    mBaseView = mBaseView%MAX_INTER_BITMAP;
    LOGFI("mBaseView %d,xielv %3.3f",mBaseView,xielv);


}

int Calibration::saveProfile(int type){

    LOGFI();

    int num  = mAllVP.size();
    uint32_t *data = (uint32_t*)malloc(num*sizeof(uint32_t));
    for(int i = 0; i < mAllVP.size(); i++){

        data[i] = mAllVP[num-1-i];
    }

     if(fp)  fclose(fp);
	//mBaseView += 16;//for YunJiaSu yuntui phone
    saveProfileFile(mAngle,2.857,mPreviewWidth,mPreviewHeight,mBaseY,mBaseView,data,num);
    return 0;
}


void Calibration::setBitmapID(int id){

    mCurrID = id;

    LOGFI("mCurrID %d",mCurrID);
}

void Calibration::update(){
    
#ifndef __x86_64

    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE,
        mScreenWidth, mScreenHeight,
        0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
        mBitmap[mCurrID].data);


#endif  

}

void Calibration::Reset(){

    vector< vector<int> >  v;
    mLines.swap(v);
}



void saveProfileFile(double angle,double gwidth,int cwidth,int cheight,uint32_t baseY,uint32_t baseView, uint32_t *data,int num){
	LOGFI("Save Profile ***************************************");
	LOGFI("angle:			%f",angle);
	LOGFI("baseY:			%u",baseY);
	LOGFI("baseView:			%u",baseView);
	LOGFI("number of Calibration:	%d",num);

#ifdef __x86_64
    char file[256] = "HolographyProfile.txt";
#else
    char file[256] = "/sdcard/HolographyProfile.txt";
#endif
	FILE *fp = fopen(file,"w+");

	int opened = 1;
	if(!fp ){
		LOGFI("open1 %s err %d\n",file,errno);
	}
	fprintf(fp,"HOLO\n");
	fprintf(fp,"angle %f\n",angle);
	fprintf(fp,"bias %d\n",1);
	fprintf(fp,"gwidth %f\n",gwidth);
	fprintf(fp,"cwidth %d\n",cwidth);
	fprintf(fp,"cheight %d\n",cheight);
	fprintf(fp,"baseY %d\n",baseY);
	fprintf(fp,"baseView %d\n",baseView);
	fprintf(fp,"data %d\n",num);

	for(int i = 0; i < num; i++){
		fprintf(fp,"%d\n",data[i]);
	}
	fprintf(fp,"end\n");
	

	fclose(fp);

	LOGFI("Save Profile ***************************************");
}


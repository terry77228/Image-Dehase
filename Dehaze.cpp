#include <iostream>
#include <cstdio>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "Dehaze.h"
using namespace std;

#define patchsize 3
#define t0 0.1
#define w 0.95




bool Dehazer::LoadImage(const std::string& _filename){

}

bool Dehazer::Dehaze(const int& _patchsize, const double& _t, const double& _w){

}

bool Dehazer::WriteImage(const std::string& _filename){

}






char filename[100]; 
CvSize size;
IplImage *InputImg;
IplImage *channel1, *channel2, *channel3, *imgDarkChannel, *TransMap;
CvMat *m_channel1, *m_channel2, *m_channel3;
double atmos_light;


IplImage *LoadFile();
void ShowImg(char *, IplImage *);
IplImage *DarkChannelImage_Create(IplImage *, IplImage *, IplImage *);
double Atmospheric_Light_Estimate(IplImage *,IplImage *,IplImage *,IplImage*);
IplImage *TransMap_Create(IplImage *, IplImage *, IplImage *);


int dehaze(char * name)
{
	strcpy(filename,name);
	IplImage *image;
	image  = LoadFile();
	imgDarkChannel = DarkChannelImage_Create(channel1,channel2,channel3);
	atmos_light = Atmospheric_Light_Estimate(imgDarkChannel,channel1,channel2,channel3);
	TransMap = TransMap_Create(channel1,channel2,channel3);
	//system("pause");
	return 0;
}

IplImage *LoadFile()
{
	double temp1,temp2,temp3;
	InputImg  = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
	size = cvGetSize(InputImg);

	channel1 = cvCreateImage(size,IPL_DEPTH_8U,1);  
	channel2 = cvCreateImage(size,IPL_DEPTH_8U,1);
	channel3 = cvCreateImage(size,IPL_DEPTH_8U,1);
	cvSplit(InputImg,channel1,channel2,channel3,0);

	//將iplimage資訊轉為cvMat
	m_channel1 = cvCreateMat(size.height,size.width,CV_64FC1);				//double 型態單一通道
	m_channel2 = cvCreateMat(size.height,size.width,CV_64FC1);
	m_channel3 = cvCreateMat(size.height,size.width,CV_64FC1);

	for(int i=0;i<channel1->height;i++)
	{
		for(int j=0;j<channel1->width;j++)
		{
			temp1 = (double)(long)((unsigned char*)channel1->imageData)[i*channel1->widthStep+j];
			temp2 = (double)(long)((unsigned char*)channel2->imageData)[i*channel2->widthStep+j];
			temp3 = (double)(long)((unsigned char*)channel3->imageData)[i*channel3->widthStep+j];
			cvmSet(m_channel1,i,j,temp1);
			cvmSet(m_channel2,i,j,temp2);
			cvmSet(m_channel3,i,j,temp3);	
		}
	}
	cout<<"Image is load."<<endl;
	return InputImg;
}
void ShowImg(char *windowname, IplImage *image)
{
	cvNamedWindow(windowname,1);
	cvShowImage(windowname, image);
	cvWaitKey(0);
	cvReleaseImage(&image);
	cvDestroyWindow(windowname);

}

IplImage *DarkChannelImage_Create(IplImage *channel1, IplImage *channel2, IplImage *channel3)
{
	char tempMIN, finalMIN;
	char sname[100];
	IplImage *image = cvCreateImage(size,IPL_DEPTH_8U,1);

	for(int i =0;i<image->height;i++)
	{
		for(int j=0;j<image->widthStep;j++)
		{
			tempMIN = min(channel1->imageData[i*channel1->widthStep+j],channel2->imageData[i*channel2->widthStep+j]);
			finalMIN = min(tempMIN, channel3->imageData[i*channel3->widthStep+j]);
			for(int n=i-patchsize/2;n<=i+patchsize/2;n++)
			{
				for(int m=j-patchsize/2;m<=j+patchsize/2;m++)
				{
					if(n<0||m<0||n>=image->height||m>=image->width)      continue;
					else
					{
						if(channel1->imageData[n*channel1->widthStep+m]<finalMIN)   finalMIN = channel1->imageData[n*channel1->widthStep+m];
						if(channel2->imageData[n*channel2->widthStep+m]<finalMIN)   finalMIN = channel2->imageData[n*channel2->widthStep+m];
						if(channel3->imageData[n*channel3->widthStep+m]<finalMIN)   finalMIN = channel3->imageData[n*channel3->widthStep+m];
					}
				}
			}
			image->imageData[i*image->widthStep+j] = finalMIN;
		}
	}
	strcpy(sname,filename);
	strcat(sname,"DarkChannel.jpg");
	cvSaveImage(sname,image);
	cout<<"The image dark channel is create!"<<endl;
	return image;
}

double Atmospheric_Light_Estimate(IplImage *imgDarkChannel,IplImage *channel1, IplImage *channel2,IplImage *channel3)
{
	CvScalar scalar,scalar_1,scalar_2,scalar_3,scalar_ori_grayLevel;
	double Max, tempMax, finalMax;
	CvPoint vertex1, vertex3,vertex11, vertex33;
	int pos_i,pos_j;

	IplImage *image = cvCloneImage(InputImg);

	IplImage *ori_grayLevel = cvLoadImage(filename,CV_LOAD_IMAGE_GRAYSCALE);
	int Height = cvGetDimSize(imgDarkChannel,0);
	int Width = cvGetDimSize(imgDarkChannel,1);

	//find out the highest pixels in the dark channel
	Max = 0;
	for(int i=0;i<Height;i++)
	{
		for(int j=0;j<Width;j++)
		{
			scalar = cvGet2D(imgDarkChannel,i,j);
			if(scalar.val[0]>Max)
				Max = scalar.val[0];
		}
	}
	cout<<Max<<endl;


	finalMax = 0;
	for(int i=0;i<Height;i++)
	{
		for(int j=0;j<Width;j++)
		{
			scalar = cvGet2D(imgDarkChannel,i,j);
			if(scalar.val[0] == Max)
			{
				scalar_ori_grayLevel = cvGet2D(ori_grayLevel,i,j);
				if(scalar_ori_grayLevel.val[0] > finalMax)	finalMax = scalar.val[0];
				pos_i=i;
				pos_j=j;
			}
		}
	}
	


	vertex1 = cvPoint(pos_i-7,pos_j-7);
	vertex3 = cvPoint(pos_i+7,pos_j+7);
	cvRectangle(image,vertex1,vertex3,CV_RGB(255,0,0),2,0);
	//ShowImg("AtmoLight",image);
	cout<<finalMax<<endl;
	return finalMax;
}

IplImage *TransMap_Create(IplImage *channel1, IplImage *channel2, IplImage *channel3)
{
	IplImage *image = cvCreateImage(size,IPL_DEPTH_8U,1);		//load the transmission map
	IplImage *image_scene = cvCreateImage(size,IPL_DEPTH_8U,3);  //load the recovered scene radiance
	double MIN = 0, tempMIN = 0, trans1 = 0, trans2 = 0, trans = 0;
	CvScalar scalar1,scalar2,scalar3,scalar11,scalar22,scalar33, TransMission, scalar_input, Scene_Radiance;
	char sname[100];
	int Height = cvGetDimSize(image,0);
	int Width = cvGetDimSize(image,1);


	for(int i=0;i<Height;i++)
	{
		for(int j=0;j<Width;j++)
		{
			scalar1 = cvGet2D(channel1,i,j);
			scalar2 = cvGet2D(channel2,i,j);
			scalar3 = cvGet2D(channel3,i,j);
			tempMIN = min(scalar1.val[0],scalar2.val[0]);
			MIN = min(tempMIN,scalar3.val[0]);

			for(int n=i-patchsize/2;n<=i+patchsize/2;n++)
			{
				for(int m=j-patchsize/2;m<=j+patchsize/2;m++)
				{
					if((n<0)||(m<0)||(n>=Height)||(m>=Width))		continue;
					else
					{
						scalar11 = cvGet2D(channel1,n,m);
						scalar22 = cvGet2D(channel2,n,m);
						scalar33 = cvGet2D(channel3,n,m);

						if(scalar11.val[0]<MIN)	MIN = scalar11.val[0];
						if(scalar22.val[0]<MIN)	MIN = scalar22.val[0];
						if(scalar33.val[0]<MIN)	MIN = scalar33.val[0];
					}
				}
			}
			trans = MIN/atmos_light;
			trans1 = (1-w*trans)*255;
			trans2 = 1-w*trans;
			TransMission  = cvScalar(trans1);
			cvSet2D(image,i,j,TransMission);

			scalar_input = cvGet2D(InputImg,i,j);
			if(trans2>t0)
			{
				scalar_input.val[0] = ((scalar_input.val[0]-atmos_light)/trans2)+atmos_light;
				scalar_input.val[1] = ((scalar_input.val[1]-atmos_light)/trans2)+atmos_light;
				scalar_input.val[2] = ((scalar_input.val[2]-atmos_light)/trans2)+atmos_light;
			}
			else
			{
				scalar_input.val[0] = ((scalar_input.val[0]-atmos_light)/t0)+atmos_light;
				scalar_input.val[1] = ((scalar_input.val[1]-atmos_light)/t0)+atmos_light;
				scalar_input.val[2] = ((scalar_input.val[2]-atmos_light)/t0)+atmos_light;
			}
			Scene_Radiance = cvScalar(scalar_input.val[0],scalar_input.val[1],scalar_input.val[2]);
			cvSet2D(image_scene,i,j,Scene_Radiance);
		}
	}



	//IplImage * refinemap = cvLoadImage("refine.jpg",0);
	//IplImage * ori = cvLoadImage("original.jpg",1);
	//IplImage * residue = cvLoadImage(filename,0);
	/*
	for(int i=0;i<Height;i++)
	{
		for(int j=0;j<Width;j++)
		{
			
			trans2 = (double)((unsigned char)refinemap->imageData[i*refinemap->widthStep+j])/255;

			scalar_input = cvGet2D(InputImg,i,j);
			if(trans2>t0)
			{
				scalar_input.val[0] = ((scalar_input.val[0]-atmos_light)/trans2)+atmos_light;
				scalar_input.val[1] = ((scalar_input.val[1]-atmos_light)/trans2)+atmos_light;
				scalar_input.val[2] = ((scalar_input.val[2]-atmos_light)/trans2)+atmos_light;
			}
			else
			{
				scalar_input.val[0] = ((scalar_input.val[0]-atmos_light)/t0)+atmos_light;
				scalar_input.val[1] = ((scalar_input.val[1]-atmos_light)/t0)+atmos_light;
				scalar_input.val[2] = ((scalar_input.val[2]-atmos_light)/t0)+atmos_light;
			}
			Scene_Radiance = cvScalar(scalar_input.val[0],scalar_input.val[1],scalar_input.val[2]);
			cvSet2D(image_scene,i,j,Scene_Radiance);
		}
		//fprintf(stderr,"\n");
	}
	*/
/*
	for(int i=0;i<residue->height;i++)
	{
		for(int j=0;j<residue->width;j++)
		{
			
		residue->imageData[i*residue->widthStep + j] = abs(channel1->imageData[i*channel1->widthStep + j]- ori->imageData[i*ori->widthStep + j]) + abs(channel2->imageData[i*channel2->widthStep + j]- ori->imageData[i*ori->widthStep + j + 1]) + abs(channel3->imageData[i*channel3->widthStep + j]- ori->imageData[i*ori->widthStep + j + 2]);
		}
		//fprintf(stderr,"\n");
	}
*/
	//cvSaveImage("ch1.jpg",channel1);
	//cvSaveImage("ch2.jpg",channel2);
	//cvSaveImage("ch3.jpg",channel3);

	//cvSaveImage("residue.jpg",residue);
	strcpy(sname,filename);
	strcat(sname,"TransMap.jpg");
	cout<<"Transmission Map is created."<<endl;
	cvSaveImage(sname,image);
	cout<<"Achieve image dehazing."<<endl;
	strcpy(sname,filename);
	strcat(sname,"output.jpg");
	cvSaveImage(sname,image_scene);

	return image;
}
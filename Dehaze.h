#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <memory>
using namespace std;
using namespace cv;
class ImageDehazer{

public:
	bool LoadImage(const std::string& _filename);
	bool Dehaze(const int& _patchsize, const double& _t, const double& _w);
	bool WriteImage(const std::string& _filename);

private:
	IplImage * m_InputImage;
	cv::Mat m_Image;
	cv::Mat m_DoubleImage;
	cv::Mat m_DarkChannelImage;
	cv::Mat m_RecoveredImage;
	IplImage *InputImg;
	double m_AtmosLight;

	void DarkChannelImage_Create(const int& _patchsize);
	double Atmospheric_Light_Estimate();
	void TransMap_Create(const int& _patchsize, const double& _t, const double& _w);
};



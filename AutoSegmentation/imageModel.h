#ifndef __20140414AUTOSEGMENTATION__
#define __20140414AUTOSEGMENTATION__

#include <iostream>
#include <iterator>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Labeling.h"

using namespace std;

#ifdef _DEBUG
    //Debugモードの場合
    #pragma comment(lib,"opencv_core248d.lib")
    #pragma comment(lib,"opencv_imgproc248d.lib")
    #pragma comment(lib,"opencv_highgui248d.lib")
#else
    //Releaseモードの場合
    #pragma comment(lib,"opencv_core248.lib")
    #pragma comment(lib,"opencv_imgproc248.lib")
    #pragma comment(lib,"opencv_highgui248.lib")
#endif


enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2, LBUTTON = 3, RBUTTON = 4, BGR = 5, HSV = 6 };//Flag

class IMAGESET
{
public:
	cv::Mat foreImg;
	cv::Mat mask;
	cv::Mat tempMask;

	//For drawing
	cv::Point lstart, rstart;
	cv::Point lend, rend;
	uchar labelsState;

public:
	IMAGESET(const string &foreG);
	~IMAGESET(){};
	void autoSeg();
	void saveImage(const string &file);
	void drawing(const string &windowName,cv::Mat &_foreImg,cv::Mat &_mask);
	void grabCut(const string &windowName,const cv::Rect &rect);
	void grabCut(const string &windowName);
	void removeIsolation();

private:
	IMAGESET(){};
	
};

#endif
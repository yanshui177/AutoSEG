//
//imageModel.cpp
//
//
// Copyright(c) 2014 Shunsuke Saito. All rights reserved.
#include <map>

#include "imageModel.h"
#include "Labeling.h"

#include "Eigen/Core"
#define EIGEnN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include "Eigen/Sparse"


void connectedComponent(cv::Mat &input,vector<int> &lut)
{
	cv::Mat bining;
	cv::threshold(input,bining,0,255,CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	cv::Mat label(input.size(),CV_16SC1);

	LabelingBS labeling;
	labeling.Exec(bining.data,(short*)label.data,input.cols,input.rows,false,0);


}

void resizeMask(const cv::Mat input,cv::Mat &output,cv::Size &size)
{
	output.create(size,input.type());

	const double scale=input.size().height/(double)size.height;

	int oldX,oldY;
	for(int i=0;i<output.rows;++i)
	{
		uchar* ptr=output.ptr<uchar>(i);
		oldY=i*scale;
		for(int j=0;j<output.cols;++j)
		{
			oldX=j*scale;
			ptr[j]=input.at<uchar>(oldY,oldX);
		}
	}
}


IMAGESET::IMAGESET(const string &foreG)
{
	foreImg=cv::imread(foreG);

	mask.create(foreImg.size(),CV_8UC1);
	mask.setTo(cv::GC_PR_BGD);
}


void IMAGESET::saveImage(const string &file)
{
	cv::Mat backTmp=foreImg.clone();
	backTmp.setTo(cv::Vec3b(0,255,255));

	cv::compare(mask,cv::GC_PR_FGD|cv::GC_FGD,mask,cv::CMP_EQ);
	// Generate output image
	foreImg.copyTo(backTmp,mask);

	cv::imwrite(file,backTmp);
}

void IMAGESET::autoSeg()
{
	cv::RNG rng(12345);

	cv::Mat grayImg, thresh;
	vector<vector<cv::Point>> contours;
	vector<cv::Point> cnt;
	vector<cv::Vec4i> hierarchy;
	cv::cvtColor(foreImg,grayImg,cv::COLOR_BGR2GRAY);

	cv::threshold(grayImg,thresh,127,255,0);
	cv::findContours(thresh,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
	
	vector<vector<cv::Point>> contours_poly(contours.size());
	vector<cv::Rect> boundRect(contours.size());
	vector<cv::Point2f> center(contours.size());
	vector<float> radius(contours.size());

	for(int i=0;i<contours.size();++i)
	{
		cv::approxPolyDP(cv::Mat(contours[i]),contours_poly[i],3,true);
		boundRect[i]=cv::boundingRect(cv::Mat(contours[i]));
		cv::minEnclosingCircle((cv::Mat)contours[i],center[i],radius[i]);
	}

	cv::Mat drawing=cv::Mat::zeros(thresh.size(),CV_8UC3);
	for(int i=0;i<contours.size();++i)
	{
		cv::Scalar color=cv::Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
		cv::drawContours(drawing,contours_poly,i,color,1,8,vector<cv::Vec4i>(),0,cv::Point());
		cv::rectangle(drawing,boundRect[i].tl(),boundRect[i].br(),color,2,8,0);
		cv::circle(drawing,center[i],(int)radius[i],color,2,8,0);
	}
	
	cv::namedWindow("contour",CV_WINDOW_AUTOSIZE);
	cv::imshow("contour", drawing);

	cout << contours.size() << " " << contours[0].size() << endl;

	while(1){
		cv::imshow("result",miniMat);
		cv::waitKey(30);
	}
}

void IMAGESET::drawing(const string &windowName,cv::Mat &_foreImg,cv::Mat &_mask)
{
    cv::Mat scribbled_src = _foreImg.clone();
    const float alpha = 0.7f;
	static const cv::Vec3b bg_color(255,0,0);
	static const cv::Vec3b fg_color(0,0,255);

	for(int y=0; y < _foreImg.rows; y++)
	{
		for(int x=0; x < _foreImg.cols; x++)
		{
			cv::Vec3b& pix = scribbled_src.at<cv::Vec3b>(y, x);
			if(_mask.at<uchar>(y,x) == cv::GC_FGD) {
				pix=fg_color;
			}else if(_mask.at<uchar>(y,x) == cv::GC_BGD) {
				pix=bg_color;
			}else if(_mask.at<uchar>(y,x) == cv::GC_PR_BGD){
				pix[0] = (uchar)(pix[0] * alpha + bg_color[0] * (1-alpha));
				pix[1] = (uchar)(pix[1] * alpha + bg_color[1] * (1-alpha));
				pix[2] = (uchar)(pix[2] * alpha + bg_color[2] * (1-alpha));
			}else{
				pix[0] = (uchar)(pix[0] * alpha + fg_color[0] * (1-alpha));
				pix[1] = (uchar)(pix[1] * alpha + fg_color[1] * (1-alpha));
				pix[2] = (uchar)(pix[2] * alpha + fg_color[2] * (1-alpha));

			}
		}
	}	
    cv::imshow(windowName,scribbled_src);
	cv::waitKey(30);
}

void IMAGESET::grabCut(const string &windowName)
{
	cv::Mat bgModel,fgModel;
	cv::Mat foreTmp=foreImg.clone();
	cv::Mat maskTmp=mask.clone();
	cv::resize(foreTmp,foreTmp,cv::Size(),0.5,0.5,cv::INTER_NEAREST);
	cv::resize(maskTmp,maskTmp,cv::Size(),0.5,0.5,cv::INTER_NEAREST);

	cv::grabCut(foreTmp,maskTmp,cv::Rect(),bgModel,fgModel,5,cv::GC_INIT_WITH_MASK);
	resizeMask(maskTmp,mask,mask.size());

	tempMask.create(mask.size(),CV_8UC1);
	for(int i=0;i<mask.rows;++i)
	{
		uchar* mPtr=mask.ptr<uchar>(i);
		uchar* ptr=tempMask.ptr<uchar>(i);
		for(int j=0;j<mask.cols;++j)
		{
			if(mPtr[j]==cv::GC_FGD||mPtr[j]==cv::GC_PR_FGD) ptr[j]=0;
			else ptr[j]=255;
		}
	}
		
	drawing(windowName,foreImg,mask);
}


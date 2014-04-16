#include "imageModel.h"

const string foreFile="kiichi.jpg";

//const string foreFile="fffff.png";
//const string foreFile="test.png";

const string window="Edit";

static void events( int e, int x, int y, int flags, void* s);

IMAGESET imageSet(foreFile);



int main()
{
	cv::Mat maskMat;

	imageSet.autoSeg();
	int key;
	while(1){
		imageSet.drawing(window,imageSet.foreImg,imageSet.mask);
		key=cv::waitKey(5);
	}
}

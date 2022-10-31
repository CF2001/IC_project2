#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc != 3) {
		cerr << "usage: ex1.out <Image_Path> <final Image>" << endl;
		return -1;
	}
	
	char *originalImage = argv[1];
	char *finalImage = argv[2];
	
	Mat imageIn;
	imageIn = imread(originalImage, 1);
	if (!imageIn.data) {
		cerr << "No image data" << endl;
		return -1;
	}
	
	
	//namedWindow("Display Image", WINDOW_AUTOSIZE);
	//imshow("Display Image", image);
	//waitKey(0);
	return 0;
}

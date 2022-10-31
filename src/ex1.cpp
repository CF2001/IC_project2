#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc != 3) {
		cerr << "Usage : " << argv[0] << "<input filename> <output filename>\n";
		return -1;
	}
	
	char *inputFile = argv[1];
	char *outputFile = argv[2];
	
	
	Mat imageIn;
	imageIn = imread(inputFile);	// Read the image file
	if (!imageIn.data) {
		cerr << "No image data" << endl;
		return -1;
	}
	
	// Create output image 
	Mat imageOut(imageIn.rows, imageIn.cols, imageIn.type());
	
	for (int i = 0; i < imageIn.rows; i++)
	{
		for (int j = 0; j < imageIn.cols; j++)
		{
			imageOut.ptr<Vec3b>(i)[j] = Vec3b(imageIn.ptr<Vec3b>(i)[j][0], imageIn.ptr<Vec3b>(i)[j][1], imageIn.ptr<Vec3b>(i)[j][2]);
		}
	}
	
	imwrite(outputFile,imageOut);
	
	namedWindow("Output image", WINDOW_NORMAL);
    	resizeWindow("Output image", 800, 650);
    	imshow("Output image", imageOut);
	waitKey(0);
	
	return 0;
}

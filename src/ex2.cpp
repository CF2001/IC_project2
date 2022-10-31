#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc != 4) {
		cerr << "Usage : " << argv[0] << "<input filename> <output filename> <filter type>\n";
		return -1;
	}
	
	char *inputFile = argv[1];
	char *outputFile = argv[2];
	int filterType = stoi(argv[3]);
	
	
	Mat imageIn;
	imageIn = imread(inputFile);	// Read the image file
	if (!imageIn.data) {
		cerr << "No image data" << endl;
		return -1;
	}
	
	// Create output image 
	Mat imageOut(imageIn.rows, imageIn.cols, imageIn.type());
	
	switch(filterType) {
	
		case 0:	//  Creates the negative version of an image
		
			for (int i = 0; i < imageIn.rows; i++)
			{
				for (int j = 0; j < imageIn.cols; j++)
				{
					imageOut.ptr<Vec3b>(i)[j] = Vec3b(255 -imageIn.ptr<Vec3b>(i)[j][0], 
									255 - imageIn.ptr<Vec3b>(i)[j][1], 
									255 - imageIn.ptr<Vec3b>(i)[j][2]);
				}
			}
	
			imwrite(outputFile,imageOut);
			
			namedWindow("Output image", WINDOW_NORMAL);
		    	resizeWindow("Output image", 800, 650);
		    	imshow("Output image", imageOut);
			waitKey(0);
	
			break;
		
		case 1: 	// Creates a mirrored version of an image horizontally
		
			int line = 10
			for (int i = 0; i < imageIn.rows ; i++)
			{
				for (int j = 0; j < imageIn.cols; j++)
				{
					imageOut.ptr<Vec3b>(i+line)[j]
				}
				
			}
	
			imwrite(outputFile,imageOut);
			
			namedWindow("Output image", WINDOW_NORMAL);
		    	resizeWindow("Output image", 800, 650);
		    	imshow("Output image", imageOut);
			waitKey(0);
		
			break;
			
		case 2: 	// Creates a mirrored version of an image vertically 
		
			break;
	}
	
	return 0;
}

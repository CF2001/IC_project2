#include <opencv2/opencv.hpp>
#include <stdio.h>

#include "FilterType.h"

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
	
	FilterType filterT{ };
	
	switch(filterType) {
	
		case 0:	//  Creates the negative version of an image
			filterT.negativeVersion(imageIn, imageOut, outputFile, "negativeVersion");
			break;
		
		case 1: 	// Creates a mirrored version of an image horizontally	
			
			filterT.imageHorizontally(imageIn, imageOut, outputFile, "imageHorizontal");
			break;
			
		case 2: 	// Creates a mirrored version of an image vertically 
		
			filterT.imageVertically(imageIn, imageOut, outputFile, "imageVertical");
			break;
	}
	
	return 0;
}

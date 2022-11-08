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
	
	int beta = 0;
	
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
		case 3:		// Rotates an image by a multiple of 90º 
			filterT.imageRotate(imageIn, imageOut, outputFile , "imageRotated");
			break;
		case 4:		// Increases the intensity values of an image 
			
			cout << "Insert the intensity value (>0): \n";
			cin >> beta;
			while(beta < 0)
			{
				cerr << "Error: must be > 0\n" ;
				cin >> beta;
			}
			
			filterT.imageMoreLight(imageIn , imageOut , outputFile , "imageMoreLight", beta);
			
			break;
		case 5:		// Decreases the intensity values of an image 
			
			cout << "Insert the intensity value (>0): \n";
			cin >> beta;
			while(beta < 0)
			{
				cerr << "Error: must be > 0\n" ;
				cin >> beta;
			}
			filterT.imageLessLight(imageIn , imageOut, outputFile, "imageLessLight" , beta);
			break;
	}
	
	return 0;
}

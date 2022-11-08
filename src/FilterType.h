#include <opencv2/opencv.hpp>

using namespace cv;

class FilterType {

	
	public: 
		
		/***  Creates the negative version of an image  ***/
		void negativeVersion(Mat imageIn, Mat imageOut, char *outputFile, const char *nameOutputImg)
		{
			for (int i = 0; i < imageIn.rows; i++)
			{
				for (int j = 0; j < imageIn.cols; j++)
				{
					// Vec3b(255-R,255-G,255-B) 
					imageOut.ptr<Vec3b>(i)[j] = Vec3b(255 -imageIn.ptr<Vec3b>(i)[j][0], 
									255 - imageIn.ptr<Vec3b>(i)[j][1], 
									255 - imageIn.ptr<Vec3b>(i)[j][2]);
				}
			}
			
			writeImage(outputFile, imageOut, nameOutputImg); 
		}
		
		/*** Creates a mirrored version of an image horizontally ***/
		void imageHorizontally(Mat imageIn, Mat imageOut, char *outputFile, const char *nameOutputImg)
		{
			for (int row = 0; row < imageIn.rows; ++row)
			{
				for (int col = 0; col < imageIn.cols; ++col)
				{
			           imageOut.ptr<Vec3b>(row)[imageIn.cols-1-col][0] = imageIn.ptr<Vec3b>(row)[col][0];
			           imageOut.ptr<Vec3b>(row)[imageIn.cols-1-col][1] = imageIn.ptr<Vec3b>(row)[col][1];
  				   imageOut.ptr<Vec3b>(row)[imageIn.cols-1-col][2] = imageIn.ptr<Vec3b>(row)[col][2];
				}
				
			}
			writeImage(outputFile, imageOut, nameOutputImg);
		}
		
		/*** Creates a mirrored version of an image vertically ***/
		void imageVertically(Mat imageIn, Mat imageOut, char *outputFile, const char *nameOutputImg)
		{
			for (int row = 0; row < imageIn.rows; ++row)
			{
				for (int col = 0; col < imageIn.cols; ++col)
				{
			           imageOut.ptr<Vec3b>(imageIn.rows-1-row)[col][0] = imageIn.ptr<Vec3b>(row)[col][0];
			           imageOut.ptr<Vec3b>(imageIn.rows-1-row)[col][1] = imageIn.ptr<Vec3b>(row)[col][1];
  				   imageOut.ptr<Vec3b>(imageIn.rows-1-row)[col][2] = imageIn.ptr<Vec3b>(row)[col][2];
			            
				}	
			}
			writeImage(outputFile, imageOut, nameOutputImg);
		}
		
		/*** Rotates an image by a multiple of 90º ***/
		void imageRotate(Mat imageIn, Mat imageOut, char *outputFile, const char *nameOutputImg)
		{
			
		}
		
		/*** Increase the intensity value of an image **/
		void imageMoreLight(Mat imageIn , Mat imageOut , char *outputFile , const char *nameOutputImg , int beta)
		{
		
		
			for( int y = 0; y < imageIn.rows; y++ ) {
				for( int x = 0; x < imageIn.cols; x++ ) {
	    				for( int c = 0; c < imageIn.channels(); c++ ) {   // Channels RGB
                				imageOut.at<Vec3b>(y,x)[c] = saturate_cast<uchar>(imageIn.at<Vec3b>(y,x)[c] + beta );
			    		}
				}
		    	}	
			writeImage(outputFile , imageOut , nameOutputImg);
		
		} 
		
		/*** Decrease the intensity value of an image **/
		void imageLessLight(Mat imageIn , Mat imageOut , char *outputFile , const char *nameOutputImg , int beta)
		{
			
			for( int y = 0; y < imageIn.rows; y++ ) {
				for( int x = 0; x < imageIn.cols; x++ ) {
	    				for( int c = 0; c < imageIn.channels(); c++ ) {   // Channels RGB
                				imageOut.at<Vec3b>(y,x)[c] = saturate_cast<uchar>(imageIn.at<Vec3b>(y,x)[c] - beta );
			    		}
				}
		    	}	
			writeImage(outputFile , imageOut , nameOutputImg);
		
		} 
		
		void writeImage(char *outputFile,  Mat imageOut, const char *nameOutputImg)
		{
			imwrite(outputFile,imageOut);
			
			namedWindow(nameOutputImg, WINDOW_NORMAL);
		    	resizeWindow(nameOutputImg, 800, 650);
		    	imshow(nameOutputImg, imageOut); 
			waitKey(0);
		}
	
};



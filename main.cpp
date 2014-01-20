
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
#include <assert.h>
using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    
	namedWindow( "Display Window", WINDOW_AUTOSIZE ); // Create a window for display.
	namedWindow( "Input", WINDOW_AUTOSIZE);
	namedWindow("intermediate", WINDOW_AUTOSIZE);
    Mat image;

	VideoCapture cap("http://10.16.22.11/mjpg/video.mjpg");
	try{
    while(cap.isOpened())
	{
		Mat in=Mat::zeros(image.rows, image.cols, CV_8UC3);
		cap>>in;
		Mat image;
		Mat image_bw;
		cvtColor(in, image_bw, CV_RGB2GRAY);
		adaptiveThreshold(image_bw, image, 190, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, -10);
		if(image.empty()) break;

		Mat dest = Mat::zeros(image.rows, image.cols, CV_8UC3);
		image=image > 1;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		

		for (int i = 0; i>=0; i=hierarchy[i][0])
		{
			Scalar color(rand()&255, rand()&255, rand()&255);
			drawContours( dest, contours, i, color, CV_FILLED, 8, hierarchy);
		}
		
		imshow("Display window", image); // Show our image inside it.
		imshow("Input", in);
		imshow("intermediate", image_bw);
		if(waitKey(30) >=0) break;
	}
	}
	catch (std::exception& e) {
		cout << e.what();
		return 1;
	}
	
    waitKey(0);
    return 0;
}
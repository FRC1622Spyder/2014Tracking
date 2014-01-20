
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
using namespace cv;
using namespace std;

//#define LIVE_CAP //uncomment for axis camera use

int main( int argc, char** argv )
{

	namedWindow( "Display Window", WINDOW_AUTOSIZE ); // Create a window for display.
	namedWindow( "Input", WINDOW_AUTOSIZE);
	namedWindow("intermediate", WINDOW_AUTOSIZE);
	Mat image;

#ifdef LIVE_CAP
	VideoCapture cap("http://10.16.22.11/mjpg/video.mjpg");
	try{
		while(cap.isOpened())
		{
#else 
	try{
#endif
		Mat in=Mat::zeros(image.rows, image.cols, CV_8UC3);
#ifdef LIVE_CAP
		cap.grab();
		cap.retrieve(in);
#else
		in = imread("cap_sample.jpg");
#endif
		Mat image;
		Mat image_bw;
		cvtColor(in, image_bw, CV_RGB2GRAY);
		imshow("Input", image_bw);
		adaptiveThreshold(image_bw, image, 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, -5);
		imshow("intermediate", image);
		Mat dest =Mat::zeros(in.size(), CV_32FC3);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

		imshow("Display window", image); // Show our image inside it.
		for(int i =0; i<contours.size(); i++)
		{
			while( (contours[i].size() < 4 )&&(contours.size()>i)) { //if not a rectangle or larger...
				contours.erase(contours.begin()+i);
			}
		}
#ifdef LIVE_CAP
		if(waitKey(30) >=0) break;

	}
#endif
}
catch (cv::Exception& e) {
	cout <<"CV_ERROR: " << e.what();
}
catch (std::exception& e) {
	cout << "ERROR: " << e.what();

}


waitKey(0);
return 0;
}
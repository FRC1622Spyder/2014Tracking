
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
using namespace cv;
using namespace std;
#define R_ 2
#define G_ 1
#define B_ 0

#define LIVE_CAP //uncomment for axis camera use

int main( int argc, char** argv )
{
	int counter = 0;
	//namedWindow( "Display Window", WINDOW_AUTOSIZE ); // Create a window for display.
	namedWindow( "contour", WINDOW_AUTOSIZE);
	namedWindow( "Input-b1", WINDOW_AUTOSIZE);
	namedWindow( "Input-b2", WINDOW_AUTOSIZE);
	//namedWindow("intermediate", WINDOW_AUTOSIZE);
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
		cout << "get image" <<endl;
		cap >> in;
#else
		in = imread("cap_sample.jpg");
#endif
		Mat image;
		vector<Mat> images;
		if(in.cols==0)
		{
			cout << "got null image. re-opening stream." << endl;
			counter = 0;
			cap.release();
			cap.open("http://10.16.22.11/mjpg/video.mjpg");
			cap >> in;
			if( in.cols==0) break;
		}
		cout << "split" <<endl;
		split(in, images);
		imshow("Input-b1", images[B_]);
		cout << "thresh" <<endl;
		adaptiveThreshold(images[B_], images[B_], 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -50);
		imshow("Input-b2", images[B_]);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		int thresh = 100;
		int max_thresh = 255;
		Canny( images[B_], images[B_], thresh, thresh*2, 3 );
		/// Find contours
		cout << "contours" <<endl;
		findContours( images[B_], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		vector<vector<Point> > contours_poly( contours.size() );
		vector<Rect> boundRect( contours.size() );
		vector<Point2f>center( contours.size() );

		/// Draw contours
		cout << "prune" <<endl;
		Mat drawing = Mat::zeros( images[B_].size(), CV_8UC3 );
		for(int j =0; j < contours.size(); j++) // trim the extraneous ones
		{
			while(((contours[j].size() < 4 )||(15.0f>contourArea(contours[j])))&&(j < contours.size())) { //if not a rectangle or smaller than n pixels...				
				contours.erase(contours.begin()+j);
				hierarchy.erase(hierarchy.begin()+j);
			}
		}
		cout << "rect" <<endl;
		for( int i = 0; i < contours.size(); i++ )
		{ 
			approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		}
		cout << "draw " << contours.size() <<endl;
		for( int i = 0; i< contours.size(); i++ )
		{
			Scalar color = Scalar( 255, 0,0 );
			drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );
			rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 1, 8, 0 );
		}
		cout << "show" <<endl;
		imshow("contour", drawing);

		contours.clear();
		images.clear();
		hierarchy.clear();
#ifdef LIVE_CAP
		if(waitKey(30) >=0) break;
		counter++;
		cout << counter <<endl;
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
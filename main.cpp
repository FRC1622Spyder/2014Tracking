
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
#include "Vision.h"
using namespace cv;
using namespace std;
#define R_ 2
#define G_ 1
#define B_ 0


// NOTES:
// Keeping the object count down by filtering by size
// 


#define LIVE_CAP //uncomment for axis camera use
void reopen(VideoCapture cap)
{
	cap.release();
	cap.open("http://10.16.22.11/mjpg/video.mjpg");
}
int main( int argc, char** argv )
{
	int counter = 0;
	namedWindow( "contour", WINDOW_AUTOSIZE);
	namedWindow( "Input-b1", WINDOW_AUTOSIZE);
	namedWindow( "Input-b2", WINDOW_AUTOSIZE);
#ifdef LIVE_CAP
	VideoCapture cap("http://10.16.22.11/mjpg/video.mjpg");
	while(cap.isOpened())
	{
#endif
	Mat in;
#ifdef LIVE_CAP
	cap >> in;
	if(in.cols==0)
	{
		cout << "got null image. re-opening stream." << endl;
		reopen(cap);
		cap >> in;
		if( in.cols==0) break;
	}
#else
	in = imread("image.jpg");
	
#endif
	Vision v(in);
	v.doThreshold();
	v.processContours();
	v.filterContours();
	Mat drawing = Mat::zeros( v.images[B_].size(), CV_8UC3 );
	for(unsigned int i = 0; i< v.contours.size(); i++ )
	{
		drawContours( in, v.contours, i, Scalar( 255, 0,0 ), 1, 8,v.hierarchy, 0, Point() );
		rectangle( in, v.boundRect[i].tl(), v.boundRect[i].br(), Scalar(0,255,0), 1, 8, 0 );
		float xavg = 0.0f;
		float yavg = 0.0f;
		xavg=((v.boundRect[i].tl().x) + v.boundRect[i].br().x)/2;
		yavg=((v.boundRect[i].tl().y) + v.boundRect[i].br().y)/2;
		v.centers.push_back(
			KeyPoint(xavg, yavg, 1.0f));
		drawKeypoints(in, v.centers, drawing, Scalar(0,0,255));
	}
	

	
	for(unsigned int i = 0; i<v.centers.size(); i++)
	{
		v.rCenters.push_back(Point2f(
			((v.centers[i].pt.x-v.centerX)*(2/(float)in.cols)), 
			((v.centers[i].pt.y-v.centerY)*(2/(float)in.rows))));
	}
//#ifdef _DEBUG //debugging output
	cout<<"rows: " << in.rows << " cols: " << in.cols<<endl;
	cout<<"[ID] [CenterX,CenterY] [Apparent Size (px.)] [relative position to center]"<<endl;
	for(unsigned int i = 0; i< v.centers.size(); i++)
	{
		cout << "[" << i << "] " << v.centers[i].pt <<" ["<<v.boundRect[i].area()<<"] "<<v.rCenters[i]<<endl;
	}
//#endif
	imshow("contour", drawing);
	v.~Vision();

#ifdef LIVE_CAP
	if(waitKey(30) >=0) break;
	counter++;
	cout << counter <<endl;
}
#endif


/*
catch (cv::Exception& e) {
cout <<"CV_ERROR: " << e.what();
}
catch (std::exception& e) {
cout << "ERROR: " << e.what();

}
*/

waitKey(0);
return 0;
}
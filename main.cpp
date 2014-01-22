
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
using namespace cv;
using namespace std;
#define R_ 2
#define G_ 1
#define B_ 0


// NOTES:
// Keeping the object count down by filtering by size
// 


//#define LIVE_CAP //uncomment for axis camera use
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
	vector<Mat> images;
	split(in, images);
	imshow("Input-b1", images[B_]);

	//still debating about wether to use normal or adaptive thresholds.
	adaptiveThreshold(images[B_], images[B_], 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, -10);
	//threshold(images[B_], images[B_], 230, 255, THRESH_BINARY);

	imshow("Input-b2", images[B_]);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<KeyPoint> centers;
	vector<Point2f> rCenters;
	const int centerX = in.cols/2;
	const int centerY = in.rows/2;
	int thresh = 100;
	Canny( images[B_], images[B_], thresh, thresh*2, 3 ); //edge detection

	// Find contours
	findContours( images[B_], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );

	// Draw contours
	
	for(unsigned int j =0; j < contours.size(); j++) // trim the extraneous contours
	{
		while((contours[j].size() <= 4 )&&(j < contours.size())) { //if chain contains less than 4 points				
			contours.erase(contours.begin()+j);
			hierarchy.erase(hierarchy.begin()+j);
		}
	}

	for(unsigned int i = 0; i < contours.size(); i++ )
	{ 
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	}

	for(unsigned int j =0; j < contours.size(); j++) // trim the extraneous bounding boxes
	{
		while(((5000.0f<boundRect[j].area())||(2000.0f>boundRect[j].area()))&&(j < contours.size())) { //a sort of band pass filter for rectangle area.			
			contours.erase(contours.begin()+j);
			hierarchy.erase(hierarchy.begin()+j);
			boundRect.erase(boundRect.begin()+j);
		}
	}
	Mat drawing = Mat::zeros( images[B_].size(), CV_8UC3 );
	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		drawContours( in, contours, i, Scalar( 255, 0,0 ), 1, 8, hierarchy, 0, Point() );
		rectangle( in, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 1, 8, 0 );
		centers.push_back(
			KeyPoint(
			((float)(boundRect[i].tl().x + boundRect[i].br().x) / 2),
			((float)(boundRect[i].tl().y + boundRect[i].br().y) / 2),
			1.0f)
			);
		drawKeypoints(in, centers, drawing, Scalar(0,0,255));
	}
	

	
	for(unsigned int i = 0; i<centers.size(); i++)
	{
		rCenters.push_back(Point2f(
			((centers[i].pt.x-centerX)*(2/(float)in.cols)), 
			((centers[i].pt.y-centerY)*(2/(float)in.rows))));
	}
	cout<<"rows: " << in.rows << " cols: " << in.cols<<endl;
	cout<<"[ID] [CenterX,CenterY] [Apparent Size (px.)] [relative position to center]"<<endl;
	for(unsigned int i = 0; i< centers.size(); i++)
	{
		cout << "[" << i << "] " << centers[i].pt <<" ["<<boundRect[i].area()<<"] "<<rCenters[i]<<endl;
	}

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
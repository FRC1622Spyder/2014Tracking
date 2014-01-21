
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

int main( int argc, char** argv )
{
	int counter = 0;
	namedWindow( "contour", WINDOW_AUTOSIZE);
	namedWindow( "Input-b1", WINDOW_AUTOSIZE);
	namedWindow( "Input-b2", WINDOW_AUTOSIZE);
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
		if(in.cols==0)
		{
			cout << "got null image. re-opening stream." << endl;
			counter = 0;
			cap.release();
			cap.open("http://10.16.22.11/mjpg/video.mjpg");
			cap >> in;
			if( in.cols==0) break;
		}
#else
		in = imread("image.jpg");
#endif
		Mat image;
		vector<Mat> images;
		split(in, images);
		imshow("Input-b1", images[B_]);
		
		//still debating about wether to use normal or adaptive thresholds.
		adaptiveThreshold(images[B_], images[B_], 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, -10);
		//threshold(images[B_], images[B_], 230, 255, THRESH_BINARY);
		
		imshow("Input-b2", images[B_]);
		
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		int thresh = 100;
		int max_thresh = 255;
		Canny( images[B_], images[B_], thresh, thresh*2, 3 );

		// Find contours
		findContours( images[B_], contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		vector<vector<Point> > contours_poly( contours.size() );
		vector<Rect> boundRect( contours.size() );
		vector<Point2f>center( contours.size() );

		// Draw contours
		Mat drawing = Mat::zeros( images[B_].size(), CV_8UC3 );
		for(int j =0; j < contours.size(); j++) // trim the extraneous contours
		{
			while(((contours[j].size() <= 4 )/*||(10.0f>contourArea(contours[j]))*/)&&(j < contours.size())) { //if not a rectangle or smaller than 15 pixels...				
				contours.erase(contours.begin()+j);
				hierarchy.erase(hierarchy.begin()+j);
			}
		}

		for( int i = 0; i < contours.size(); i++ )
		{ 
			approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		}

		for(int j =0; j < contours.size(); j++) // trim the extraneous bounding boxes
		{
			while(((5000.0f<boundRect[j].area())||(2000.0f>boundRect[j].area()))&&(j < contours.size())) { //a sort of band pass filter for rectangle area.			
				contours.erase(contours.begin()+j);
				hierarchy.erase(hierarchy.begin()+j);
				boundRect.erase(boundRect.begin()+j);
			}
		}

		vector<KeyPoint> centers;
		for( int i = 0; i< contours.size(); i++ )
		{
			drawContours( in, contours, i, Scalar( 255, 0,0 ), 1, 8, hierarchy, 0, Point() );
			rectangle( in, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 1, 8, 0 );
			centers.push_back(
				KeyPoint(
				((boundRect[i].tl().x + boundRect[i].br().x) / 2),
				((boundRect[i].tl().y + boundRect[i].br().y) / 2),
				1.0f)
				);
			drawKeypoints(in, centers, drawing, Scalar(0,0,255));
		}
	    // o=i(2/360)
		vector<Point2f> rCenters;
		const int centerX = drawing.cols/2;
		const int centerY = drawing.rows/2;
		cout<<"rows: " << drawing.rows << " cols: " << drawing.cols<<endl;
		for(int i = 0; i<centers.size(); i++){
			rCenters.push_back(Point2f(
				((centers[i].pt.x-centerX)*(2/(float)drawing.cols)), 
				((centers[i].pt.y-centerY)*(2/(float)drawing.rows))));

		}
		cout<<"[ID] [CenterX,CenterY] [Apparent Size (px.)] [relative position to center]"<<endl;
		for(int i = 0; i< centers.size(); i++){
			
			cout << "[" << i << "] " << centers[i].pt <<" ["<<boundRect[i].area()<<"] "<<rCenters[i]<<endl;
		}

		imshow("contour", drawing);

		// cleanup before next pass
		contours.clear();
		images.clear();
		hierarchy.clear();
		centers.clear();
		boundRect.clear();
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
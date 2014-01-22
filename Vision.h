#pragma once
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
using namespace cv;
using namespace std;

#define R_ 2
#define G_ 1
#define B_ 0

class Vision
{
private:
	Mat in;
	int thresh;
	unsigned int contourSize;
	vector<vector<Point> > contours_poly;
	vector<Point2f> center;
public:
	
	int centerX;
	int centerY;
	vector<KeyPoint> centers;
	vector<Point2f> rCenters;
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours;
	vector<Mat> images;
	vector<Rect> boundRect;
	Vision(void);
	Vision(Mat);
	~Vision(void);
	void filterContours();
	void processContours();
	void doThreshold();
};

//for reference:

	/*vector<Mat> images;
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
	*/
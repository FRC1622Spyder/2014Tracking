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
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<KeyPoint> centers;
	vector<Point2f> rCenters;
	int centerX;
	int centerY;
	int thresh;
	vector<vector<Point> > contours_poly;
	vector<Rect> boundRect;
	vector<Point2f> center;
	vector<Mat> images;
public:
	Vision(void);
	Vision(Mat);
	~Vision(void);
	void filterContours();
	void processContours();
	void threshold();
};


#pragma once
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
using namespace cv;
using namespace std;

#define R_ 2
#define G_ 1
#define B_ 0


template <class T>
class bandPass{
public:
	T high;
	T low;
};
class Vision
{
private:
	Mat in;
	Mat drawing;
	unsigned int contourSize;
	vector<vector<Point> > contours_poly;
	vector<Point2f> center;
	bandPass<float> bBoxBand;
	double cannyThresh1;
	double cannyThresh2;
	int centerX;
	int centerY;
	vector<KeyPoint> centers;
	vector<Point2f> rCenters;
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours;
	vector<Mat> images;
	vector<Rect> boundRect;
	
	double thresholdMin;
	double thresholdMax;
	double adaptThresholdVoxSz;
	double adaptThresholdDiff;
public:
	void setThreshold(double, double);
	void setAdaptThresh(double, double);
	int getCenterX(void);
	int getCenterY(void);
	vector<KeyPoint> getCenters(void);
	vector<Point2f> getRCenters(void);
	vector<Vec4i> getHierarchy(void);
	vector<vector<Point>> getContours(void);
	vector<Mat> getImages(void);
	vector<Rect> getBoundRect(void);
	Mat getDrawing(void);
	Vision(void);
	Vision(Mat);
	~Vision(void);

	void filterContours();
	void processContours();
	void doThreshold();
	void drawCenters();
	void setBBoxBand(bandPass<float>);
	bandPass<float> getBBoxBand(void);
	void setCannyThresh(double);
	double getCannyThresh(void);
};

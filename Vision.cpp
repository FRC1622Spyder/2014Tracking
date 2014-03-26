
#include "stdafx.h"
#include <stdlib.h>
#include "Vision.h"
#include "config.h"

Vision::Vision()
{
	this->centerX=240/2;
	this->centerY=360/2;
}

Vision::Vision(Mat inp)
{
	this->in = inp.clone();
	this->centerX = inp.cols/2;
	this->centerY = inp.rows/2;
	this->cannyThresh1 = 100;
	this->bBoxBand.high = 5000;
	this->bBoxBand.low = 1000;
	this->thresholdMin = 230;
	this->thresholdMax = 255;
	this->adaptThresholdDiff = -10;
	this->adaptThresholdVoxSz = 21;
}

Vision::~Vision(void)
{

}

void Vision::setBBoxBand(bandPass<float> b)
{
	this->bBoxBand.high = b.high;
	this->bBoxBand.low = b.low;
}

bandPass<float> Vision::getBBoxBand()
{
	return this->bBoxBand;
}

void Vision::setCannyThresh(double thresh)
{
	this->cannyThresh1 = thresh;
}

double Vision::getCannyThresh()
{
	return this->cannyThresh1;
}

int Vision::getCenterX(void)
{
	return this->centerX;
}

int Vision::getCenterY(void)
{
	return this->centerY;
}

vector<KeyPoint> Vision::getCenters(void)
{
	return this->centers;
}

vector<Point2f> Vision::getRCenters(void)
{
	return this->rCenters;
}

vector<Vec4i> Vision::getHierarchy(void)
{
	return this->hierarchy;
}

vector<vector<Point>> Vision::getContours(void)
{
	return this->contours;
}

vector<Mat> Vision::getImages(void)
{
	return this->images;
}

vector<Rect> Vision::getBoundRect(void)
{
	return this->boundRect;
}

void Vision::setThreshold(double min, double max)
{
	this->thresholdMin = min;
	this->thresholdMax = max;
}

void Vision::setAdaptThresh(double voxSz, double diff) //* voxel sample size, gradient difference.
{
	this->adaptThresholdDiff = diff;
	this->adaptThresholdVoxSz = voxSz;
}

void Vision::filterContours()
{
	try{
		if(contours.size() != 0)
		{
			for(unsigned int j = 0; j < this->contours.size(); j++) // trim the extraneous contours
			{
				while((this->contours[j].size() <= 4 ) 
					&& (j+1 < this->contours.size())) 
				{ //if chain contains less than 4 points				
					this->contours.erase(this->contours.begin()+j);
					this->hierarchy.erase(this->hierarchy.begin()+j);
				}
			}

			for(unsigned int i = 0; i < this->contours.size(); i++ )
			{ 
				approxPolyDP( Mat(this->contours[i]), this->contours_poly[i], 3, true );
				this->boundRect[i] = boundingRect( Mat(this->contours_poly[i]) );
			}
#ifndef EXTRA_OBJECTS
			for(unsigned int j = 0; j < this->contours.size(); j++) // trim the extraneous bounding boxes
			{
				while(((this->bBoxBand.high < this->boundRect[j].area()) ||	(this->bBoxBand.low > this->boundRect[j].area())) 
					&& (j+1 < this->contours.size())) 
				{ //a sort of band pass filter for rectangle area.			
					this->contours.erase(this->contours.begin()+j);
					this->hierarchy.erase(this->hierarchy.begin()+j);
					this->boundRect.erase(this->boundRect.begin()+j);
				}
			}
			for(unsigned int j = 0; j < this->boundRect.size(); j++) // trim horizontal boxes
			{
				while((this->boundRect[j].width > this->boundRect[j].height) 
					&& (j+1<this->boundRect.size()))
				{
					this->contours.erase(this->contours.begin()+j);
					this->hierarchy.erase(this->hierarchy.begin()+j);
					this->boundRect.erase(this->boundRect.begin()+j);
				}
			}
#endif
		}
	}
	catch(Exception &e)
	{
		cout<<e.what();
	}
}

void Vision::processContours()
{
	this->cannyThresh2 = cannyThresh1*2;
	Canny( this->images[B_], this->images[B_], this->cannyThresh1, this->cannyThresh2, 3 ); //edge detection

	// Find contours
	findContours( this->images[B_], this->contours, this->hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	// now that we know the size of Vision::contours, set up these vectors.
	this->boundRect.resize( this->contours.size() );
	this->center.resize( this->contours.size() );
	this->contours_poly.resize( this->contours.size() );
}

void Vision::doThreshold()
{
	split(this->in, this->images);
	if(images.size() == 0) // this shouldn't ever happen.
	{
		abort(); // split failed? Nothing to do here.s
	}
	imshow("Input-b1", this->images[B_]);
	
	
	//still debating about wether to use normal or adaptive thresholds.
	//adaptiveThreshold(this->images[B_], this->images[B_], 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, this->adaptThresholdVoxSz, this->adaptThresholdDiff);//21, -10
	threshold(this->images[B_], this->images[B_], this->thresholdMin, this->thresholdMax, THRESH_BINARY);//230, 255
	imshow("Input-b2", this->images[B_]);
}

void Vision::drawCenters()
{
	if(this->contours.size() != 0)
	{
		for(unsigned int i = 0; i< this->contours.size(); i++ )
		{
			drawContours( this->in, this->contours, i, Scalar( 255, 0,0 ), 1, 8, this->hierarchy, 0, Point() );
			rectangle( in, this->boundRect[i].tl(), this->boundRect[i].br(), Scalar(0,255,0), 1, 8, 0 );
			float xavg = 0.0f;
			float yavg = 0.0f;
			xavg=(float)((this->boundRect[i].tl().x) + this->boundRect[i].br().x)/2;
			yavg=(float)((this->boundRect[i].tl().y) + this->boundRect[i].br().y)/2;
			this->centers.push_back(
				KeyPoint(xavg, yavg, 1.0f));
			drawKeypoints(this->in, this->centers, this->drawing, Scalar(0,0,255));
		}
		for(unsigned int i = 0; i < this->centers.size(); i++)
		{
			this->rCenters.push_back(Point2f(
				((this->centers[i].pt.x-this->centerX)*(2/(float)in.cols)), 
				((this->centers[i].pt.y-this->centerY)*(2/(float)in.rows))));
		}
	} else {
		this->drawing = this->in;
	}
}

Mat Vision::getDrawing()
{
	return this->drawing; 
}

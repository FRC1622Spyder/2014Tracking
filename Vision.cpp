
#include "stdafx.h"
#include "Vision.h"
Vision::Vision()
{
	this->centerX=240/2;
	this->centerY=360/2;
}


Vision::Vision(Mat inp)
{
	in = inp.clone();
	this->centerX = inp.cols/2;
	this->centerY = inp.rows/2;
}

Vision::~Vision(void)
{

}

//FIXME: error handling for when no contours are detected.
void Vision::filterContours()
{
	try{
		if(contours.size() !=0)
		{
			for(unsigned int j =0; j < this->contours.size(); j++) // trim the extraneous contours
			{
				while((this->contours[j].size() <= 4 )&&(j+1 < this->contours.size())) 
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

			for(unsigned int j =0; j < this->contours.size(); j++) // trim the extraneous bounding boxes
			{
				while(((5000.0f < this->boundRect[j].area())||(1000.0f > this->boundRect[j].area()))&&(j+1 < this->contours.size())) 
				{ //a sort of band pass filter for rectangle area.			
					this->contours.erase(this->contours.begin()+j);
					this->hierarchy.erase(this->hierarchy.begin()+j);
					this->boundRect.erase(this->boundRect.begin()+j);
				}
			}
			for(unsigned int j = 0; j < this->boundRect.size(); j++) // trim horizontal boxes
			{
				while((this->boundRect[j].width > this->boundRect[j].height)&&(j+1<this->boundRect.size()))
				{
					this->contours.erase(this->contours.begin()+j);
					this->hierarchy.erase(this->hierarchy.begin()+j);
					this->boundRect.erase(this->boundRect.begin()+j);
				}
			}
		}
	}
	catch(Exception &e)
	{
		cout<<e.what();
	}
}


void Vision::processContours()
{

	Canny( this->images[B_], this->images[B_], this->thresh, this->thresh*2, 3 ); //edge detection

	// Find contours
	findContours( this->images[B_], this->contours, this->hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );



	this->boundRect.resize( this->contours.size() );
	this->center.resize( this->contours.size() );
	this->contours_poly.resize( this->contours.size() );
}


void Vision::doThreshold()
{
	split(this->in, this->images);
	//#ifdef _DEBUG
	imshow("Input-b1", this->images[B_]);
	//#endif
	//still debating about wether to use normal or adaptive thresholds.
	//adaptiveThreshold(this->images[B_], this->images[B_], 245, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, -10);
	threshold(this->images[B_], this->images[B_], 230, 255, THRESH_BINARY);
	//#ifdef _DEBUG
	imshow("Input-b2", this->images[B_]);
	//#endif
}
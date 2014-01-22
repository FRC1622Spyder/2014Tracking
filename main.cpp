
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
#include "Vision.h"
using namespace cv;
using namespace std;

// NOTES:
// Keeping the object count down by filtering by size
// will keep the processing speed up.

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
		Vision v(in);
		v.doThreshold();
		v.processContours();
		v.filterContours();
		v.drawCenters();

		cout<<"rows: " << in.rows << " cols: " << in.cols<<endl;
		cout<<"[ID] [CenterX,CenterY] [Apparent Size (px.)] [relative position to center]"<<endl;
		try{
			for(unsigned int i = 0; i < v.getCenters().size(); i++)
			{
				cout << "[" << i << "] " << v.getCenters()[i].pt <<" ["<<v.getBoundRect()[i].area()<<"] "<<v.getRCenters()[i]<<endl;
			}
		}
		catch(Exception &e)
		{
			cout<<e.what();
			exit(2);
		}
		imshow("contour", v.getDrawing());
		v.~Vision();

#ifdef LIVE_CAP
		if(waitKey(30) >=0) break;
		counter++;
		cout << counter <<endl;
	}
#endif

	waitKey(0);
	return 0;
}

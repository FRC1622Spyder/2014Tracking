
#include "stdafx.h"
#include <opencv\cv.h>
#include <exception>
#include "config.h"
#include "Vision.h"
#include "Server.h"

using namespace cv;
using namespace std;

// NOTES:
// Keeping the object count down by filtering by size
// will keep the processing speed up.


void reopen(VideoCapture cap)
{
	cap.release();
	cap.open("http://10.16.22.11/mjpg/video.mjpg");
}
int main( int argc, char** argv )
{
	Server srv;
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

		vector<VisionPacketEntry> packet;

		cout<<"rows: " << in.rows << " cols: " << in.cols<<endl;
		cout<<"[ID] [CenterX,CenterY] [Apparent Size (px.)] [relative position to center]"<<endl;
		try{
			for(unsigned int i = 0; i < v.getCenters().size(); i++)
			{
				struct VisionPacketEntry ent;
				cout << "[" << i << "] " << v.getCenters()[i].pt <<" ["<<v.getBoundRect()[i].area()<<"] "<<v.getRCenters()[i]<<endl;
				ent.area = v.getBoundRect()[i].area();
				ent.id = i;
				ent.centerX = v.getCenters()[i].pt.x;
				ent.centerY = v.getCenters()[i].pt.y;
				ent.rCenterX = v.getRCenters()[i].x;
				ent.rCenterY = v.getRCenters()[i].y;
				srv.sendPacket(ent);
				Sleep(10);
			}
		}
		
		catch(cv::Exception &e)
		{
			cout<<e.what();
			exit(2);
		}
		catch(std::exception &e)
		{
			cout<<e.what();
			exit(2);
		}
		Mat d = v.getDrawing();
		if(((d.size().width>0 && d.size().height>0)))
		{
			imshow("contour", d);
			
		} else {
			cout << "getDrawing returned null image"<<endl;
		}
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

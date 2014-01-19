#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    
	namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.
    Mat image;
	VideoCapture cap("http://10.16.22.11/mjpg/video.mjpg");
    while(cap.isOpened())
	{
		cap>>image;
		if(image.empty()) break;
		imshow( "Display window", image ); // Show our image inside it.
		if(waitKey(30) >=0) break;
	}
    waitKey(0);
    return 0;
}
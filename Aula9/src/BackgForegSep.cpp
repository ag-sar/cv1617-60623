#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

bool type;
int method;
int threshValue;
string filename = "vid/vid1.mp4";
Mat frame;

int main()
{
	Mat fgMaskMOG2; //fg mask generated by MOG method
	//Ptr<BackgroundSubtractorMOG> pMOG;
	//pMOG = new BackgroundSubtractorMOG();
	BackgroundSubtractorMOG::BackgroundSubtractorMOG MOG;

	//opens default camera
	VideoCapture cap(0);
  VideoCapture capture(filename);

	//check if success
	if (!cap.isOpened())
		{
			cout << "Error when reading camera!" << endl;
			return -1;
		}
  if(!capture.isOpened())
	{
		cout << "Error when reading video file!" << endl;
		return -1;
	}

	//presents options to user
	system("clear");		//clears terminal window
	cout << endl << "---------------" << endl;
	cout << " Choose Video Capture[0] or load a Video File[1]" << endl;
	cout << "---------------" << endl << endl;
	cin >> type;

  if(type)
  {
    namedWindow( "Video File", 1);
    for( ; ; )
    {
      capture >> frame;
      if(frame.empty())
        break;
      imshow("Video File", frame);

			//update the background model
			MOG(frame, fgMaskMOG);
			//get the frame number and write it on the current frame
			stringstream ss;
			rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
			          cv::Scalar(255,255,255), -1);
			ss << capture.get(CAP_PROP_POS_FRAMES);
			string frameNumberString = ss.str();
			putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
			        FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
			//show the current frame and the fg masks
			imshow("Video File", frame);
			imshow("Video File Subtracted", fgMaskMOG);

      waitKey(20);
    }
		capture.release();
    waitKey(0);
  }
	else
	{
		namedWindow("Video Capture", 1);
		for(;;)
		{
			cap >> frame;
			if(frame.empty())
				break;
			imshow("Video Capture", frame);
			if (waitKey(5) >= 0) break;				//waits 30ms for program to render next frame
		}
		waitKey(0);
	}

}

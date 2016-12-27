#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


Mat roiFrame, roiAux;
Rect roiBox;
vector<Point2f> roiPts;		//points defining a ROI
int cnt = 0;				//mouse clicks counter

int funcInt;	//for program function type


static void roiSelection(int event, int x, int y, int, void*) {
	// This function waits for user to select points that define a ROI or a point of interest
	switch (event) {
		case CV_EVENT_LBUTTONDOWN:
			cnt++;

			//point selection and ROI definition
			if (cnt <= funcInt) {
				if (funcInt == 2) {
					//point selection and display
					Point selected = Point(x, y);
					roiPts.push_back(selected);

					circle(roiFrame, selected, 5, Scalar(0, 0, 255), 1);
					if (cnt == 2) {
						//ROI display and storage
						rectangle(roiFrame, roiPts[0], roiPts[1], Scalar(255, 0, 0), 2);
						roiBox = Rect(roiPts[0], roiPts[1]);
					}
				}
				else if (funcInt == 3) {
					//point selection and display
					Point first = Point(1, 1);
					roiPts.push_back(first);
					Point last = Point(roiFrame.size());
					roiPts.push_back(last);

					cnt = cnt + 2;

					Point selected = Point(x, y);
					roiPts.push_back(selected);

					circle(roiFrame, selected, 5, Scalar(0, 0, 255), 1);

					roiBox = Rect(roiPts[0], roiPts[1]);
				}
			}
			else {
				//flushes point vector
				roiFrame = roiAux.clone();
				roiPts.clear();
				cnt = 0;
			}

			imshow("Optical Flow", roiFrame);
	}
}

int main() {
	int sel;							//video selection
	bool func;							//program function type
	char file[20];
	int linesz;							//line thickness

	Mat frame, prevFrame, nextFrame, roi;

	vector<Point2f> prevPts, nextPts;	//frame features
	vector<uchar> status;				//status vector
	vector<float> err;					//error vector

	int nPts = 2000;					//number of features
	double qLevel = 0.05;				//quality level
	double minDist = 0.01;				//min euclidian distance

	float x1, y1, x2, y2;				//point coordinates for drawing
	vector<Point2f> linPts;				//auxilliary vector for point path

	//presents user interface
	system("clear");		//clears terminal window
	cout << endl
		 << " --------------------------" << endl
		 << "  Optical Flow for Project " << endl
		 << " --------------------------" << endl << endl
		 << " Choose video file [1 - ?]: ";
	cin >> sel;
	cout << endl;
	cout << " Choose [0 = ROI, 1 = single point]: ";
	cin >> func;
	cout << endl << endl;

	//properties for each program function
	if (func) {
		funcInt = 3;
		linesz = 2;
	}
	else {
		funcInt = 2;
		linesz = 1;
	}

	//video file
	VideoCapture cap;

	sprintf(file, "vid/vid%d.mp4", sel);
	//const char* video = "vid/vid1.mp4";
	cap.open(file);

	//check if success
	if (!cap.isOpened()) return -1;

	for (;;) {
        //displays point selection (cnt == 0 by default)
		if (cnt == 0) {
			cout
			<< " Select two points to define ROI with the mouse. A third mouse click will reset the selection." << endl
			<< " Press ENTER when the selection is made." << endl << endl;

			cap >> frame;		//gets new frame
			if(!frame.data) {
				cout << " Error: no video data found." << endl
				break;
			}

			//resizes video frames for viewing purposes
			resize(frame, frame, Size(), 1.5, 1.5, INTER_CUBIC);

			//converts frame to grayscale
			cvtColor(frame, nextFrame, CV_BGR2GRAY);

			//images for selection
			roiFrame = frame.clone();
			roiAux = roiFrame.clone();

			//mouse callback for selecting ROI
			imshow("Optical Flow", roiFrame);
			setMouseCallback("Optical Flow", roiSelection);
			waitKey(0);

			//if points have been selected and ROI defined
			if (roiPts.size() == funcInt) {
				//creates ROI and ROI mask
				roi = frame(roiBox);
				cvtColor(roi, nextFrame, CV_BGR2GRAY);

				if (func) {
					nextPts.push_back(roiPts[2]);
					linPts.push_back(roiPts[2]);
				}
			}
			else {
				cout << " Error: not enough points selected to form ROI." << endl;
				return -1;
			}
		}

		//gets image features
		if (!func) {
			goodFeaturesToTrack(nextFrame, nextPts, nPts, qLevel, minDist);
		}

		prevFrame = nextFrame.clone();		//first frame is the same as last one
		prevPts = nextPts;
		cap >> frame;						//gets a new frame from camera
		
		//ends tracking if video ends
		if(!frame.data) {
			cout << " Video has ended. Tracking Stopped." << endl << endl;
			break;
		}

		resize(frame, frame, Size(), 1.5, 1.5, INTER_CUBIC);
		cvtColor(frame, nextFrame, CV_BGR2GRAY);

		nextFrame = nextFrame(roiBox);

		//calculates optical flow using Lucas-Kanade
		calcOpticalFlowPyrLK(prevFrame, nextFrame, prevPts, nextPts, status, err);

		if (!func) {
			//draws ROI rectangle
			rectangle(frame, roiPts[0], roiPts[1], Scalar(255, 255, 0), 1);
		

			//draws motion lines on display
			for (int i = 0; i < nextPts.size(); i++) {
				if (status[i]) {
					x1 = roiPts[0].x + prevPts[i].x;
					y1 = roiPts[0].y + prevPts[i].y;
					x2 = roiPts[0].x + nextPts[i].x;
					y2 = roiPts[0].y + nextPts[i].y;

					/*cout << "PONTOS" << endl;
					cout << roiPts << endl;
					cout << prevPts[i] << endl;
					cout << nextPts[i] << endl << endl;

					cout << x1 << endl;
					cout << y1 << endl;
					cout << x2 << endl;
					cout << y2 << endl << endl;*/

					line(frame, Point(x1, y1), Point(x2, y2), Scalar(255, 255, 0), linesz);
				}
			}
		}
		else {
			//stores points in a new vector
			linPts.push_back(nextPts[0]);
			//cout << linPts << endl;
			//cout << linPts.size() << endl << endl;

			//draws the path of selected point
			for (int i = 0; i < linPts.size() - 1; i++) {
				if (status[0]) {
					line(frame, linPts[i], linPts[i+1], Scalar(255, 255, 0), linesz);
					
					//draws circle at current point position
					if (i == linPts.size() - 2) {
						circle(frame, linPts[i+1], 5, Scalar(0, 0, 255), 1);
					}
				}
			}
		}

		imshow("Optical Flow", frame);	//shows original capture

		//pauses and exits videos
		char key = waitKey(33);

		//if "space" pressed, pauses. If "esc" pressed, exits program
	    if (key == 32) {
	    	key = 0;
	    	while (key != 32) {
	    		imshow("Optical Flow", frame);
	    		key = waitKey(5);
	    		if (key == 27) break;
	    	}
	    	if (key == 27) {
	    		cout << " Tracking aborted by user. Exiting." << endl << endl;
	    		break;
	    	}
	    	else key = 0;
	    }
	    //if "esc" pressed, exits
	    if (key == 27) break;
	}

	return 0;
}
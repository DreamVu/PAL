/*

CODE SAMPLE # 013: Object Tracking With GPIO compatibilty
This code will grab the 360 rgb data, do object tracking and toggle GPIO pins of the Nvidia board.


>>>>>> Compile this code using the following command....


g++ 013_object_tracking_GPIO.cpp /usr/src/tensorrt/bin/common/logger.o ../lib/libPAL.so ../lib/libPAL_CAMERA.so  ../lib/libPAL_DEPTH_HQ.so ../lib/libPAL_DEPTH_128.so  ../lib/libPAL_DE.so ../lib/libPAL_EDET.so  ../lib/libPAL_Track.so  `pkg-config --libs --cflags opencv`   -O3  -o 013_object_tracking_GPIO.out -I../include/ -lv4l2 -lcudart -L/usr/local/cuda/lib64 -lnvinfer -lnvvpi -lnvparsers -lnvinfer_plugin -lnvonnxparser -lmyelin -lnvrtc -lcudart -lcublas -lcudnn -lrt -ldl -lJetsonGPIO -lpthread -w



>>>>>> Execute the binary file by typing the following command...

./013_object_tracking_GPIO.out


>>>>>> KEYBOARD CONTROLS:

	Press Ctrl+C key to exit the code sample


*/


# include <stdio.h>
# include <opencv2/opencv.hpp>
# include <chrono>
# include <bits/stdc++.h>
# include "PAL.h"
#include <JetsonGPIO.h>
#include "unistd.h"

// Linux headers
# include <fcntl.h> // Contains file controls like O_RDWR
# include <errno.h> // Error integer and strerror() function
# include <termios.h> // Contains POSIX terminal control definitions
# include <unistd.h> // write(), read(), close()



static bool g_bExit = false;

void signalHandler( int signum )
{
	g_bExit = true;
}


//camera and detection pins
int camera_pin = 15;
int detection_pin = 7;
int camera_pin_1 = 29;
int camera_pin_2 = 31;
int camera_pin_3 = 33;

using namespace cv;
using namespace std;

using namespace std::chrono;

namespace PAL
{

	int RunTrack(cv::Mat& img, cv::Mat& depth, vector<vector<float>> &boxes, 
	    vector<int> &ids, vector<float> &depthValues, vector<Scalar> &colours);
}

namespace PAL
{
	namespace Internal
	{
		void EnableDepth(bool flag);
		void MinimiseCompute(bool flag);
	}
}

int main(int argc, char *argv[])
{
	
	signal(SIGINT, signalHandler);

	PAL::Internal::EnableDepth(false);
	PAL::Internal::MinimiseCompute(true);

	//setting mode to GPIO board
	GPIO::setmode(GPIO::BOARD);

	//setting camera pins active
	GPIO::setup(camera_pin, GPIO::OUT, GPIO::LOW);
	GPIO::setup(camera_pin_1, GPIO::OUT, GPIO::LOW);
	GPIO::setup(camera_pin_2, GPIO::OUT, GPIO::LOW);
	GPIO::setup(camera_pin_3, GPIO::OUT, GPIO::LOW); 




	//setting detection pin inactive by default
	GPIO::setup(detection_pin, GPIO::OUT, GPIO::HIGH);



	cv::VideoCapture cap;

	while(!cap.open("/dev/pal5")) 
	{
		cout << "\n\n[INFO] CAMERA NOT CONNECTED, CONNECT THE CAMERA \n\n" << endl;
		system("clear");
		
		GPIO::output(camera_pin, 1);
		GPIO::output(camera_pin_1, 1);
		GPIO::output(camera_pin_2, 1);
		GPIO::output(camera_pin_3, 1);
		GPIO::output(detection_pin, 1);
	}
	
	cap.release();
	
	int width, height;
	while(PAL::Init(width, height, -1) != PAL::SUCCESS) //Connect to the PAL camera
	{
		printf("\n[INFO] Init Failed\n");
		return 1;
	}
	GPIO::output(camera_pin, 0);
	GPIO::output(camera_pin_1, 0);
	GPIO::output(camera_pin_2, 0);
	GPIO::output(camera_pin_3, 0);


	PAL::CameraProperties data; 
	PAL::Acknowledgement ack = PAL::LoadProperties("../Explorer/SavedPalProperties.txt", &data);
	if(ack != PAL::SUCCESS)
	{
	printf("Error Loading settings\n");
	}

	PAL::CameraProperties prop;

	unsigned int flag = PAL::MODE;
	flag = flag | PAL::FD;
	flag = flag | PAL::NR;
	flag = flag | PAL::FILTER_SPOTS;
	flag = flag | PAL::VERTICAL_FLIP;

	prop.mode = PAL::Mode::TRACKING;
	prop.fd = 1;
	prop.nr = 0;
	prop.filter_spots = 1;
	prop.vertical_flip = 0;
	PAL::SetCameraProperties(&prop, &flag);

	printf("Press Ctrl+C to exit the app\n");   


	bool flip = false;
	bool filter_spots = true;
	bool nr = false;
	bool fd = true;

	vector<vector<float>> boxes; 
	vector<int> ids; 
	vector<float> depthValues; 
	vector<Scalar> colours; 
	int num;
	bool useDepth = false;

	bool bDetectionPinActive = false;


	int factor = 0; 
	PAL::Acknowledgement cam_ack;
	//27 = esc key. Run the loop until the ESC key is pressed
	while (!g_bExit)
	{


		
		PAL::Image left, right, depth, disparity;
		Mat img, d;
		if (useDepth)
			cam_ack = PAL::GrabFrames(&left, &right, &depth);
		else
			cam_ack = PAL::GrabFrames(&left, &right);
		
		if(cam_ack == PAL::Acknowledgement::FAILURE)
		{
			GPIO::output(camera_pin, 1);
			GPIO::output(camera_pin_1, 1);
			GPIO::output(camera_pin_2, 1);
			GPIO::output(camera_pin_3, 1);
			GPIO::output(detection_pin, 1);
			PAL::CameraStatus();
			GPIO::output(camera_pin, 0);
			GPIO::output(camera_pin_1, 0);
			GPIO::output(camera_pin_2, 0);
			GPIO::output(camera_pin_3, 0);
			continue;
		}
		
		//Convert PAL::Image to Mat
		img = Mat(left.rows, left.cols, CV_8UC3, left.Raw.u8_data);



		if (useDepth)
		{
			d = Mat(depth.rows, depth.cols, CV_32FC1, depth.Raw.f32_data);
		}
		else
		{
			d = cv::Mat::zeros(cv::Size(1, 1), CV_32FC1);
		}
		

		num = PAL::RunTrack(img, d, boxes, ids, depthValues, colours);


		if(num)
		{
			if(!bDetectionPinActive)
			{
				cout<<"Setting GPIO detection pin active"<<endl;
				GPIO::output(detection_pin, 0);

				bDetectionPinActive = true;
			}

		}
		else
		{
			if(bDetectionPinActive)
			{
				cout<<"Setting GPIO detection pin inactive"<<endl;
				GPIO::output(detection_pin, 1);
				bDetectionPinActive = false;
			}

		}


		
		boxes.clear();
		ids.clear();
		if(useDepth)
		depthValues.clear();
		colours.clear();
		
		//Wait for the keypress - with a timeout of 1 ms
		
		
	}

	printf("exiting the application\n");

	GPIO::setup(camera_pin, GPIO::OUT, GPIO::HIGH);
	GPIO::setup(camera_pin_1, GPIO::OUT, GPIO::HIGH);
	GPIO::setup(camera_pin_2, GPIO::OUT, GPIO::HIGH);
	GPIO::setup(camera_pin_3, GPIO::OUT, GPIO::HIGH);

	GPIO::setup(detection_pin, GPIO::OUT, GPIO::HIGH);

	GPIO::cleanup();

	PAL::Destroy();

	return 0;
}

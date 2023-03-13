/*

CODE SAMPLE # 002: PAL Depth Panorama
This code will grab the left & depth panorama and display in a window using opencv


>>>>>> Compile this code using the following command....

./compile.sh 002_depth_panorama.cpp

>>>>>> Execute the binary file by typing the following command...

./002_depth_panorama.out


>>>>>> KEYBOARD CONTROLS:

ESC key closes the window
Press v/V to toggle vertical flip property    
Press f/F to toggle filter rgb property       

*/


#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "PAL.h"
#include "TimeLogger.h"
#include <time.h>
#include <unistd.h>

using namespace cv;
using namespace std;


Mat getColorMap(Mat img, float scale)
{
    Mat img_new = img * scale;
    img_new.convertTo(img_new, CV_8UC1);
    img_new = 255-img_new;
    applyColorMap(img_new, img_new, COLORMAP_JET);
    return img_new;
}

int main( int argc, char** argv )
{
	// Create a window for display.
	namedWindow( "PAL Depth Panorama", WINDOW_NORMAL ); 

	int width, height;
	PAL::Mode mode = PAL::Mode::LASER_SCAN;

	std::vector<int> camera_indexes{5};
	
	if(argc > 1) 
		camera_indexes[0] = std::atoi(argv[1]);


	PAL::Mode def_mode = PAL::Mode::LASER_SCAN;

	char path[1024];
	sprintf(path,"/usr/local/bin/data/pal/data%d/",camera_indexes[0]);

	char path2[1024];
	sprintf(path2,"/usr/local/bin/data/pal/data%d/",6);

	PAL::SetPathtoData(path, path2);
	
	//Connect to the PAL camera
	if (PAL::Init(width, height, camera_indexes, &def_mode) != PAL::SUCCESS) 
	{
		cout<<"Init failed"<<endl;
		return 1;
	}
	
	PAL::SetAPIMode(PAL::API_Mode::DEPTH);
	usleep(1000000);

	PAL::CameraProperties data;
	PAL::Acknowledgement ack_load = PAL::LoadProperties("../../Explorer/SavedPalProperties.txt", &data);
	if(ack_load == PAL::Acknowledgement::INVALID_PROPERTY_VALUE)
	{
		PAL::Destroy();
		return 1;
	}
	
	if(ack_load != PAL::SUCCESS)
	{
		cout<<"Error Loading settings! Loading default values."<<endl;
	}

	//discarding initial frames
	std::vector<PAL::Data::ODOA_Data> discard;
	for(int i=0; i<5;i++)
		discard =  PAL::GrabRangeScanData();		

	//width and height are the dimensions of each panorama.
	//Each of the panoramas are displayed at their original resolution.
	resizeWindow("PAL Depth Panorama", width, height);

	int key = ' ';

	cout<<"\n\nPress ESC to close the window."<<endl;
	printf("Press v/V to toggle vertical flip property\n");	
	printf("Press f/F to toggle filter rgb property\n\n");
	
	bool filter_spots = data.filter_spots;
	bool flip = data.vertical_flip;	
	Mat output = cv::Mat::zeros(height, width, CV_8UC3);
	bool raw_depth = data.raw_depth;

	//Display the overlayed image
	imshow( "PAL Depth Panorama", output);

	//27 = esc key. Run the loop until the ESC key is pressed
	while(key != 27)
	{

		std::vector<PAL::Data::ODOA_Data> data;

		data =  PAL::GrabRangeScanData();	
		
		Mat display;
		Mat l = data[0].left;
		Mat d;
		if(raw_depth)
			d = data[0].fused_depth.clone();
		else
			d = data[0].distance.clone();
		
		
		d = getColorMap(d, 1);

		//Vertical concatenation of rgb and depth into the final output
		vconcat(l, d, display);

		//Display the depth with rgb panorama
		imshow( "PAL Depth Panorama", display);  

		//Wait for the keypress - with a timeout of 1 ms
		key = waitKey(1) & 255;
		
		if (key == 'f' || key == 'F')
		{	
			PAL::CameraProperties prop;
			filter_spots = !filter_spots;
			prop.filter_spots = filter_spots;
			unsigned long int flags = PAL::FILTER_SPOTS;
			PAL::SetCameraProperties(&prop, &flags);
		}
		if (key == 'v' || key == 'V')
		{		    
			PAL::CameraProperties prop;
			flip = !flip;
			prop.vertical_flip = flip;
			unsigned long int flags = PAL::VERTICAL_FLIP;
			PAL::SetCameraProperties(&prop, &flags);
		}

	}

	printf("exiting the application\n");
	PAL::Destroy();

   
    return 0;
}


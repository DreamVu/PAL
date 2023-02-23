/*

CODE SAMPLE # 003: Range scan panorama
This code will grab the left panorama with range scan overlayed on it and would be displayed in a window using opencv


>>>>>> Compile this code using the following command....

./compile.sh 003_range_scan_panorama.cpp

>>>>>> Execute the binary file by typing the following command...

./003_range_scan_panorama.out


>>>>>> KEYBOARD CONTROLS:

ESC key closes the window
Press v/V to toggle vertical flip property   
        
*/


#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "PAL.h"
#include "TimeLogger.h"
#include <time.h>
#include <unistd.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	// Create a window for display.
	namedWindow( "PAL Range Scan", WINDOW_NORMAL ); 

	int width, height;
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
	
	usleep(1000000);
	PAL::SetAPIMode(PAL::API_Mode::RANGE_SCAN);
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
	//Each of the panoramas are displayed at otheir original resolution.
	resizeWindow("PAL Range Scan", width, height);

	int key = ' ';

	cout<<"\n\nPress ESC to close the window."<<endl;
	printf("Press v/V to toggle vertical flip property\n\n");	
	
	bool flip = data.vertical_flip;	
	Mat output = cv::Mat::zeros(height, width, CV_8UC3);

	//Display the overlayed image
	imshow( "PAL Range Scan", output);

	//27 = esc key. Run the loop until the ESC key is pressed
	while(key != 27)
	{

		std::vector<PAL::Data::ODOA_Data> data;

		data =  PAL::GrabRangeScanData();	

		//Display the overlayed image
		imshow( "PAL Range Scan", data[0].marked_left);  

		//Wait for the keypress - with a timeout of 1 ms
		key = waitKey(1) & 255;
		
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


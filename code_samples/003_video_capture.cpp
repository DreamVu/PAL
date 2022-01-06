/*

CODE SAMPLE # 003: Video/image capture

This code will save the panoramas into still images or videos - depending on the user input


>>>>>> Compile this code using the following command....


g++ 003_video_capture.cpp ../lib/PAL.a `pkg-config --libs --cflags opencv`    -O3 -o 003_video_capture -I../include/ -lv4l2


>>>>>> Execute the binary file by typing the following command...


./003_video_capture


>>>>>> KEYBOARD CONTROLS:

       'B' key begins the video capture
       'E' key ends the video capture
       'C' key captures a single frame into a PNG file
       ESC key closes the window
       

*/
# include <stdio.h>
# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include "PAL.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    namedWindow( "PAL Video Capture", WINDOW_NORMAL );
    
    int width, height;
    if(PAL::Init(width, height,-1) != PAL::SUCCESS)
    {
        printf("Init failed\n");
        return 1;
    }

    PAL::CameraProperties data; 
    PAL::Acknowledgement ack = PAL::LoadProperties("../Explorer/SavedPalProperties.txt", &data);
    if(ack != PAL::SUCCESS)
    {
        printf("Error Loading settings\n");
    }

    PAL::CameraProperties prop;

    unsigned int flag = PAL::MODE;
    flag = flag | PAL::FILTER_SPOTS;
    flag = flag | PAL::VERTICAL_FLIP;
    flag = flag | PAL::FD;
    
    prop.fd = 1;
    prop.mode = PAL::Mode::FAST_DEPTH; // The other available option is PAL::Mode::HIGH_QUALITY_DEPTH
    prop.filter_spots = 1;
    prop.vertical_flip =0;
    PAL::SetCameraProperties(&prop, &flag);
    
    //width and height are the dimensions of each panorama.
    //Each of the panoramas are displayed at quarter their original resolution.
    //Since the left+right+disparity are vertically stacked, the window height should be thrice the quarter-height
    int window_width = width/4;
    int window_height = (height/4)*2;
    resizeWindow("PAL Video Capture", window_width, window_height);

    int key = ' ';
    bool record = false;
    bool closed = false;
    bool filter_spots = true;   
    bool flip = false;
    
    printf("Press ESC to close the window\n");
    printf("Press f/F to toggle filter rgb property\n");
    printf("Press v/V to toggle vertical flip property\n");
    printf("Press C to capture a single frame into a PNG file\n");
    printf("Press B to begin the video capture\n");
    printf("Press E to end the video capture\n");
    
    cv::VideoWriter video;
    
    while(!closed)
    {
        PAL::Image left, right, depth, disparity;

        PAL::GrabFrames(&left, &right, &depth);

        Mat temp, output;
        //Convert PAL::Image to cv::Mat
        Mat l = Mat(left.rows, left.cols, CV_8UC3, left.Raw.u8_data);
        Mat d = Mat(depth.rows, depth.cols, CV_32FC1, depth.Raw.f32_data);
	    d.convertTo(d, CV_8UC1);
        cvtColor(d, d, cv::COLOR_GRAY2BGR);
        
        vconcat(l, d, output);
        //Display the final vertically concatinated image
        imshow( "PAL Video Capture", output);  
        key = waitKey(1) & 255;
        

        if (key == 'f' || key == 'F')
        {   
            PAL::CameraProperties prop;
            filter_spots = !filter_spots;
            prop.filter_spots = filter_spots; 
            unsigned int flags = PAL::FILTER_SPOTS;
            PAL::SetCameraProperties(&prop, &flags);
        }
        if (key == 'v' || key == 'V')
        {           
            PAL::CameraProperties prop;
            flip = !flip;
            prop.vertical_flip = flip; 
            unsigned int flags = PAL::VERTICAL_FLIP;
            PAL::SetCameraProperties(&prop, &flags);
        }
        if(key == 'C' || key == 'c') //capture an image using imwrite
        {
            imwrite("image.png", output);
            printf("The current frame is saved as image.png\n");
        }
        else if(key == 'B' || key == 'b')
        {
            cv::Size size = cv::Size(output.cols, output.rows);
            int fps = 10;
            printf("Opening the video\n");
            video = cv::VideoWriter("pal_video.avi",cv::VideoWriter::fourcc('X','V','I','D'), fps, size);
            record = true;
        }
        else if (key == 'E' || key == 'e' || key == 27)
        {
            closed = true;
            if(record)
            {
                record = false;            
                printf("Releasing the video \n");
                video.release();
            }
        }
        if(record)
        { 
            video.write(output);
        }
    }

    printf("exiting the application\n");
    PAL::Destroy();
    
    return 0;
}


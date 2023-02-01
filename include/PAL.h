# ifndef PAL_H
# define PAL_H

//This is the only file the end-user needs to include in the application
//All the functionality provided by the API is covered here.

# include <vector>

# include "DataExchange.h"
# include "CameraProperties.h" 

namespace PAL
{

		//Pipeline g_oPipeline;
		//Mini(){};
		//Initializes the PAL API
		//returns SUCCESS/FAILURE etc.
		PAL::Acknowledgement Init(int& panoramaWidth, int& panoramaHeight, std::vector<int> camera_indexes, void* arg=nullptr);
		
		
		//This is a blocking call, waits till all the pending depth / disparity computations are finished and returns.
		//This should be used only when asynchronous is true in GrabFrames function arguments
		void Synchronize();

        PAL::Acknowledgement SetPathtoData(std::string path1, std::string path2="");

		//Writes the current camera properties into the provided memory location
		PAL::Acknowledgement GetCameraProperties(PAL::CameraProperties* properties);
		
		
		// SetCameraProperties
		// changes the camera properties like gamma, saturation etc.
		//
		// ARGUMENTS:
		// flags (read/write)		: Should point to a value formed by one/more combinations of CameraPropertyFlags
		// properties (readonly)	: Only those members are updated, who correspondings flags are set.
		//
		/* EXAMPLE:

			PAL::CameraProperties properties;
			properties.saturation = 2.0f;
			properties.gamma = 30000.0f;
			int flags = PAL::SATURATION | PAL::GAMMA;
			PAL_SDK::SetCameraProperties(&properties, &flags);
		*/
		// RETURNS:
		// 
		// returns SUCCESS/INVALID_PROPERTY_VALUE etc.
		// On successful return, flags should point to zero.
		// In case if an invalid properties are sent, 
		// the corresponding CameraPropertyFlags would be set to the int location pointed by flags
		// Refer API Doc for more information about this function
		PAL::Acknowledgement SetCameraProperties(PAL::CameraProperties* properties, unsigned long int *flags);


		//Returns a vector of available resolutions. 
		//While changing the resolution through SetCameraProperties...
		//users can use one of the available resolutions
		std::vector<PAL::Resolution> GetAvailableResolutions();


		//This function resets the camera properties.
		//If this function is called before the Init function, it would be ignored. Else, returns SUCCESS
		//If a pointer to PAL::CameraProperties is provided, the default values would be written into that location
		PAL::Acknowledgement SetDefaultCameraProperties(PAL::CameraProperties* properties = 0);
		

		//If a vector of Points is provided, those points would be saved with the mentioned fileName
		//If the vector is not provided, GetPointCloud function would be used internally
		PAL::Acknowledgement SavePointCloud(const char* fileName, std::vector<PAL::Point> *pc = 0);

		//Saves the current camera properties into the provided file name
		PAL::Acknowledgement SaveProperties(const char* fileName);

		//Loads the camera properties saved in the provided file
		//If data argument is provided, the properties in the file would be written into data
	    PAL::Acknowledgement LoadProperties(const char* fileName, PAL::CameraProperties* data = 0);

		//Initializes the Person Detection API from the provided path
		//returns SUCCESS/FAILURE etc.
		//ARGUMENTS:
		//threshold	: Confidence threshold of the detections ranging from 0 to 1 
	    PAL::Acknowledgement InitPersonDetection(float threshold = 0.3);

		//Grabs the latest person detection data available
		//returns SUCCESS/FAILURE etc.
		//ARGUMENTS:
		//left			: Panorama - as seen by the left eye - OpenCV CV_8UC3 type
		//depth			: Panorama - depth values in cm as seen by the depth sensor - OpenCV CV_32FC1 type
		//BoundingBoxes : writes bounding box values of the detections in the provided memory location
		//Loc3Ds		: writes 3D locations of the detections in the provided memory location
		//timestamp     : writes timestamp value of the data retrieved 
		PAL::Acknowledgement GetPeopleDetection(cv::Mat& rgb, cv::Mat& depth,    
				                    std::vector<PAL::BoundingBox> *BoundingBoxes, 
				                    timeval *timestamp = 0);

		//Sets height of camera from floor required for floor mapping
		//ARGUMENTS:
		//camera_height: Approximate Height of PAL camera from the floor in cm
		PAL::Acknowledgement SetFloorMappingParameters(int camera_height = 100);
		
		//Destroys all the resources related to Camera communication	
		void Destroy();
		
		void UpdateZoomParams(int updated_Width, int updated_Height, int x_c, int y_c);
		void DisableTRTModels(bool flag);

		std::vector<PAL::Data::Stereo> GetStereoData();
		PAL::Data::Depth  GetDepthData();
		PAL::Data::People GetPeopleData(bool depth);
		PAL::Data::PointCloud GetPointCloudData();
		bool SavePointCloud(const char* fileName, cv::Mat pcMat);
		std::vector<PAL::Data::ODOA_Data> GrabRangeScanData();
		PAL::Acknowledgement EnablePC(bool PC);
		void SetCliffMaskFlag(bool flag);
		void PauseComputation(bool flag);
		void SetAPIMode(int mode);
		void SetRemapRGBMode(bool rgb);
	
}

# endif //PAL_H

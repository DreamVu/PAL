# CODE SAMPLE # 002: Depth panorama
# This code will grab the left & depth panorama and display in a window using opencv

import sys
import PAL_PYTHON
import cv2
import numpy as np

def main():

	# Initialising camera
	image_width = 0
	image_height = 0
	camera_index = 5	
	arg = len(sys.argv)

	if arg == 2:
		camera_index = int(sys.argv[1])
		
	path = "/usr/local/bin/data/pal/data"+str(camera_index)+"/"	
	PAL_PYTHON.SetPathtoDataP(path)
		
	width, height, ack_init = PAL_PYTHON.InitP(image_width, image_height, camera_index)

	if ack_init != PAL_PYTHON.SUCCESSP:
		print("Camera Init failed\n")
		return

	PAL_PYTHON.SetAPIModeP(PAL_PYTHON.DEPTHP)

	loaded_prop = {}
	prop = PAL_PYTHON.createPALCameraPropertiesP(loaded_prop)
	
	loaded_prop, ack_load = PAL_PYTHON.LoadPropertiesP("../../Explorer/SavedPalProperties.txt", prop)

	if ack_load != PAL_PYTHON.SUCCESSP:
		print("Error Loading settings! Loading default values.")
	
	# Creating a window
	source_window = 'PAL Depth Panorama'
	cv2.namedWindow(source_window, cv2.WINDOW_NORMAL)
	
	# Current image resolution
	#print("The image resolution is : ", width, "x", height, "\n")

	# Changing window size
	cv2.resizeWindow(source_window, (int(width), int((height)*2)))

	key = ' '

	print("Press ESC to close the window.")
	print("Press f/F to toggle filter rgb property.")

	flip = False
	filter_spots = bool(loaded_prop["filter_spots"])
	pitch = int(loaded_prop["pitch"])
	
	# ESC
	while key != 27:
		# GrabFrames function
		left, right, depth  = PAL_PYTHON.GrabDepthDataP()

		# BGR->RGB FLOAT->RGB
		left_mat = cv2.cvtColor(left,cv2.COLOR_BGR2RGB)
		depth_mat = np.uint8(depth)
		depth_mat = cv2.cvtColor(depth_mat, cv2.COLOR_GRAY2RGB)
		
		# Concatenate vertically
		concat_op = cv2.vconcat([left_mat,depth_mat])

		# Show results
		cv2.imshow(source_window, concat_op)

		# Wait for 1ms
		key = cv2.waitKey(1) & 255
		#print(key)

		if key == 102:		    
			flag = PAL_PYTHON.FILTER_SPOTSP
			filter_spots = not(filter_spots)
			loaded_prop["filter_spots"] = filter_spots
			prop, flags, res_scp = PAL_PYTHON.SetCameraPropertiesP(loaded_prop, flag)

		

		

	# Destroying connections
	print("exiting the application\n")
	PAL_PYTHON.DestroyP()

	return

if __name__ == "__main__":
    main()




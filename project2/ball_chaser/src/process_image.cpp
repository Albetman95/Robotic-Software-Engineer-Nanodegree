#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Moving the robot towards the ball");
	
    //Request velocities to drive the robot
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;
	
    // Call the ball_chaser service and pass the requested velocities
	if(!client.call(srv))
		ROS_ERROR("Failed to call service ball_chaser/command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool ball_detected = false;
    float lin_x = 0.0;
    float ang_z = 0.0;
    int white_pixel_row = 0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    /*------------------------------------------------------------------------------------ Solution ---------------------------------------------
   |                                                                                                                                            |
   |-->>  Matrix each pixel is = Height * Width = 800 * 800                                                                                     |
   | -->>  Step = means the entire row value = 2400                                                                                             |
   |-->>  encodig = "RGB8" = means each color is store isong 8-bit --> max possible value 2^8 = 256                                             |
   |                                                                                                                                            |
    ---------------------------------------------------------------------------------------------------------------------------------------------
    */
    
    // Loop through each pixel varying form  0 to 2400*800 = 1920000
    for (int i = 0; i < img.height * img.step; i++)
    {
		if(img.data[i] == white_pixel)                  	  //Detecting if pixel has the same color as the white_pixel
		{
		 	ball_detected = true;
		    white_pixel_row = i % img.step;               // Dividing the pixel by 2400 
		  	
		  	if (white_pixel_row < img.step / 3)        	// Going to the left 
		  	{   
		  	    lin_x = 0.0; 
		  	    ang_z = 0.5;
		  	    
		  	}
		  	else if (white_pixel_row > img.step * 2 / 3) 	// Going to the Right
		  	{
		  	   
		  	    lin_x = 0.0;
		  	    ang_z = -0.5; 
		  	}
		  	else 
		  	{                                            //Going forwrad
		  	    lin_x = 0.5; 
		  	    ang_z = 0.0; 
		  	 
		  	 }
		  	    break; 
		}
					
        
    }
	drive_robot(lin_x, ang_z);                         // passing the values to drive the robot
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}



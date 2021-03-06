#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;


// This function calls the DriveToTarget service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{


    // TODO: Request a service and pass the velocities to it to drive the robot
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;
	ROS_INFO_STREAM("Sending Data to Robot... linear_x"+ std::to_string(lin_x)+" angular_z "+ std::to_string(ang_z));	
    // Call drive_bot service
    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive_bot");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
   bool ball_found = false;
   int ball_location = 0;
   int i = 0;
   int j =0;
	ROS_INFO_STREAM("Image Processing Started...");
    // Loop through each pixel in the image and check if its equal to the white value
    for (i = 0; i < img.height; i++){
	for (j = 0; j < (img.width); j++)
	 {

    ball_found = (img.data[((j*3)+(i*img.step))] == white_pixel)
        &&(img.data[((j*3)+(i*img.step))+1] == white_pixel)
        &&(img.data[((j*3)+(i*img.step))+2] == white_pixel);



        //ROS_INFO_STREAM("Iter: i,j "+std::to_string(i)+" , "+std::to_string(j));
	//ROS_INFO_STREAM("Current Loop i"+ std::to_string(i)+" j"+ std::to_string(j)+" data "+std::to_string(j+((img.step)*i)));
//ROS_INFO_STREAM(std::to_string(img.data[(j+(i*img.step))]));
//ROS_INFO_STREAM(std::to_string(img.data[(j+(i*img.step))+1]));
//ROS_INFO_STREAM(std::to_string(img.data[(j+(i*img.step))+2]));

        if (ball_found) 
        {
	//if (img.data[i*j] == white_pixel) {
	//ROS_INFO_STREAM("condition: i,j "+std::to_string(img.width)+" "+std::to_string(i)+" , "+std::to_string(j));	
		ball_location = j;
            	break;
        }

    }
	if(ball_found)
		break;
}

	    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

	if (ball_found)
	{	
		//ROS_INFO_STREAM("img/3 "+ std::to_string(img.width/3)+"Location: "+std::to_string(ball_location));
		if (ball_location<(img.width/3)){
		drive_robot(0,0.5);
		ROS_INFO_STREAM("Ball Found Left "+ std::to_string(ball_location));
		}
		else if (ball_location>((img.width/3)*2)){
		drive_robot(0,-0.5);
		ROS_INFO_STREAM("Ball Found Right "+ std::to_string(ball_location));
		}
		else{
		drive_robot(0.2,0);
		ROS_INFO_STREAM("Ball Found Middle "+ std::to_string(ball_location));
		}
	}else{
		ball_location = 0;
		drive_robot(0,0);
		ROS_INFO_STREAM("Ball not Found "+ std::to_string(ball_location));
		}


}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from DriveToTarget
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

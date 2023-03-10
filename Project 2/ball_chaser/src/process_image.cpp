#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    //Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Request to move the robot");
    
    //creating request
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    if (!client.call(srv))
    ROS_ERROR("Failed to call service /ball_chaser/command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    bool found_ball = false;
    int column_index = 0;
    
    for (int i=0; i < img.height * img.step; i += 3)
    {
        if ((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255))
        {
            column_index = i % img.step;
 
            if (column_index < img.step/3)
                drive_robot(0.5, 1);
            else if (column_index < (img.step/3 * 2))
                drive_robot(0.5, 0); 
            else
                drive_robot(0.5, -1);
            found_ball = true;
            break;
         }
     }

     if (found_ball == false)
         drive_robot(0, 0);
 
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

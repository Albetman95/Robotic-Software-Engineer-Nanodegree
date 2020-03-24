//Headers Setting up
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

//Allow to communicate with action to communicate with action that adhere to the MoveBaseAction action interface. 
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

///Including the pick up and drop off areas
double PickUpObject_x = 1.670, PickUpObject_y = 2.84, DropOff_x = -0.206, DropOff_y = 1.576;

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "pick_objects");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //Wait for 5 sec for move_base action to server to come up
  while (!ac.waitForServer(ros::Duration(5.0)))
  {
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  
  //initialize the goal object
  move_base_msgs::MoveBaseGoal goal;

  //setting up parameters 
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  // Request robot to move to Pickup location
  goal.target_pose.pose.position.x = PickUpObject_x;
  goal.target_pose.pose.position.y = PickUpObject_y;
  goal.target_pose.pose.orientation.w = 1.0;

  ROS_INFO("Robot is travelling to the pickup zone");
  //sending values to move_base
  ac.sendGoal(goal);
  //waiting infinite time for the results 
  ac.waitForResult();

  if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {

    ROS_INFO("Robot picked up the virtual object");
    
   
  }
  else
  {
    //First Check in if the robot has moved 
    ROS_INFO("Unable to get to the pickup zone");
  }

     // Wait for 5 seconds
    ros::Duration(5).sleep();

    // Request robot to move to Dropoff location
    goal.target_pose.pose.position.x = DropOff_x;
    goal.target_pose.pose.position.y = DropOff_y;
    goal.target_pose.pose.orientation.w = 1.0;

    ROS_INFO("Robot is travelling to the dropoff zone");
    ac.sendGoal(goal);
    ac.waitForResult();
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {
      // Robot reached dropoff zone
      ROS_INFO("Robot dropped the virtual object");
     
    }
    else
    {
      //Second Check in if the robot has  moved 
      ROS_INFO("Unable to get to the dropoff zone");
    }
  


  return 0;
}

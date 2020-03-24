#include <ros/ros.h>
#include <ros/console.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <visualization_msgs/Marker.h>

double robotPose_x_, robotPose_y_;
double thresh = 0.01;
double pickUpObject_X = 1.670, pickUpObject_Y = 2.84;
double DropOff_X = -0.206, DropOff_Y = 1.576;
int timeToWait = 5;
int currentWait = 0;

void robotCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr &msg_amcl)
{
  // Update robot position
  robotPose_x_ = msg_amcl->pose.pose.position.x;
  robotPose_y_ = msg_amcl->pose.pose.position.y;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "basic_shapes");
  ros::NodeHandle n;
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  int state = 0;

  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;

  visualization_msgs::Marker marker;
  // Set the frame ID and timestamp.  See the TF tutorials for information on these.
  marker.header.frame_id = "map";
  marker.header.stamp = ros::Time::now();

  // Set the namespace and id for this marker.  This serves to create a unique ID
  // Any marker sent with the same namespace and id will overwrite the old one
  marker.ns = "basic_shapes";
  marker.id = 0;

  // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
  marker.type = shape;

  // Set marker orientation
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;

  // Set the scale of the marker -- 1x1x1 here means 1m on a side
  marker.scale.x = 0.3;
  marker.scale.y = 0.3;
  marker.scale.z = 0.3;

  // Set the color -- be sure to set alpha to something non-zero!
  marker.color.r = 1.0f;
  marker.color.g = 0.0f;
  marker.color.b = 0.0f;
  marker.color.a = 1.0;

  marker.pose.position.z = 0;

  marker.lifetime = ros::Duration();

  // Subscribe to /amcl_pose
  ros::Subscriber sub1 = n.subscribe("/amcl_pose", 1000, robotCallback);

  while (ros::ok())
  {

    switch (state)
   {
     case 0: 
        {	
  double calc_pickDistance = abs(robotPose_x_ - pickUpObject_X) + abs(robotPose_y_ - pickUpObject_Y);
	
	  if (calc_pickDistance > thresh)
		{
			marker.action = visualization_msgs::Marker::ADD;
        		marker.pose.position.x = pickUpObject_X;
        		marker.pose.position.y = pickUpObject_Y;
		}

	  else
               {
      		       state = 1;
                       marker.action = visualization_msgs::Marker::DELETE;
               }
			 	break; 
        }
     case 1:
	{
	 if (currentWait < timeToWait)
     	       {
       		      currentWait += 1;
               }
        else
     	       {
                      state = 2;
               }
	                   break;   
        }
     case 2:
       {
      double calc_dropDistance = abs(robotPose_x_ - DropOff_X) + abs(robotPose_y_ - DropOff_Y);

       if (calc_dropDistance > thresh)
               {
      		       marker.action = visualization_msgs::Marker::DELETE;
               }
      else
               {
                       marker.action = visualization_msgs::Marker::ADD;
                       marker.pose.position.x = DropOff_X;
                       marker.pose.position.y = DropOff_Y;
                }
                              
   			break; 
       }
   }

    // Publish the Marker
    marker_pub.publish(marker);

    // Sleep for 1 seconds
    sleep(1);

    // Handle ROS communication events
    ros::spinOnce();
  }
}

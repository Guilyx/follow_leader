//Cpp
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

//ROS
#include "ros/ros.h"

// Include here the ".h" files corresponding to the topic types you use.
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <visualization_msgs/Marker.h>

// You may have a number of globals here.


// Callback functions...

// Our marker message has many fields that remain constant and
// are initialized only once by the following function.

ros::Publisher pubMarker ;
visualization_msgs::Marker marker;

void initializeMarker(){
    // Fetch node name. Markers will be blue if the word "blue" is in the name, red otherwise.
    std::string nodeName ;
    nodeName = ros::this_node::getName();
    // Create a marker for this node. Only timestamp and position will be later updated.
    marker.header.frame_id = "/map";
    marker.ns = nodeName;
    marker.id = 0;
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 0.1;
    marker.scale.y = 0.1;
    marker.scale.z = 0.1;
    marker.color.r = 1.0f;
    marker.color.g = 0.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;
}


// Function to publish a marke at a given (x,y) position.
void publishMarkerAt( geometry_msgs::Point markerPos) {    
    marker.header.stamp = ros::Time::now();
    marker.pose.position.x = markerPos.x ;
    marker.pose.position.y = markerPos.y ;
    marker.lifetime = ros::Duration();
    pubMarker.publish(marker);
}


// On receiving a valid code, kbdCallback modifies the
// speed of the leader.

double speed ;
void kbdCallback( std_msgs::Int16 key_msg ) {
    if( key_msg.data == 43 ){ 
        if( speed < 2.0 ) speed += 0.1 ;    
    }
    if( key_msg.data == 45 ){
        if( speed > 0.5 ) speed -= 0.1 ;
    }
}


int main (int argc, char** argv)
{

    //ROS Initialization
    ros::init(argc, argv, "leader");

    // Define your node handles
    ros::NodeHandle nh_loc("~") ;
    ros::NodeHandle nh_glob ;

    // Declare your node's subscriptions and service clients
    ros::Subscriber kbdSub = nh_glob.subscribe<std_msgs::Int16>("/key_typed",1,kbdCallback) ;

    // Declare you publishers and service servers
    pubMarker = nh_glob.advertise<visualization_msgs::Marker>("/visualization_marker",1) ;
    ros::Publisher pubPos = nh_loc.advertise<geometry_msgs::Point>("leader_pos", 1);
    ros::Publisher pubSpeed = nh_loc.advertise<std_msgs::Float64>("leader_speed", 1);

    std_msgs::Float64 speedMsg;
    geometry_msgs::Point leaderPosition ;
    leaderPosition.x = 0.0 ;
    leaderPosition.y = 0.0 ;
    leaderPosition.z = 0.0 ;

    speed             = 3.0 ;
    double radius     = 3.0 ;
    double polarAngle = 0.0 ;

    initializeMarker() ;
    publishMarkerAt( leaderPosition ) ;

    ros::Rate rate(50);  
    ros::Time currentTime, prevTime = ros::Time::now() ; 
    while (ros::ok()){
	ros::spinOnce();

        // Calculate new position of the leader. The leader moves 
        // along a circle of a given radius (see above for the radius).
        currentTime = ros::Time::now() ;
        ros::Duration timeElapsed = currentTime - prevTime ;
        prevTime = currentTime ;
        // w = speed/radius. Polar angle increases by DeltaT * w at each period.
        polarAngle += (speed/radius)*timeElapsed.toSec() ;
        leaderPosition.x = radius*cos(polarAngle) ;
        leaderPosition.y = radius*sin(polarAngle) ;

        // Publish a marke at the position of the leader.
        speedMsg.data = speed;
        pubPos.publish(leaderPosition);
        pubSpeed.publish(speedMsg);
        publishMarkerAt( leaderPosition ) ;

	rate.sleep();
    }
}


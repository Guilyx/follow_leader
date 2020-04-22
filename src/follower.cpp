/**
 * \file
 * \brief 
 * \author 
 * \version 0.1
 * \date 
 * 
 * \param[in] 
 * 
 * Subscribes to: <BR>
 *    ° 
 * 
 * Publishes to: <BR>
 *    ° 
 *
 * Description
 *
 */


//Cpp
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

//ROS
#include "ros/ros.h"

// Include here the ".h" files corresponding to the topic type you use.
// ...
#include <geometry_msgs/Point.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <visualization_msgs/Marker.h>

// You may have a number of globals here.
//...
ros::Publisher pubMarker ;
visualization_msgs::Marker marker;
geometry_msgs::Point currentLeaderPosition;
std_msgs::Float64 currentLeaderSpeed;

// utilitary functions

void initializeMarker(){
    // Fetch node name. Markers will be blue if the word "blue" is in the name, red otherwise.
    std::string nodeName ;
    nodeName = ros::this_node::getName();
    // Create a marker for this node. Only timestamp and position will be later updated.
    marker.header.frame_id = "/map";
    marker.ns = nodeName;
    marker.id = 0;
    marker.type = visualization_msgs::Marker::CUBE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 0.1;
    marker.scale.y = 0.1;
    marker.scale.z = 0.1;
    marker.color.r = 0.0f;
    marker.color.g = 0.0f;
    marker.color.b = 1.0f;
    marker.color.a = 1.0;
}

void publishMarkerAt(geometry_msgs::Point markerPos) {    
    marker.header.stamp = ros::Time::now();
    marker.pose.position.x = markerPos.x ;
    marker.pose.position.y = markerPos.y ;
    marker.lifetime = ros::Duration();
    pubMarker.publish(marker);
}


// Callback functions...
void leaderPositionCallback(geometry_msgs::Point receivedLeaderPosition){
    // ... Callback function code
    currentLeaderPosition.x = receivedLeaderPosition.x;
    currentLeaderPosition.y = receivedLeaderPosition.y;
    currentLeaderPosition.z = receivedLeaderPosition.z;
}

void leaderSpeedCallback(std_msgs::Float64 receivedLeaderSpeed) {
    currentLeaderSpeed.data = receivedLeaderSpeed.data;
}


int main (int argc, char** argv)
{

    //ROS Initialization
    ros::init(argc, argv, "default_node_name_here");

    // Define your node handles
    ros::NodeHandle nh_loc("~") ;
    ros::NodeHandle nh_glob ;

    // Read the node parameters if any
    double init_pos_x;
    double init_pos_y;
    nh_loc.param("init_pos_x", init_pos_x, 0.0);
    nh_loc.param("init_pos_y", init_pos_y, 0.0);

    // Declare your node's subscriptions and service clients
    // ...
    ros::Subscriber subLeaderPos = nh_glob.subscribe<geometry_msgs::Point>("leader_pos", 1, leaderPositionCallback);
    ros::Subscriber subLeaderSpeed = nh_glob.subscribe<std_msgs::Float64>("leader_speed", 1, leaderSpeedCallback);

    // Declare you publishers and service servers
    // ...
    ros::Publisher pubPos = nh_loc.advertise<geometry_msgs::Point>("follower_pos", 1);
    ros::Publisher pubSpeed = nh_loc.advertise<std_msgs::Float64>("follower_speed", 1);
    pubMarker = nh_glob.advertise<visualization_msgs::Marker>("/visualization_marker",1) ;

    // Initialization of main code
    geometry_msgs::Point pos;
    geometry_msgs::Point direction;
    std_msgs::Float64 speed_msg;
    ros::Time currentTime, prevTime = ros::Time::now(); 
    initializeMarker();

    // Initialize follower at parameter position
    pos.x = init_pos_x;
    pos.y = init_pos_y;
    pos.z = 0.0;
    publishMarkerAt(pos); 

    // The follower doesn't move at startup
    double speed = 0.0;

    // The follower has no goal at startup
    geometry_msgs::Point goal = pos;

    ros::Rate rate(50);   // Or other rate.
    while (ros::ok()){
        ros::spinOnce();

        // Calculate time elapsed since last action
        currentTime = ros::Time::now();
        ros::Duration timeElapsed = currentTime - prevTime ;
        prevTime = currentTime ;

        // Get speed
        speed = abs(currentLeaderSpeed.data);
        speed_msg.data = speed;

        // Direction of motion
        float dx = currentLeaderPosition.x - pos.x;
        float dy = currentLeaderPosition.y - pos.y;
        direction.x = speed * timeElapsed.toSec() * (dx) / sqrt(pow(dx, 2) + (dy, 2));
        direction.y = speed * timeElapsed.toSec() * (dy) / sqrt(pow(dx, 2) + (dy, 2));
        direction.z = 0.0;

        // Where should follower go
        pos.x += direction.x;
        pos.y += direction.y;
        pos.z += direction.z;
        
        pubPos.publish(pos);
        pubSpeed.publish(speed_msg);
        publishMarkerAt(pos);

        rate.sleep();
    }
}

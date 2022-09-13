#include "ros/ros.h"
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/CompressedImage.h>
#include <opencv2/opencv.hpp>
#include <iostream>

void warning_message() {
    ROS_WARN("The compressed image messages will be replaced with raw image messages in the output bagfile.");
    // ROS_WARN("Warning! The input bagfile will be modified. Press 'y' to continue and 'n' to exit.");
    // char cmd;
    // std::cin >> cmd;
    // if(cmd != 'y') {
    //     ROS_WARN("Exiting.");
    //     exit(1);
    // }
}

void animation() {
    static int i = 0;
    if(i==0) {
        std::cout << "\b\\" << std::flush;
    } 
    else if(i==1) {
        std::cout << "\b|" << std::flush;
    } 
    else if(i==2) {
        std::cout << "\b/" << std::flush;
    } 
    else {
        std::cout << "\b-" << std::flush;
        i=0;
        return;
    }
    i++;
    return;
}

//decode images and copy to bagile
void decode_and_create_bagfile(std::string bagfile_in, std::string bagfile_out, std::string rgb_topic) {
    rosbag::Bag bag_read;
    bag_read.open(bagfile_in, rosbag::bagmode::Read);

    rosbag::Bag bag_write;
    bag_write.open(bagfile_out, rosbag::bagmode::Write);

    std::string rgb_topic_compressed = rgb_topic + "/compressed";
    ROS_INFO("Using %s as compressed image topic.", rgb_topic_compressed.c_str());

    ROS_INFO("Creating bagfile with decoded images...");
    rosbag::View view(bag_read);
    for(rosbag::MessageInstance const m : view) {
        std::string topic = m.getTopic();
        if (topic == rgb_topic_compressed) {
            try {
                sensor_msgs::CompressedImageConstPtr img_comp = m.instantiate<sensor_msgs::CompressedImage>();
                cv::Mat img = cv::imdecode(cv::Mat(img_comp->data),1);
                sensor_msgs::ImagePtr img_decomp = cv_bridge::CvImage(img_comp->header, "bgr8", img).toImageMsg();
                bag_write.write(rgb_topic, img_comp->header.stamp, *img_decomp);
            }
            catch (cv_bridge::Exception& e) {
                ROS_ERROR("Error copying image from bagfile!");
            }
        }
        else {
            bag_write.write(m.getTopic(), m.getTime(), m); //writes message without needing type
        }
        animation();
    }

    bag_read.close();
    bag_write.close();

    return;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "decompress_and_create_bagfile");
    ros::NodeHandle nh;

    warning_message();

    std::string bagfile_in;
    if(!ros::param::get("~bagfile_in", bagfile_in)) {
        ROS_ERROR("Must define bagfile_in param!");
        exit(1);
    }

    std::string bagfile_out;
    if(!ros::param::get("~bagfile_out", bagfile_out)) {
        ROS_ERROR("Must define bagfile_out param!");
        exit(1);
    }

    std::string rgb_topic;
    if(!ros::param::get("~rgb_topic", rgb_topic)) {
        ROS_ERROR("Must define rgb_topic param!");
        exit(1);
    }

    ROS_INFO("bagfile_in: %s", bagfile_in.c_str());
    ROS_INFO("bagfile_out: %s", bagfile_out.c_str());
    ROS_INFO("image topic: %s", rgb_topic.c_str());

    decode_and_create_bagfile(bagfile_in, bagfile_out, rgb_topic);
  
  return 0;
}
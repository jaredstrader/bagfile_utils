# bagfile_utils
Utilities for ROS bagfiles

## Usage

To create a bagfile with compressed images replaced with raw images:
```
roslaunch rosbag_utils decompress.launch bagfile_in:='path/to/bagfile' rgb_topic:=/camera/image_raw
```
The output bagfile will be created in `/bagfile_utils/bags/output.bag`.
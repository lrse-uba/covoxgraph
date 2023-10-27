# covoxgraph


## Install

covoxgraph extends voxgraph and voxblox and runs on [ROS](https://www.ros.org/). Therefore start by installing and configuring these as described in the [voxblox installation instructions](https://voxblox.readthedocs.io/en/latest/pages/Installation.html) and the [voxgraph installation instructions](https://github.com/ethz-asl/voxgraph#install)

Next, clone this repository and its dependencies into the same catkin workspace.

```shell script
cd ~/catkin_ws/src/
git clone https://github.com/lrse-uba/covoxgraph.git

wstool merge ./voxgraph/voxgraph_https.rosinstall
wstool update
```
## Install CUDA Toolkit
### Installation Steps
1. Go to https://developer.nvidia.com/cuda-toolkit-archive and choose your desire CUDA toolkit version that is compatible with the hardware you are using.
2. Select your OS.
3. Select your system architecture.
4. Select your OS version.
5. Select Installer Type and Follow the steps provided. (.exe on Windows and .run or .deb on Linux)

## Run

#### Demo
Voxgraph provides a demo dataset [here](http://robotics.ethz.ch/~asl-datasets/2020_voxgraph_arche), which features a hexacopter flying through an indoor-outdoor search and rescue training site. The rosbag includes the robot's visual, inertial and LiDAR sensor data, and the GPS RTK measurements used for the evaluations in their paper.

Running this example requires a couple more packages than the basic install, for performing lidar undistortion. Clone and build these using:

```shell script
cd ~/catkin_ws/src/
wstool merge ./voxgraph/arche_https.rosinstall
wstool update
catkin build lidar_undistortion
```

Download and unzip the dataset
```shell script
wget -P ~/Downloads http://robotics.ethz.ch/~asl-datasets/2020_voxgraph_arche/arche_flight1_2ms_indoor-outdoor-figure-8.zip
unzip ~/Downloads/arche_flight1_2ms_indoor-outdoor-figure-8.zip
```
Launch the example
```shell script
roslaunch voxgraph arche_demo.launch rosbag_path:=${HOME}/Downloads/arche_flight1_2ms_indoor-outdoor-figure-8.bag
```
If everything has worked correctly, this should open rviz and within a few seconds the first fragment of the map should appear.


## Results
<img alt="Trajectories" src= "https://github.com/lrse-uba/covoxgraph/blob/main/imgs/trajectories.png">

## Requeriments

1. CUDA 12
2. GCC 11 or later
3. ROS Melodic or Noetic




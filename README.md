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

## Requeriments

1. CUDA 12
2. GCC 11 or later
3. ROS Melodic or Noetic



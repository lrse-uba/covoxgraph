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
## Requeriments

1. CUDA 12
2. GCC 11 or later
3. ROS Melodic or Noetic
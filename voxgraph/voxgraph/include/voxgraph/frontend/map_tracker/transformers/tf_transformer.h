#ifndef VOXGRAPH_FRONTEND_MAP_TRACKER_TRANSFORMERS_TF_TRANSFORMER_H_
#define VOXGRAPH_FRONTEND_MAP_TRACKER_TRANSFORMERS_TF_TRANSFORMER_H_

#include <map>
#include <string>

#include <minkindr_conversions/kindr_msg.h>
#include <tf2_ros/transform_listener.h>

#include "voxgraph/common.h"

namespace voxgraph {
class TfTransformer {
 public:
  TfTransformer()
      : tf_buffer_(ros::Duration(10.0)),
        tf_listener_(tf_buffer_),
        transform_lookup_retry_period_(0.02),
        transform_lookup_max_time_(0.25) {}

  // Method that waits for a transform to become available, while doing less
  // agressive polling that ROS's standard tf2_ros::Buffer::canTransform(...)
  bool waitForTransform(const std::string& to_frame_id,
                        const std::string& from_frame_id,
                        const ros::Time& frame_timestamp);

  // Method to lookup transforms and convert them Kindr
  bool lookupTransform(const std::string& to_frame_id,
                       const std::string& from_frame_id,
                       const ros::Time& frame_timestamp,
                       Transformation* transform);

 private:
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  // Transform lookup timers
  // Timeout between each update attempt
  const ros::WallDuration transform_lookup_retry_period_;
  // Maximum time to wait before giving up
  const ros::WallDuration transform_lookup_max_time_;
};
}  // namespace voxgraph

#endif  // VOXGRAPH_FRONTEND_MAP_TRACKER_TRANSFORMERS_TF_TRANSFORMER_H_

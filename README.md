# px4ctrl

一个简单的 vision_to_mavros 功能包
主要功能：
（1）将 VINS-Fusion 的 body 坐标系在 world 坐标系下为位姿转化为 base_link 在 map 坐标系中的位姿；
（2）将转化后的位姿信息以话题 /mavros/vision_pose/pose 发布。

Please follow the steps here:

1.Put the folder of source code into {your_catkin_workspace}/src
2.cd {your_catkin_workspace}
3.catkin_make
4.source {your_catkin_workspace}/devel/setup.bash

5.run Laserscan node: 
    roslaunch lakibeam1 lakibeam1_scan.launch
  run Laserscan node with rviz view: 
    roslaunch lakibeam1 lakibeam1_scan_view.launch
  run PointCloud2 node:
    roslaunch lakibeam1 lakibeam1_pcd.launch
  run PointCloud2 node with rviz view:
    roslaunch lakibeam1 lakibeam1_pcd_view.launch
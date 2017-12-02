# Unscented-Kalman-Filter
In this project, I used C++ to write a program that processes Lidar data to track an object via the a Particle Filter. 

## Project Info
For a great paper on Particle Filters, [click here](http://robots.stanford.edu/papers/thrun.pf-in-robotics-uai02.pdf)

To see the implementation please visit the following three files in the 'src' folder:

1. particle_filter.cpp

## Setup instructions
FYI, I ran my code on a Macbook Pro. Please ensure you have downloaded the Udacity SDCND Simulator [here](https://github.com/udacity/self-driving-car-sim/releases/) and have installed cmake (3.5), make (4.1), and gcc/gcc+ (5.4).

1. Open Terminal
2. `git clone https://github.com/tlapinsk/CarND-Kidnapped-Vehicle-Project.git`
3. `cd CarND-Kidnapped-Vehicle-Project`
4. `sh install-mac.sh`
5. `mkdir build && cd build`
6. `cmake`
7. `make`
8. `./particle_filter`
9. Run the term2_sim application, select Project 2, and click 'Start'

## Results
My Particle Filter produced the below results.

|  Input    |    MSE   |
|  -----    |  ------- |
|  Error X  |  0.146   |
|  Error Y  |  0.120   |
|  Yaw      |  0.004   |


![Visualization](https://github.com/tlapinsk/CarND-Unscented-Kalman-Filter-Project/blob/master/output/results.png?raw=true "Visualization")

## Resources
Shoutout to the tutorials provided by Udacity and Mercedes on Particle Filters. Below are further resources and helpful links that I used to complete this project:

- [Tracking a self-driving car with high precision](https://towardsdatascience.com/helping-a-self-driving-car-localize-itself-88705f419e4a)
- [Udacity Self-Driving Car Nanodegree Project 8 - Kidnapped Vehicle](http://jeremyshannon.com/2017/06/02/udacity-sdcnd-kidnapped-vehicle.html)
- [RMSE](https://discussions.udacity.com/t/rmse-is-ok-but-not-great/465374/4)
- [updateWeights](https://discussions.udacity.com/t/updateweights-equation-clarification/369534/12)
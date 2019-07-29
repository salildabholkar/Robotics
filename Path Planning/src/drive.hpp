#ifndef behavior_hpp
#define behavior_hpp

#include <vector>
#include <string>

using namespace std;

class Drive {
public:
    int curr_lane;
    double curr_lead_vehicle_speed = 22.352 - 0.5;
    double target_vehicle_speed;
    vector<double> avg_scores = {0,0,0};
    
    // Returns amount of meters left or right to move
    int nextMove(double s, double d, vector<vector<double>> sensor_fusion);
    
    // Calculates if d value corresponds to left, right, or center lane
    int nextLane(double d);
    
    // Calculates the closest vehicle either in front or behind the car in a given lane
    // Returns distance and speed of that vehicle
    vector<double> closestVehicle(double s, int lane, vector<vector<double>> sensor_fusion, bool direction);
    
    // Scores each lane
    // Returns the lane with the best score (0 left, 1 middle, 2 right)
    int scoreLane(double s, int lane, vector<vector<double>> sensor_fusion);
};

#endif
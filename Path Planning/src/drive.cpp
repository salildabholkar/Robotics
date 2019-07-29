#include "drive.hpp"

int Drive::nextMove(double s, double d, vector<vector<double>> sensor_fusion) {
    int lane = nextLane(d);
    int new_lane;
    double distance = closestVehicle(s, lane, sensor_fusion, true)[0];
    
    curr_lane = lane;
    
    if (distance > 20) { // if large space
        new_lane = lane;
        target_vehicle_speed = 22.352 - 0.5;
        avg_scores = {0,0,0}; // Reset
        return 0;
    } else {
        new_lane = scoreLane(s, lane, sensor_fusion);
        vector <double> vehicle = closestVehicle(s, new_lane, sensor_fusion, true);
        target_vehicle_speed = vehicle[1];
    }
    
    // Space between each lane is four meters
    if (new_lane == lane) {
        return 0;
    } else if (new_lane < lane) {
        return -4;
    } else {
        return 4;
    }
}

int Drive::nextLane(double d) {
    // Left is 0, middle is 1, right is 2
    int lane;
    if (d < 4) {
        lane = 0;
    } else if (d < 8) {
        lane = 1;
    } else {
        lane = 2;
    }
    return lane;
}

vector<double> Drive::closestVehicle(double s, int lane, vector<vector<double>> sensor_fusion, bool direction) {
    double dist = 10000;
    double velocity = 22.352 - 0.5; // no cars
    double vehicle_s;
    double vehicle_d;
    double vehicle_v;
    int vehicle_lane;
    
    // Check each vehicle in sensor range
    for(int vehicle = 0; vehicle < sensor_fusion.size(); vehicle++) {
        vehicle_s = sensor_fusion[vehicle][5];
        vehicle_d = sensor_fusion[vehicle][6];
        vehicle_v = sqrt(pow(sensor_fusion[vehicle][3], 2)+pow(sensor_fusion[vehicle][4], 2));
        vehicle_lane = nextLane(vehicle_d);
        
        if (vehicle_lane == lane) { // if same lane
            if (direction) {
                if (vehicle_s > s and (vehicle_s - s) < dist) { // ahead of our vehicle
                    dist = vehicle_s - s;
                    velocity = vehicle_v;
                }
            } else {
                if (s >= vehicle_s and (s - vehicle_s) < dist) { // behind
                    dist = s - vehicle_s;
                    velocity = vehicle_v;
                }
            }
        }
    }
    if (dist <= 0) {
        dist = 1.0;
    }
    if (lane == curr_lane and direction) {
        curr_lead_vehicle_speed = velocity;
    }
    return {dist, velocity};
}

int Drive::scoreLane(double s, int lane, vector<vector<double>> sensor_fusion) {
    vector <double> scores = {0,0,0};
    vector <double> front_vehicle;
    vector <double> back_vehicle;
    
    for (int i = 0; i < 3; i++) {
        if (i == lane) {  // prefer not changing
            scores[i] += 0.5;
        }

        front_vehicle = closestVehicle(s, i, sensor_fusion, true);
        back_vehicle = closestVehicle(s, i, sensor_fusion, false);

        if (front_vehicle[0] > 1000 and back_vehicle[0] > 1000) {
            scores[i] += 5; // if wide open lane, move into that lane
        } else {
            if (front_vehicle[0] < 10) {
                scores[i] -= 5; // not if front car too close
            }
            if (back_vehicle[0] < 10) {
                scores[i] -= 5; // also not if back car too close
            }

            // Positives
            scores[i] += 1 - (10/(front_vehicle[0]/3)); // large open distance in lane in front
            scores[i] += 1 - (10/(back_vehicle[0]/3)); // large open distance in lane in back
            scores[i] += 1 - (10/(front_vehicle[1]/2)); // faster car in lane in front
            scores[i] += 1 / (back_vehicle[1]/2); // slower car in lane in back
        }

        // Avg Sample from last ten iterations
        avg_scores[i] = (avg_scores[i] * 10) - avg_scores[i];
        avg_scores[i] += scores[i];
        avg_scores[i] /= 10;
    }
    
    // Only compare applicable lanes
    if (lane == 0) {
        return max_element(avg_scores.begin(), avg_scores.end() - 1) - avg_scores.begin();
    } else if (lane == 1) {
        return max_element(avg_scores.begin(), avg_scores.end())  - avg_scores.begin();
    } else {
        return max_element(avg_scores.begin() + 1, avg_scores.end())  - avg_scores.begin();
    }
}

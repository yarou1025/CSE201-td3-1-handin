#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
  double *new_array;
  new_array = new double [new_size];
  for(int i = 0; i < new_size; i++){
      if(i < length) new_array[i] = array[i];
      else new_array[i] = 0;
  }
  delete [] array;
  return new_array; // YOU CAN CHANGE THIS
}

double* shrink_array(double* array, int length, int new_size) {
    double *new_array;
    new_array = new double [new_size];
    for(int i = 0; i < new_size; i++){
        new_array[i] = array[i];
    }
    delete [] array;
    return new_array; // YOU CAN CHANGE THIS
}

double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {
  if(max_size == current_size){
      array = extend_array(array, current_size, max_size + 5);
      max_size += 5;
  }
      array[current_size] = element;
      current_size += 1;

  return array;
}

double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
    //if(max_size - current_size >= 5) {
    if(max_size - current_size >= 4 && max_size - 5 >= 0) {
        array = shrink_array(array, max_size, max_size - 5);
        max_size = max_size - 5;
    }

    if(current_size > 0) current_size = current_size - 1;

    return array;
}

bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (! hit_target) && (! hit_obstacle)) {
    telemetry = append_to_array(t, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(x, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(y, telemetry, telemetry_current_size, telemetry_max_size);
    double * target_coordinates = find_collision(x, y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {
      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }
  }

  return hit_target;
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &telemetry,
                     int &telemetry_current_size,
                     int &telemetry_max_size) {
    //std::cout<<tot_telemetries<<"\n";
    //std::cout<<telemetry_current_size<<"\n";
    //std::cout<<telemetry_max_size<<"\n";

    for(int i = 0; i < tot_telemetries; i++){
        //std::cout<<"("<<i<<")";
        for(int j = 0; j < telemetries_sizes[i]/3; j++){
            //std::cout<<"("<<i<<","<<j<<") t:"<<telemetries[i][3*j]<<",x:"<<telemetries[i][3*j+1]<<",y:"<<telemetries[i][3*j+2]<<"\n";
            //if(j == 0) min_t = telemetries[i][j];
            //else min_t = min_t < telemetries[i][j] ? min_t : telemetries[i][j];
            telemetry = append_to_array(telemetries[i][3*j], telemetry, telemetry_current_size, telemetry_max_size);
            telemetry = append_to_array(telemetries[i][3*j+1], telemetry, telemetry_current_size, telemetry_max_size);
            telemetry = append_to_array(telemetries[i][3*j+2], telemetry, telemetry_current_size, telemetry_max_size);
        }
        //std::cout<<"\n";
    }

    //sort
    for(int i = 0; i < telemetry_current_size/3 - 1; i++){
        for(int j = i; j < telemetry_current_size/3 - 1; j++){
            if(telemetry[3*j] > telemetry[3*(j+1)]){
                double tmp = telemetry[3*j];
                telemetry[3*j] = telemetry[3*(j+1)];
                telemetry[3*(j+1)] = tmp;

                tmp = telemetry[3*j+1];
                telemetry[3*j+1] = telemetry[3*(j+1)+1];
                telemetry[3*(j+1)+1] = tmp;
                tmp = telemetry[3*j+2];

                telemetry[3*j+2] = telemetry[3*(j+1)+2];
                telemetry[3*(j+1)+2] = tmp;
            }
        }
    }
}

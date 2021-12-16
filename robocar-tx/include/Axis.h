#ifndef AXIS_H
#define AXIS_H

#include <Arduino.h>
#include "tx.h"

class Axis {
 public:
  Axis(int, int, int, int);

  int getOuput();
  void calibrate();
  void startCalibration();
  void applyCalibration();

 private:
  float applyExp(float, float);
  float normInput(int, int, int, float, float);
  int normOutput(float, float, float, int, int);

  const int in_pin_;
  const int trm_pin_;
  const int rev_pin_;
  const int exp_pin_;
  int min_in_ = MIN_IN;
  int max_in_ = MAX_IN;
  int min_trm_ = MIN_IN;
  int max_trm_= MAX_IN;
  int min_exp_ = MIN_IN;
  int max_exp_= MAX_IN;
  int min_in_cal_;
  int max_in_cal_;
  int min_trm_cal_;
  int max_trm_cal_;
  int min_exp_cal_;
  int max_exp_cal_;
};

#endif
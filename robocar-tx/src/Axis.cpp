#include "Axis.h"

Axis::Axis(int in_pin, int trim_pin, int exp_pin, int rev_pin) 
    : in_pin_(in_pin), 
      trm_pin_(trim_pin), 
      exp_pin_(exp_pin),
      rev_pin_(rev_pin)
      {
}

void Axis::startCalibration() {
  min_in_cal_ = MAX_IN;
  max_in_cal_ = MIN_IN;
  min_trm_cal_ = MAX_IN;
  max_trm_cal_ = MIN_IN;
  min_exp_cal_ = MAX_IN;
  max_exp_cal_ = MIN_IN;
}

void Axis::applyCalibration() {
  min_in_ = min_in_cal_;
  max_in_ = max_in_cal_;
  min_trm_ = min_trm_cal_;
  max_trm_ = max_trm_cal_;
  min_exp_ = min_exp_cal_;
  max_exp_ = max_exp_cal_;
}

void Axis::calibrate() {
  int raw_in = analogRead(in_pin_);
  if (raw_in > max_in_cal_) {
    max_in_cal_ = raw_in;
  }
  if (raw_in < min_in_cal_) {
    min_in_cal_ = raw_in;
  }
  int raw_trm = analogRead(trm_pin_);
  if (raw_trm > max_trm_cal_) {
    max_trm_cal_ = raw_trm;
  }
  if (raw_trm < min_trm_cal_) {
    min_trm_cal_ = raw_trm;
  }
  int raw_exp = analogRead(exp_pin_);
  if (raw_exp > max_exp_cal_) {
    max_exp_cal_ = raw_exp;
  }
  if (raw_exp < min_exp_cal_) {
    min_exp_cal_ = raw_exp;
  }
  
}

float Axis::normInput(int x, int in_min=0, int in_max=1023, 
                      float out_min=-1.0, float out_max=1.0) {
  return float(x - in_min) * (out_max - out_min) / float(in_max - in_min) + out_min;
}

int Axis::normOutput(float x, float in_min=-1.0, float in_max=1.0, 
                     int out_min=-127, int out_max=127) {
  return (x - in_min) * (out_max - out_min) / float(in_max - in_min) + out_min;
}

  // Return byte to send to nano
int Axis::getOuput() {
  // read pins
  int raw_in = analogRead(in_pin_);
  int raw_trm = analogRead(trm_pin_);
  int raw_exp = analogRead(exp_pin_);
  int rev = digitalRead(rev_pin_);
  // nrom appropriately
  float normed_in = normInput(raw_in, min_in_, max_in_); 
  float normed_trm = normInput(raw_trm, min_trm_, max_trm_);
  float normed_exp = normInput(raw_exp, min_exp_, max_exp_, 0, 1); // instead of -1.0, 1.0
  // apply trim
  float trimmed_in = normed_in + normed_trm;
  // apply exp
  // float output = applyExp(trimmed_in, normed_exp);

  float output = trimmed_in; // FOR NOW

  // convert to output format
  int out_int = normOutput(output);
  // reverse if needed
  if (rev == LOW) {
    out_int *= -1;
  }  
  return out_int;
}

// rcgroups.com/forums/showthread.php?1675540-who-know-the-algorithm-for-exponential-curve-in-RC
// Where 0 <= a <= 1
float Axis::applyExp(float x, float a) {
  return a * pow(x, 3) + (1.0 - a) * x;
}
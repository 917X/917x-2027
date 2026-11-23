#pragma once
#include "EZ-Template/drive/drive.hpp"
#include "pros/distance.hpp"

// Distance sensor position reset.  left() / right() / front() / back() each give
// 72 minus how far the center of the robot is from the wall that sensor points at:
//
//     72 - (reading + offset) * cos(tilt)
//
// reading is the sensor in inches, offset is how far the sensor sits from the center
// of rotation, and tilt is how far the robot is turned off the wall (odom theta,
// folded to between -45 and 45).  At 0 / 90 / 180 / 270 the cos is 1 and it's just
// 72 - (reading + offset).
//
// The number is always the size, never the sign.  Which axis it goes in (x or y) and
// whether it's + or - depends on which wall the sensor is facing, so you pick that.
//
// If a sensor is nullptr (back has no port yet), isn't plugged in, or can't see a wall,
// you get back `fallback` instead.  Pass in what the value already is and it stays as is:
//
//     // facing 0, front toward the far wall, left side toward the left wall
//     chassis.odom_y_set(distReset.front(chassis.odom_y_get()));
//     chassis.odom_x_set(-distReset.left(-chassis.odom_x_get()));
class DistReset {
 public:
  DistReset(ez::Drive& chassis,
            pros::Distance* leftSensor, double leftOffset,
            pros::Distance* rightSensor, double rightOffset,
            pros::Distance* frontSensor, double frontOffset,
            pros::Distance* backSensor, double backOffset);

  double left(double fallback);
  double right(double fallback);
  double front(double fallback);
  double back(double fallback);

  ez::Drive& chassis;  // odom theta is the robot angle used for the tilt

  pros::Distance* leftSensor;  // nullptr if there isn't one
  pros::Distance* rightSensor;
  pros::Distance* frontSensor;
  pros::Distance* backSensor;

  double leftOffset;  // inches from the center of rotation to the sensor
  double rightOffset;
  double frontOffset;
  double backOffset;

 private:
  double read(pros::Distance* sensor, double offset, double fallback);
};

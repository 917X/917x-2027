#include "subsystem/dist_reset.hpp"

#include <cmath>
#include <cstdint>

#include "pros/error.h"

namespace {
constexpr double FIELD_HALF = 72.0;  // center of the field to the wall, inches
constexpr double MM_PER_IN = 25.4;
constexpr double DEG_TO_RAD = 3.14159265358979323846 / 180.0;
constexpr std::int32_t NO_OBJECT = 9999;  // what get() gives when it can't see anything
}  // namespace

DistReset::DistReset(ez::Drive& chassis,
                     pros::Distance* leftSensor, double leftOffset,
                     pros::Distance* rightSensor, double rightOffset,
                     pros::Distance* frontSensor, double frontOffset,
                     pros::Distance* backSensor, double backOffset)
    : chassis(chassis),
      leftSensor(leftSensor),
      rightSensor(rightSensor),
      frontSensor(frontSensor),
      backSensor(backSensor),
      leftOffset(leftOffset),
      rightOffset(rightOffset),
      frontOffset(frontOffset),
      backOffset(backOffset) {}

double DistReset::left(double fallback) { return read(leftSensor, leftOffset, fallback); }
double DistReset::right(double fallback) { return read(rightSensor, rightOffset, fallback); }
double DistReset::front(double fallback) { return read(frontSensor, frontOffset, fallback); }
double DistReset::back(double fallback) { return read(backSensor, backOffset, fallback); }

double DistReset::read(pros::Distance* sensor, double offset, double fallback) {
  if (sensor == nullptr || !sensor->is_installed()) return fallback;

  std::int32_t mm = sensor->get();
  if (mm == PROS_ERR || mm <= 0 || mm >= NO_OBJECT) return fallback;

  // How far the robot is turned off square with the walls, between -45 and 45.
  // The sensors all point 0 / 90 / 180 / 270 off the front, so this is the same tilt
  // for every one of them.
  double tilt = std::remainder(chassis.odom_theta_get(), 90.0);

  double toWall = (mm / MM_PER_IN + offset) * std::cos(tilt * DEG_TO_RAD);
  return FIELD_HALF - toWall;
}

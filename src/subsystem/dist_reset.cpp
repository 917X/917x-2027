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
                     pros::Distance* backSensor, double backOffset,
                     double backEdgeOffset)
    : chassis(chassis),
      leftSensor(leftSensor),
      rightSensor(rightSensor),
      frontSensor(frontSensor),
      backSensor(backSensor),
      leftOffset(leftOffset),
      rightOffset(rightOffset),
      frontOffset(frontOffset),
      backOffset(backOffset),
      backEdgeOffset(backEdgeOffset) {}

double DistReset::left(double fallback) { return read(leftSensor, leftOffset, fallback); }
double DistReset::right(double fallback) { return read(rightSensor, rightOffset, fallback); }
double DistReset::front(double fallback) { return read(frontSensor, frontOffset, fallback); }
double DistReset::back(double fallback) { return read(backSensor, backOffset, fallback); }

double DistReset::read(pros::Distance* sensor, double offset, double fallback) {
  double dist;
  if (!toWall(sensor, offset, dist)) return fallback;
  return FIELD_HALF - dist;
}

bool DistReset::toWall(pros::Distance* sensor, double offset, double& out) {
  if (sensor == nullptr || !sensor->is_installed()) return false;

  std::int32_t mm = sensor->get();
  if (mm == PROS_ERR || mm <= 0 || mm >= NO_OBJECT) return false;

  // How far the robot is turned off square with the walls, between -45 and 45.
  // The sensors all point 0 / 90 / 180 / 270 off the front, so this is the same tilt
  // for every one of them.
  double tilt = std::remainder(chassis.odom_theta_get(), 90.0);

  out = (mm / MM_PER_IN + offset) * std::cos(tilt * DEG_TO_RAD);
  return true;
}

DistReset::Result DistReset::reset(double maxCorrection) {
  double x = chassis.odom_x_get();
  double y = chassis.odom_y_get();
  double theta = chassis.odom_theta_get();

  // Walls numbered by the heading that points at them: 0 = +y, 1 = +x, 2 = -y, 3 = -x.
  // The quadrant picks the two close ones.
  int nearX = x >= 0 ? 1 : 3;
  int nearY = y >= 0 ? 0 : 2;

  struct Side {
    pros::Distance* sensor;
    double offset;
    double mount;  // degrees the sensor points, clockwise off the front of the robot
  };
  const Side sides[] = {
      {frontSensor, frontOffset, 0.0},
      {rightSensor, rightOffset, 90.0},
      {backSensor, backOffset, 180.0},
      {leftSensor, leftOffset, 270.0},
  };

  Result result;
  double newX = x;
  double newY = y;

  for (const Side& side : sides) {
    // Which wall this sensor is pointed at right now, snapped to the closest of the 4.
    int wall = static_cast<int>(std::lround((theta + side.mount) / 90.0)) % 4;
    if (wall < 0) wall += 4;
    if (wall != nearX && wall != nearY) continue;  // pointed at a far wall

    double dist;
    if (!toWall(side.sensor, side.offset, dist)) continue;

    // +x / +y walls are at +72, -x / -y walls at -72, and the robot is `dist` inside it.
    double pos = FIELD_HALF - dist;
    if (wall == 2 || wall == 3) pos = -pos;

    bool isX = (wall == nearX);
    double current = isX ? x : y;
    if (maxCorrection > 0 && std::fabs(pos - current) > maxCorrection) continue;

    if (isX) {
      newX = pos;
      result.x = true;
    } else {
      newY = pos;
      result.y = true;
    }
  }

  if (result.x) chassis.odom_x_set(newX);
  if (result.y) chassis.odom_y_set(newY);
  return result;
}

DistReset::Result DistReset::wallReset(int timeMs, int power, double maxCorrection) {
  chassis.drive_set(-power, -power);
  pros::delay(timeMs);

  // Square on the wall, so heading is the closest multiple of 90.  Rounding keeps
  // the same number of full turns (e.g. 448 -> 450) so later turns don't unwind.
  double theta = std::round(chassis.odom_theta_get() / 90.0) * 90.0;
  chassis.odom_theta_set(theta);

  // The wall behind: 0 = +y, 1 = +x, 2 = -y, 3 = -x (same numbering as reset()).
  int wall = static_cast<int>(std::lround((theta + 180.0) / 90.0)) % 4;
  if (wall < 0) wall += 4;
  bool wallIsX = (wall == 1 || wall == 3);
  double wallPos = FIELD_HALF - backEdgeOffset;
  if (wall == 2 || wall == 3) wallPos = -wallPos;

  // Set the wall axis first so reset() picks the right quadrant, then let the sensors
  // do the other axis.  The bumper on the wall beats any sensor for the wall axis, so
  // it gets set again after (the back sensor is too close to the wall to trust here).
  auto setWallAxis = [&]() {
    if (wallIsX)
      chassis.odom_x_set(wallPos);
    else
      chassis.odom_y_set(wallPos);
  };
  setWallAxis();
  Result result = reset(maxCorrection);
  setWallAxis();

  chassis.drive_set(0, 0);

  if (wallIsX)
    result.x = true;
  else
    result.y = true;
  return result;
}

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
            pros::Distance* backSensor, double backOffset,
            double backEdgeOffset);

  double left(double fallback);
  double right(double fallback);
  double front(double fallback);
  double back(double fallback);

  // Which axes reset() actually changed.
  struct Result {
    bool x = false;
    bool y = false;
  };

  // Resets odom x and y on its own, from wherever the robot is.
  //
  // The quadrant the robot is in (the sign of odom x and y) tells it which two walls
  // are close: x > 0 means the +x wall, y < 0 means the -y wall, and so on.  Then, using
  // odom theta, it works out which way each sensor is pointing on the field
  // (0 = +y, 90 = +x, 180 = -y, 270 = -x) and only uses the ones pointed at one of those
  // two close walls.  A sensor on the +x wall gives  x = 72 - toWall,  on the -x wall
  // x = -72 + toWall, same for y.
  //
  // Sensors pointed at the far walls are skipped - too far away and too much in the way.
  // If no sensor points at a close wall, that axis is left alone.
  //
  // maxCorrection: if a reading would move odom more than this many inches, it's thrown
  // out (the sensor probably saw a game element or robot, not the wall).  0 turns it off.
  //
  //     distReset.reset();        // that's it, no picking sensors or signs
  Result reset(double maxCorrection = 12.0);

  // Backs into the wall behind the robot at `power` for `timeMs`, then:
  //   - heading: snaps odom theta to the closest 0 / 90 / 180 / 270 (the robot is
  //     square on the wall, so that's exactly where it's facing)
  //   - the axis into the wall: 72 - backEdgeOffset from the center, signed for that wall
  //   - the other axis: reset() with the distance sensors, if one points at a close wall
  //
  // Call it already roughly facing away from the wall (within ~45) so it snaps the
  // right way.  Leaves the drive stopped - the next pid_*_set takes over again.
  //
  //     distReset.wallReset(600);
  Result wallReset(int timeMs, int power = 60, double maxCorrection = 12.0);

  ez::Drive& chassis;  // odom theta is the robot angle used for the tilt

  pros::Distance* leftSensor;  // nullptr if there isn't one
  pros::Distance* rightSensor;
  pros::Distance* frontSensor;
  pros::Distance* backSensor;

  double leftOffset;  // inches from the center of rotation to the sensor
  double rightOffset;
  double frontOffset;
  double backOffset;
  double backEdgeOffset;  // inches from the center of rotation to the back of the robot

 private:
  double read(pros::Distance* sensor, double offset, double fallback);
  // Center of the robot to the wall the sensor sees, in inches.  false if there's no good reading.
  bool toWall(pros::Distance* sensor, double offset, double& out);
};

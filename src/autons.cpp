#include "EZ-Template/util.hpp"
#include "config.hpp"
#include "main.h"  // must come first - defines PROS_USE_SIMPLE_NAMES before api.h
#include "subsystems.hpp"
#include <atomic>

/////
// For installation, upgrading, documentations, and tutorials, check out our website!
// https://ez-robotics.github.io/EZ-Template/
/////

// These are out of 127
const int DRIVE_SPEED = 110;
const int TURN_SPEED = 90;
const int SWING_SPEED = 110;

///
// Constants
///
void default_constants() {
  chassis.drive_width_set(10.5);
  // P, I, D, and Start I
  chassis.pid_drive_constants_forward_set(9.4, 0, 27);  // 52
  chassis.pid_drive_constants_backward_set(9.4, 0, 35);
  chassis.pid_heading_constants_set(5.6, 0, 24);            // Holds the robot straight while going forward without odom. 11 40
  chassis.pid_turn_constants_set(1.2, 0.1, 3, 15);          // Turn in place constants
  chassis.pid_swing_constants_set(4, 0.0, 35);           // Swing constants
  chassis.pid_odom_angular_constants_set(6.5, 0.0, 60.5);    // Angular control for odom motions
  chassis.pid_odom_boomerang_constants_set(5.8, 0.0, 32.5);  // Angular control for boomerang motions
  // Exit conditions
  chassis.pid_turn_exit_condition_set(90_ms, 3_deg, 150_ms, 7_deg, 500_ms, 500_ms);  // 90,3_deg,250_ms,7_deg,500_ms,500_ms
  chassis.pid_swing_exit_condition_set(90_ms, 3_deg, 250_ms, 7_deg, 500_ms, 500_ms);
  chassis.pid_drive_exit_condition_set(90_ms, 1_in, 150_ms, 3_in, 500_ms, 500_ms);
  chassis.pid_odom_turn_exit_condition_set(90_ms, 3_deg, 250_ms, 7_deg, 500_ms, 750_ms);
  chassis.pid_odom_drive_exit_condition_set(90_ms, 1_in, 250_ms, 5_in, 500_ms, 750_ms);
  chassis.pid_turn_chain_constant_set(3_deg);
  chassis.pid_swing_chain_constant_set(5_deg);
  chassis.pid_drive_chain_constant_set(3_in);
  // Slew constants
  chassis.slew_turn_constants_set(3_deg, 70);
  chassis.slew_drive_constants_forward_set(3.5_in, 40);
  chassis.slew_drive_constants_backward_set(0.5_in, 110);
  chassis.slew_swing_constants_set(3_in, 80);
  // The amount that turns are prioritized over driving in odom motions
  // - if you have tracking wheels, you can run this higher.  1.0 is the max
  chassis.odom_turn_bias_set(0.9);
  chassis.odom_look_ahead_set(7_in);             // This is how far ahead in the path the robot looks at
  chassis.odom_boomerang_distance_set(16_in);    // This sets the maximum distance away from target that the carrot point can be
  chassis.odom_boomerang_dlead_set(0.625);       // This handles how aggressive the end of boomerang motions are
  chassis.pid_angle_behavior_set(ez::shortest);  // Changes the default behavior for turning, this defaults it to the shortest path there
}

///
// Drive Example
///
void drive_example() {
  // The first parameter is target inches
  // The second parameter is max speed the robot will drive at
  // The third parameter is a boolean (true or false) for enabling/disabling a slew at the start of drive motions
  // for slew, only enable it when the drive distance is greater than the slew distance + a few inches
  
  // chassis.pid_drive_set(24_in, DRIVE_SPEED, false);
  // chassis.pid_wait();

  // chassis.pid_drive_set(-12_in, DRIVE_SPEED);
  // chassis.pid_wait();

  // chassis.pid_drive_set(-12_in, DRIVE_SPEED);
  // chassis.pid_wait();
  /*
  chassis.pid_turn_set(120,100,false);
  chassis.pid_wait();
  chassis.pid_turn_set(0,100,false);
  chassis.pid_wait();
  */
// Swings the left side of the drivetrain until the robot hits 90 degrees
  // chassis.pid_swing_set(ez::LEFT_SWING, 90_deg, 110);
  // chassis.pid_wait();
  chassis.pid_drive_set(24_in, DRIVE_SPEED, false);
  chassis.pid_wait_quick();
  chassis.pid_drive_set(-24_in, DRIVE_SPEED, false);
  chassis.pid_wait_quick();
  chassis.pid_drive_set(24_in, DRIVE_SPEED, false);
  chassis.pid_wait_quick();
  chassis.pid_drive_set(-24_in, DRIVE_SPEED, false);
  chassis.pid_wait_quick();

}

///
// Turn Example
///
void turn_example() {
  // The first parameter is the target in degrees
  // The second parameter is max speed the robot will drive at

  chassis.pid_turn_set(90_deg, TURN_SPEED);
  chassis.pid_wait_quick();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait_quick();

  chassis.pid_turn_set(90_deg, TURN_SPEED);
  chassis.pid_wait_quick();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait_quick();

  chassis.pid_turn_set(90_deg, TURN_SPEED);
  chassis.pid_wait_quick();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait();
}

///
// Combining Turn + Drive
///
void drive_and_turn() {
  chassis.pid_drive_set(24_in, DRIVE_SPEED, true);
  chassis.pid_wait();

  chassis.pid_turn_set(45_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_turn_set(-45_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_drive_set(-24_in, DRIVE_SPEED, true);
  chassis.pid_wait();
}

///
// Wait Until and Changing Max Speed
///
void wait_until_change_speed() {
  // pid_wait_until will wait until the robot gets to a desired position

  // When the robot gets to 6 inches slowly, the robot will travel the remaining distance at full speed
  chassis.pid_drive_set(24_in, 30, true);
  chassis.pid_wait_until(6_in);
  chassis.pid_speed_max_set(DRIVE_SPEED);  // After driving 6 inches at 30 speed, the robot will go the remaining distance at DRIVE_SPEED
  chassis.pid_wait();

  chassis.pid_turn_set(45_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_turn_set(-45_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait();

  // When the robot gets to -6 inches slowly, the robot will travel the remaining distance at full speed
  chassis.pid_drive_set(-24_in, 30, true);
  chassis.pid_wait_until(-6_in);
  chassis.pid_speed_max_set(DRIVE_SPEED);  // After driving 6 inches at 30 speed, the robot will go the remaining distance at DRIVE_SPEED
  chassis.pid_wait();
}

///
// Swing Example
///
void swing_example() {
  // The first parameter is ez::LEFT_SWING or ez::RIGHT_SWING
  // The second parameter is the target in degrees
  // The third parameter is the speed of the moving side of the drive
  // The fourth parameter is the speed of the still side of the drive, this allows for wider arcs

  chassis.pid_swing_set(ez::LEFT_SWING, 45_deg, SWING_SPEED, 45);
  chassis.pid_wait();

  chassis.pid_swing_set(ez::RIGHT_SWING, 0_deg, SWING_SPEED, 45);
  chassis.pid_wait();

  chassis.pid_swing_set(ez::RIGHT_SWING, 45_deg, SWING_SPEED, 45);
  chassis.pid_wait();

  chassis.pid_swing_set(ez::LEFT_SWING, 0_deg, SWING_SPEED, 45);
  chassis.pid_wait();
}

///
// Motion Chaining
///
void motion_chaining() {
  // Motion chaining is where motions all try to blend together instead of individual movements.
  // This works by exiting while the robot is still moving a little bit.
  // To use this, replace pid_wait with pid_wait_quick_chain.
  chassis.pid_drive_set(24_in, DRIVE_SPEED, true);
  chassis.pid_wait();

  chassis.pid_turn_set(45_deg, TURN_SPEED);
  chassis.pid_wait_quick_chain();

  chassis.pid_turn_set(-45_deg, TURN_SPEED);
  chassis.pid_wait_quick_chain();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait();

  // Your final motion should still be a normal pid_wait
  chassis.pid_drive_set(-24_in, DRIVE_SPEED, true);
  chassis.pid_wait();
}

///
// Auto that tests everything
///
void combining_movements() {
  chassis.pid_drive_set(24_in, DRIVE_SPEED, true);
  chassis.pid_wait();

  chassis.pid_turn_set(45_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_swing_set(ez::RIGHT_SWING, -45_deg, SWING_SPEED, 45);
  chassis.pid_wait();

  chassis.pid_turn_set(0_deg, TURN_SPEED);
  chassis.pid_wait();

  chassis.pid_drive_set(-24_in, DRIVE_SPEED, true);
  chassis.pid_wait();
}

///
// Interference example
///
void tug(int attempts) {
  for (int i = 0; i < attempts - 1; i++) {
    // Attempt to drive backward
    printf("i - %i", i);
    chassis.pid_drive_set(-12_in, 127);
    chassis.pid_wait();

    // If failsafed...
    if (chassis.interfered) {
      chassis.drive_sensor_reset();
      chassis.pid_drive_set(-2_in, 20);
      pros::delay(1000);
    }
    // If the robot successfully drove back, return
    else {
      return;
    }
  }
}

// If there is no interference, the robot will drive forward and turn 90 degrees.
// If interfered, the robot will drive forward and then attempt to drive backward.
void interfered_example() {
  chassis.pid_drive_set(24_in, DRIVE_SPEED, true);
  chassis.pid_wait();

  if (chassis.interfered) {
    tug(3);
    return;
  }

  chassis.pid_turn_set(90_deg, TURN_SPEED);
  chassis.pid_wait();
}

///
// Odom Drive PID
///
void odom_drive_example() {
  // This works the same as pid_drive_set, but it uses odom instead!
  // You can replace pid_drive_set with pid_odom_set and your robot will
  // have better error correction.

  chassis.pid_odom_set(24_in, DRIVE_SPEED, true);
  chassis.pid_wait();

  chassis.pid_odom_set(-12_in, DRIVE_SPEED);
  chassis.pid_wait();

  chassis.pid_odom_set(-12_in, DRIVE_SPEED);
  chassis.pid_wait();
}

///
// Odom Pure Pursuit
///
void odom_pure_pursuit_example() {
  // Drive to 0, 30 and pass through 6, 10 and 0, 20 on the way, with slew
  chassis.pid_odom_set({{{6_in, 10_in}, fwd, DRIVE_SPEED},
                        {{0_in, 20_in}, fwd, DRIVE_SPEED},
                        {{0_in, 30_in}, fwd, DRIVE_SPEED}},
                       true);
  chassis.pid_wait();

  // Drive to 0, 0 backwards
  chassis.pid_odom_set({{0_in, 0_in}, rev, DRIVE_SPEED},
                       true);
  chassis.pid_wait();
}

///
// Odom Pure Pursuit Wait Until
///
void odom_pure_pursuit_wait_until_example() {
  chassis.pid_odom_set({{{0_in, 24_in}, fwd, DRIVE_SPEED},
                        {{12_in, 24_in}, fwd, DRIVE_SPEED},
                        {{24_in, 24_in}, fwd, DRIVE_SPEED}},
                       true);
  chassis.pid_wait_until_index(1);  // Waits until the robot passes 12, 24
  // Intake.move(127);  // Set your intake to start moving once it passes through the second point in the index
  chassis.pid_wait();
  // Intake.move(0);  // Turn the intake off
}

///
// Odom Boomerang
///
void odom_boomerang_example() {
  chassis.pid_odom_set({{0_in, 24_in, 45_deg}, fwd, DRIVE_SPEED},
                       true);
  chassis.pid_wait();

  chassis.pid_odom_set({{0_in, 0_in, 0_deg}, rev, DRIVE_SPEED},
                       true);
  chassis.pid_wait();
}

///
// Odom Boomerang Injected Pure Pursuit
///
void odom_boomerang_injected_pure_pursuit_example() {
  chassis.pid_odom_set({{{0_in, 24_in, 45_deg}, fwd, DRIVE_SPEED},
                        {{12_in, 24_in}, fwd, DRIVE_SPEED},
                        {{24_in, 24_in}, fwd, DRIVE_SPEED}},
                       true);
  chassis.pid_wait();

  chassis.pid_odom_set({{0_in, 0_in, 0_deg}, rev, DRIVE_SPEED},
                       true);
  chassis.pid_wait();
}

///
// Calculate the offsets of your tracking wheels
///
void measure_offsets() {
  // Number of times to test
  int iterations = 10;

  // Our final offsets
  double l_offset = 0.0, r_offset = 0.0, b_offset = 0.0, f_offset = 0.0;

  // Reset all trackers if they exist
  if (chassis.odom_tracker_left != nullptr) chassis.odom_tracker_left->reset();
  if (chassis.odom_tracker_right != nullptr) chassis.odom_tracker_right->reset();
  if (chassis.odom_tracker_back != nullptr) chassis.odom_tracker_back->reset();
  if (chassis.odom_tracker_front != nullptr) chassis.odom_tracker_front->reset();

  for (int i = 0; i < iterations; i++) {
    // Reset pid targets and get ready for running an auton
    chassis.pid_targets_reset();
    chassis.drive_imu_reset();
    // chassis.drive_sensor_reset();
    // chassis.drive_brake_set(MOTOR_BRAKE_HOLD);
    chassis.odom_xyt_set(0_in, 0_in, 0_deg);
    double imu_start = chassis.odom_theta_get();
    double target = i % 2 == 0 ? 90 : 270;  // Switch the turn target every run from 270 to 90

    // Turn to target at half power
    chassis.pid_turn_set(target, 63, ez::raw);
    chassis.pid_wait();
    pros::delay(250);

    // Calculate delta in angle
    double t_delta = util::to_rad(fabs(util::wrap_angle(chassis.odom_theta_get() - imu_start)));

    // Calculate delta in sensor values that exist
    double l_delta = chassis.odom_tracker_left != nullptr ? chassis.odom_tracker_left->get() : 0.0;
    double r_delta = chassis.odom_tracker_right != nullptr ? chassis.odom_tracker_right->get() : 0.0;
    double b_delta = chassis.odom_tracker_back != nullptr ? chassis.odom_tracker_back->get() : 0.0;
    double f_delta = chassis.odom_tracker_front != nullptr ? chassis.odom_tracker_front->get() : 0.0;

    // Calculate the radius that the robot traveled
    l_offset += l_delta / t_delta;
    r_offset += r_delta / t_delta;
    b_offset += b_delta / t_delta;
    f_offset += f_delta / t_delta;
  }

  // Average all offsets
  l_offset /= iterations;
  r_offset /= iterations;
  b_offset /= iterations;
  f_offset /= iterations;

  // Set new offsets to trackers that exist
  if (chassis.odom_tracker_left != nullptr) chassis.odom_tracker_left->distance_to_center_set(l_offset);
  if (chassis.odom_tracker_right != nullptr) chassis.odom_tracker_right->distance_to_center_set(r_offset);
  if (chassis.odom_tracker_back != nullptr) chassis.odom_tracker_back->distance_to_center_set(b_offset);
  if (chassis.odom_tracker_front != nullptr) chassis.odom_tracker_front->distance_to_center_set(f_offset);
}
///
// Auto-close claw
///
// Both versions return right away and wait in a background task, so drive motions,
// the lift and anything else in the auton keep running while the claw waits.
// The claw closes as soon as clawDistance reads under CLAW_GRAB_MM.  If that never
// happens, the fallback closes it anyway:
//   claw_autoclose(800);                   -> after 800 ms
//   claw_autoclose({24, -24}, 15000);      -> once the robot is within 1 in of (24, -24), or after 15 s
//   claw_autoclose({24, -24}, 15000, 2);   -> same, but within 2 in
// Calling either one again cancels whatever close was still waiting.
const int CLAW_GRAB_MM = 70;
static std::atomic<int> claw_close_id{0};  // bumped every call so an older wait gives up

static bool claw_sees_object() { return clawDistance.get() < CLAW_GRAB_MM; }

// Fallback is a timeout in milliseconds (in driver mode, lift_pivot_after is true and timeout leaves the claw open)
void claw_autoclose(int timeout_ms, bool lift_pivot_after) {
  int id = ++claw_close_id;
  pros::Task([id, timeout_ms, lift_pivot_after]() {
    uint32_t start = pros::millis();
    while (id == claw_close_id) {
      if (claw_sees_object()) {
        claw.set(false);  // false is closed
        if (lift_pivot_after) {
          pros::delay(100);  // give the claw a moment to grip before raising the arm
          if (id == claw_close_id && lift.state == Lift::MATCHLOADING) {
            lift.setPivot(lift.MATCHLOAD_LIFT_ANGLE);
          }
        }
        return;
      }
      if (pros::millis() - start >= (uint32_t)timeout_ms) {
        if (!lift_pivot_after) claw.set(false);  // auton fallback closes; driver mode leaves it open
        return;
      }
      pros::delay(10);
      std::cout<< "Claw distance: " << clawDistance.get() << std::endl;
    }
  }, "claw close");
}

// Fallback is reaching a point on the field (inches), or timeout_ms if the point is never reached
void claw_autoclose(ez::pose target, int timeout_ms, double tolerance_in) {
  int id = ++claw_close_id;
  pros::Task([id, target, tolerance_in, timeout_ms]() {
    uint32_t start = pros::millis();
    while (id == claw_close_id) {
      if (claw_sees_object() ||
          util::distance_to_point(target, chassis.odom_pose_get()) <= tolerance_in ||
          pros::millis() - start >= (uint32_t)timeout_ms) {
        claw.set(false);  // false is closed
        return;
      }
      pros::delay(10);
    }
  }, "claw close at point");
}

/*
void driver_side_three_pin() {
  chassis.odom_xyt_set(-61, 0, 90);
  chassis.pid_odom_set({{-51, 0}, fwd, 90}, false);
  lift.set(Lift::LEVEL_2);
  clawPivot.set(true);
  chassis.pid_wait_quick();
  pros::delay(1000);
  chassis.pid_odom_set({{-65, 0}, rev, 60}, false);
  chassis.pid_wait_quick();
  lift.set(Lift::ROLLERS);
  pros::delay(1000);
  clawIntake.move(0);
  chassis.pid_odom_set({{-42, 0}, fwd, 90}, false);
  pros::delay(750);
  clawPivot.set(false);
  lift.set(Lift::LOADING);
  pros::delay(500);
  intake.set(Intake::INTAKE);
  clawIntake.move(-127);
  chassis.pid_wait_quick();
  chassis.pid_turn_set(0, 0);
  chassis.pid_wait_quick();
  pros::delay(750);
  chassis.pid_odom_set({{-42, -18}, rev, 90}, false);
  pros::delay(300);
  clawPivot.set(true);
  chassis.pid_wait_quick();
  pros::delay(200);
  lift.set(Lift::ZERO);
  clawIntake.move(50);
  pros::delay(500);
  lift.set(Lift::LEVEL_1);
  pros::delay(500);

  // pick up second pin
  chassis.pid_odom_set({{-44, 2}, fwd, 90}, false);
  lift.set(Lift::LEVEL_1);
  chassis.pid_wait_quick();
  chassis.pid_turn_set(-90, 90);
  chassis.pid_wait_quick();
  chassis.pid_odom_set({{-21, 2}, rev, 40}, false);
  clawPivot.set(true);
  clawIntake.move(100);
  chassis.pid_wait();
  lift.set(Lift::ZERO);
  double target = lift.ZERO_HEIGHT;
  double tolerance = 25.0;  // degrees

  while (std::abs((lift.liftRotation.get_position() / 100.0) - target) > tolerance) {
    pros::delay(10);
  }

  clawIntake.move(127);
  pros::delay(150);
  // chassis.pid_odom_set({{-41, 2}, fwd, 90}, false);
  // lift.set(Lift::LEVEL_1);
  // chassis.pid_wait_quick();
  // chassis.pid_turn_set(0, 90);
  // chassis.pid_wait_quick();
  // chassis.pid_odom_set({{-42, -19}, rev, 90}, false);
  // chassis.pid_wait_quick();
  // clawIntake.move(50);
  // pros::delay(30000);

  // // score second pin
  // clawIntake.move(0);
  // lift.set(Lift::LEVEL_3);
  // chassis.pid_turn_set({-41, -13}, rev, 90);
  // chassis.pid_wait_quick();
  // chassis.pid_odom_set({{-41, -13}, rev, 90}, false);
  // chassis.pid_wait_quick();
  // clawIntake.move(-127);
  // pros::delay(300);

  // // pick up third pin
  // chassis.pid_odom_set({{-29, -8}, fwd, 90}, false);
  // chassis.pid_wait_quick();
  // chassis.pid_turn_set({-27, -16}, rev, 90);
  // lift.set(Lift::LEVEL_2);
  // clawIntake.move(127);
  // chassis.pid_wait_quick();
  // chassis.pid_odom_set({{-27, -16}, rev, 90}, false);
  // chassis.pid_wait_quick();
  // lift.set(Lift::LEVEL_1);

  // // score third pin
  // chassis.pid_turn_set({-36, -20}, rev, 90);
  // lift.set(Lift::LEVEL_4);
  // clawIntake.move(0);
  // chassis.pid_wait_quick();
  // chassis.pid_odom_set({{-36, -20}, rev, 90}, false);
  // chassis.pid_wait_quick();
  // clawIntake.move(-127);
}
  */

void test_autoclose() {
  claw.set(true);
  chassis.pid_drive_set(26,50);
  claw_autoclose(3000);
  chassis.pid_wait();
}

void skillz(){
  lift.pivotSpeed = 127;
  chassis.odom_pose_set({0,-63,180});
  toggleMotor.move(127);
  pros::delay(500);
  toggleMotor.move(0);
  //lift.setPivot(127);
  chassis.pid_drive_set(-17,120,false);
  chassis.pid_wait_quick();
  lift.set(Lift::MATCHLOADING);
  chassis.pid_turn_set(90,120);
  chassis.pid_wait();
  chassis.pid_drive_set(15,120,false);
  chassis.pid_wait_quick();
  claw.set(true);
  chassis.pid_drive_set(-15,120,false);
  chassis.pid_wait_quick_chain();
  chassis.pid_turn_set({24, -24}, fwd,  90);
  chassis.pid_wait();
  claw_autoclose(5000);
  chassis.pid_odom_set({{24, -24}, fwd,50}, false);
  chassis.pid_wait();
  //std::cout<<chassis.odom_x_get()<<", "<<chassis.odom_y_get()<<", "<<chassis.odom_theta_get()<<std::endl;
  lift.setPivot(127);
  chassis.pid_turn_set(185, 120);
  chassis.pid_wait();
  chassis.pid_drive_set(14,120,false);
  chassis.pid_wait();     // wait until lift is actually at that height
  lift.score();                        // drops 300 deg -> opens claw -> waits 200 ms -> raises 700 deg -> holds
}
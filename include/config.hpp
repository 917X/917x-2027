#pragma once
#include "EZ-Template/api.hpp"
#include "EZ-Template/tracking_wheel.hpp"
#include "api.h"
#include "pros/distance.hpp"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "pros/rotation.hpp"
#include "subsystem/dist_reset.hpp"
#include "subsystem/intake.hpp"
#include "subsystem/lift.hpp"

/**
 * Every device on the robot is declared here and defined in config.cpp.
 * Ports live in config.cpp so there is exactly one place to change them.
 *
 * Two robots are declared here:
 *  - the 2026-27 robot, which is what actually runs
 *  - last season's Push Back robot, kept for reference.  Nothing starts its
 *    tasks or calls its autons, so those objects sit unused.  Their ports are
 *    prefixed PB_ in config.cpp and several of them overlap this year's ports.
 */

extern pros::Controller controller;

// ---------------------------------------------------------------------------
// 2026-27 robot
// ---------------------------------------------------------------------------

extern ez::Drive chassis;

extern pros::Motor intakeMotor;
extern Intake intake;
extern pros::MotorGroup liftMotors;
extern pros::Rotation liftRotation;
extern pros::Motor pivotMotor;
extern pros::Rotation pivotRotation;
extern pros::Motor toggleMotor;
extern ez::Piston claw;
extern Lift lift;
extern ez::tracking_wheel horiz_tracker;
extern pros::Distance leftDist;
extern pros::Distance backDist;
extern pros::Distance frontDist;
extern DistReset distReset;

// ---------------------------------------------------------------------------
// Last season (Push Back) - reference only, not wired up
// ---------------------------------------------------------------------------

extern pros::adi::DigitalOut intakePiston;

extern pros::adi::DigitalOut flapperPiston;

extern pros::Imu imu;
extern pros::MotorGroup rightMotors;
extern pros::MotorGroup leftMotors;
extern pros::Distance distance;

extern pros::Distance clawDistance;

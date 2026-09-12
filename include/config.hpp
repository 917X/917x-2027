#pragma once
#include "EZ-Template/api.hpp"
#include "api.h"
#include "pros/distance.hpp"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/imu.hpp"
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

extern pros::MotorGroup liftMotors;
extern ez::Piston claw;
extern ez::Piston clawPivot;
extern Lift lift;

// ---------------------------------------------------------------------------
// Last season (Push Back) - reference only, not wired up
// ---------------------------------------------------------------------------

extern pros::adi::DigitalOut intakePiston;
extern pros::Motor indexerMotor;
extern pros::Motor frontRoller;
extern pros::Motor middleRoller;
extern pros::Motor bottomRoller;
extern Intake intake;

extern pros::adi::DigitalOut flapperPiston;

extern pros::Optical colorSort;
extern pros::Imu imu;
extern pros::MotorGroup rightMotors;
extern pros::MotorGroup leftMotors;
extern pros::Distance distance;

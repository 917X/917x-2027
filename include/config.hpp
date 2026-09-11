#pragma once
#include "EZ-Template/api.hpp"
#include "api.h"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "subsystem/lift.hpp"

/**
 * Every device on the robot is declared here and defined in config.cpp.
 * Ports live in config.cpp so there is exactly one place to change them.
 */

extern pros::Controller controller;

// drivetrain
extern ez::Drive chassis;

// intake
extern pros::Motor intakeMotor;

// lift + claw
extern pros::MotorGroup liftMotors;
extern ez::Piston claw;
extern ez::Piston clawPivot;
extern Lift lift;

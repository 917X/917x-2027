#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves to and holds
// that height; RAW_UP / RAW_DOWN move freely and RAW_HOLD holds where they stopped.
class Lift {
 public:
  Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, ez::Piston& scoringPiston);
  enum LiftState { LOADING,
                   ROLLERS,
                   LEVEL_1,
                   LEVEL_2,
                   LEVEL_3,
                   LEVEL_4,
                   LEVEL_5,
                   RAW_UP,
                   RAW_DOWN,
                   RAW_HOLD };

  void liftControl();
  void set(LiftState state);
  void levelUp();
  void levelDown();

  pros::Motor& liftMotor;
  pros::Rotation& liftRotation;
  ez::Piston& scoringPiston;
  LiftState state = LOADING;
  LiftState level = LOADING;  // last LOADING/ROLLERS..LEVEL_5 picked, so levelUp/levelDown still work after raw

  int speed = 127;     // max power moving to a level
  int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
  double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

  // Heights in rotation sensor degrees.  PLACEHOLDERS - tune on the robot
  double LOADING_HEIGHT = 750;
  double ROLLERS_HEIGHT = 2400;
  double LEVEL_1_HEIGHT = 2500;
  double LEVEL_2_HEIGHT = 4750;
  double LEVEL_3_HEIGHT = 6750;
  double LEVEL_4_HEIGHT = 8750;
  double LEVEL_5_HEIGHT = 10750;

  // The lift never drives past these  (PLACEHOLDERS - tune on the robot)
  double MIN_POSITION = 0;
  double MAX_POSITION = 100000;  // was 1005000 (typo); set ~2× max level height as hard safety stop

  // scoringPiston turns on once the lift rises above PISTON_ON_POSITION and off once it
  // drops below PISTON_OFF_POSITION.  PLACEHOLDERS - keep ON above OFF so it can't flicker
  double PISTON_ON_POSITION = 50;
  double PISTON_OFF_POSITION = 25;
};

#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves the lift to and
// holds that height, and swings the pivoting arm to the angle that goes with it.  The LEVELs
// have two sets of heights - see isBackMode.  RAW_UP /
// RAW_DOWN move the lift freely and RAW_HOLD holds where they stopped.  The arm holds its
// last angle through the raw states, HOMING and ZERO.
class Lift {
 public:
  Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, pros::Motor& pivotMotor,
       pros::Rotation& pivotRotation, ez::Piston& claw);
  enum LiftState { ZERO,
                   MATCHLOADING,
                   LOADING,
                   LEVEL_1,
                   LEVEL_2,
                   LEVEL_3,
                   LEVEL_4,
                   LEVEL_5,
                   LEVEL_6,
                   LEVEL_7,
                   RAW_UP,
                   RAW_DOWN,
                   RAW_HOLD,
                   HOMING,
                   SCORE };
  enum ScorePhase { DROPPING,
                    RELEASING,
                    RAISING };

  void liftControl();
  void set(LiftState state);
  void levelUp();
  void levelDown();
  void home();   // starts HOMING - the lift task drives down until it stalls, then zeroes
  void score();  // starts SCORE - drop scoreDrop, open the claw if it's closed, then go up to scoreRise above where it was
  void setPivot(double angle);  // swings the arm to angle and keeps it there until the next set() - the lift doesn't move
  void toggleFrontBack();       // flips isBackMode - if it's at a LEVEL it moves to the other height and angle right away

  pros::Motor& liftMotor;
  pros::Rotation& liftRotation;
  pros::Motor& pivotMotor;
  pros::Rotation& pivotRotation;
  ez::Piston& claw;        // true is open.  SCORE opens it - opcontrol toggles it the rest of the time
  LiftState state = ZERO;
  LiftState level = ZERO;  // last ZERO..LEVEL_7 picked, so levelUp/levelDown still work after raw

  int speed = 127;     // max power moving to a level
  int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
  double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

  int pivotSpeed = 127;    // max power moving to an angle
  double pivotKP = 0.4;    // power per degree of error  (PLACEHOLDER)
  double pivotTarget = 0;  // starts at 0 so the arm stays put until a state gives it an angle
  int pivotRawPower = 0;   // raw manual override power from controller

  // false (front) is LEVEL_x_HEIGHT with the arm at FRONT_ANGLE,
  // true (back) is BACK_LEVEL_x_HEIGHT with the arm at BACK_ANGLE.
  bool isBackMode = false;
  bool pivotManual = false;      // true after setPivot() or manual raw pivot - the lift state stops picking the arm angle
  double pivotManualTarget = 0;  // the angle setPivot() or manual hold asked for

  // Homing
  int homeSpeed = 45;
  int homeLooseCurrent = 130;   // mA
  int homeLoadedCurrent = 160;  // mA
  int homeLoadedTime = 50;      // ms
  int homeIgnoreTime = 100;     // ms - only delays when it starts looking, it's not a timeout
  int homeTime = 0;             // ms since homing started
  bool homeLoose = false;       // has the string gone loose yet
  int loadedTime = 0;
  int homePrintCounter = 0;

  // Score
  double scoreDrop = 300;       // degrees, same units as the heights below
  double scoreRise = 700;       // degrees above the starting position it ends at
  int scoreReleaseTime = 200;   // ms  (PLACEHOLDER)
  double scoreTolerance = 30;   // degrees  (PLACEHOLDER)
  int scoreMoveTimeout = 1000;  // ms
  ScorePhase scorePhase = DROPPING;
  double scoreStart = 0;  // where it was when score() was called
  int scoreTime = 0;      // ms in the current phase

  // Heights in rotation sensor degrees.  PLACEHOLDERS - tune on the robot
  double ZERO_HEIGHT = 0;
  double MATCHLOADING_HEIGHT = 0;
  double LOADING_HEIGHT = 500;

  // Front Heights (Levels 1-4)
  double LEVEL_1_HEIGHT = 1294;
  double LEVEL_2_HEIGHT = 2185;
  double LEVEL_3_HEIGHT = 3874;
  double LEVEL_4_HEIGHT = 3874;

  // Back Heights (Levels 3-7)
  double BACK_LEVEL_3_HEIGHT = 2513;
  double BACK_LEVEL_4_HEIGHT = 3170;
  double BACK_LEVEL_5_HEIGHT = 3170;
  double BACK_LEVEL_6_HEIGHT = 3170;
  double BACK_LEVEL_7_HEIGHT = 3170;

  // The lift never drives past these.  HOMING ignores them - it has to go below the old zero.
  double MIN_POSITION = -10;    // just under ZERO_HEIGHT, so raw down stops before the string re-winds
  double MAX_POSITION = 18000;  // about where a LEVEL_7 would be - redo if the heights change

  // Arm angles in rotation sensor degrees, 0 is where the arm is when the program starts.
  // PLACEHOLDERS - tune on the robot
  double FRONT_ANGLE = 293;
  double PRESCORING_ANGLE = 137;
  double BACK_ANGLE = 132;
  double LOADING_ANGLE = 0;

  // The arm never drives past these
  double PIVOT_MIN_ANGLE = -360;
  double PIVOT_MAX_ANGLE = 360;
};
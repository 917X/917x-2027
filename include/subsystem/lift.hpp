#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves the lift to and
// holds that height, and swings the pivoting arm to the angle that goes with it.  The LEVELs
// have two sets of heights - see isBackMode.  RAW_UP /
// RAW_DOWN move the lift freely and RAW_HOLD holds where they stopped.  The arm holds its
// last angle through the raw states, HOMING, ZERO and CUSTOM_HEIGHT.  setHeight() goes to and
// holds any height you give it.  Going from MATCHLOADING to a front level, the lift goes up right
// away while the arm waits at FRONT_SCORING_ANGLE until the lift gets there, then goes to
// FRONT_LEVEL_ANGLE - see scoringAngleHold.
class Lift {
 public:
  Lift(pros::MotorGroup& liftMotors, pros::Rotation& liftRotation, pros::Motor& pivotMotor,
       pros::Rotation& pivotRotation, ez::Piston& claw);
  enum LiftState { ZERO,
                   MATCHLOADING,
                   FLOORPIN,  // picking a pin up off the floor - FLOORPIN_HEIGHT, arm at FLOORPIN_ANGLE
                   LOADING,
                   LEVEL_1,
                   LEVEL_2,
                   LEVEL_3,
                   LEVEL_4,
                   LEVEL_5,
                   RAW_UP,
                   RAW_DOWN,
                   RAW_HOLD,
                   HOMING,
                   SCORE,
                   CUSTOM_HEIGHT };  // setHeight() - holds customHeight, the arm holds its last angle
  enum ScorePhase { DROPPING,
                    RELEASING,
                    RAISING };

  void liftControl();
  void set(LiftState state);
  void setLevel(LiftState level, bool back);       // set() with the side picked - back only matters for LEVEL_2-3.
                                                   // LEVEL_1 is always front and LEVEL_4-5 always back
  void setHeight(double height);                   // goes to and holds height (rotation sensor degrees, kept between
                                                   // MIN_POSITION and MAX_POSITION).  The arm doesn't move - call setPivot() after it to move the arm
  bool waitUntilCompletion(int timeout = 2000, bool waitForArm = false);
                                                   // blocks until the lift is within waitTolerance of its height, or timeout
                                                   // ms pass.  waitForArm also waits for the arm to be within pivotWaitTolerance
                                                   // of its angle.  Waits for SCORE / HOMING to finish first.  false if it timed out
  void levelUp();                                  // next level up - LEVEL_1 is on the front, LEVEL_2-5 always go to the back
  void levelDown();                                // next level down - same front / back rule as levelUp, LEVEL_1 goes down to LOADING
  void home();                                     // starts HOMING - the lift task drives down until it stalls, then zeroes
  void score();                                    // starts SCORE - drop scoreDrop, open the claw if it's closed, then go up scoreRise from where it let go.
                                                   // At zero it only opens the claw.
  void driverScore();                              // LEFT's score - same as score() up to the release (but drops driverScoreDrop, never below
                                                   // ZERO_HEIGHT), then goes all the way down to ZERO.  At zero it only opens the claw.
  void startScore(bool toZero);                    // what score() and driverScore() both call
  void setPivot(double angle);                     // swings the arm to angle and keeps it there until the next set() - the lift doesn't move
  void toggleFrontBack();                          // flips isBackMode and keeps the same level - zero or LEVEL_1 go to back LEVEL_2
  double levelHeight(LiftState level, bool back);  // the height a state aims for, front or back
  double targetHeight();                           // the height the lift is aiming for right now

  pros::MotorGroup& liftMotors;  // 1 or 2 motors - the second one is only plugged in sometimes
  pros::Rotation& liftRotation;
  pros::Motor& pivotMotor;
  pros::Rotation& pivotRotation;
  ez::Piston& claw;  // true is open.  SCORE opens it - opcontrol toggles it the rest of the time
  LiftState state = ZERO;
  LiftState level = ZERO;   // last ZERO..LEVEL_5 picked, so levelUp/levelDown still work after raw
  double customHeight = 0;  // where CUSTOM_HEIGHT aims - set by setHeight()

  int speed = 127;  // max power moving to a level
  int lowerSpeed = 80;
  int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
  double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

  int pivotSpeed = 127;        // max power moving to an angle - both kPs below share it
  double pivotKP = 0.4;        // everything except back scoring - power per degree of error  (PLACEHOLDER)
  double pivotLoadedKP = 1.4;  // back scoring (back LEVEL_2-5), since the arm carries the pin over to the back  (PLACEHOLDER)
  int matchloadPushPower = 40;        // MATCHLOADING - raw power swinging the arm to FRONT_ANGLE and pushing it into the hardstop  (PLACEHOLDER)
  double matchloadPushTolerance = 2;  // degrees - within this of FRONT_ANGLE (or past it) counts as landed  (PLACEHOLDER)
  int matchloadPushTime = 300;        // ms it keeps pushing after it lands - then plain P holds FRONT_ANGLE  (PLACEHOLDER)
  int matchloadPushLeft = 0;          // ms of pushing left since it landed
  bool matchloadHolding = false;      // done pushing, P is holding FRONT_ANGLE - set() clears it
  double pivotTarget = 0;      // starts at 0 so the arm stays put until a state gives it an angle
  int pivotRawPower = 0;       // raw manual override power from controller

  // false (front) is LEVEL_x_HEIGHT with the arm at FRONT_ANGLE,
  // true (back) is BACK_LEVEL_x_HEIGHT with the arm at BACK_ANGLE.
  bool isBackMode = false;
  bool pivotManual = false;      // true after setPivot() or manual raw pivot - the lift state stops picking the arm angle
  double pivotManualTarget = 0;  // the angle setPivot() asked for, or where raw pivot was let go

  // waitUntilCompletion
  double waitTolerance = 25;      // degrees of lift  (PLACEHOLDER)
  double pivotWaitTolerance = 5;  // degrees of arm  (PLACEHOLDER)

  // Going from MATCHLOADING to a front level, the arm stays at FRONT_SCORING_ANGLE while the lift goes up,
  // and goes to FRONT_LEVEL_ANGLE once the lift is within levelAngleTolerance of the level's height
  double levelAngleTolerance = 25;  // degrees of lift  (PLACEHOLDER)
  bool scoringAngleHold = false;    // the arm is waiting at FRONT_SCORING_ANGLE for the lift

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
  double scoreDrop = 600;       // degrees, same units as the heights below
  int scoreDropSpeed = 60;      // max power going down during SCORE - lowerSpeed is 80  (PLACEHOLDER)
  double scoreRise = 0;         // degrees it goes back up after letting go - 0 holds where it let go
  int scoreReleaseTime = 200;   // ms  (PLACEHOLDER)
  double scoreTolerance = 30;   // degrees  (PLACEHOLDER)
  int scoreMoveTimeout = 1000;  // ms
  ScorePhase scorePhase = DROPPING;
  double scoreStart = 0;  // where it was when score() was called
  double scoreBottom = 0;  // where it let go - RAISING goes up scoreRise from here
  int scoreTime = 0;      // ms in the current phase
  double driverScoreDrop = 300;  // degrees driverScore() drops before it opens the claw
  bool scoreToZero = false;      // this SCORE is driverScore() - after the release it goes down to ZERO instead of up

  // Heights in rotation sensor degrees.  PLACEHOLDERS - tune on the robot
  double ZERO_HEIGHT = 0;
  double MATCHLOADING_HEIGHT = -10;
  double MATCHLOADING_PIVOT_HEIGHT = 192.8;  // A doesn't swing the arm until the lift is below this
  double LOADING_HEIGHT = 0;
  double FLOORPIN_HEIGHT = 136.9;

  // Front Heights (Levels 1-3)
  double LEVEL_1_HEIGHT = 1842.8;
  double LEVEL_2_HEIGHT = 3088.8;
  double LEVEL_3_HEIGHT = 3830.8;

  // Back Heights (Levels 2-5)
  double BACK_LEVEL_2_HEIGHT = 0;
  double BACK_LEVEL_3_HEIGHT = 990.8;
  double BACK_LEVEL_4_HEIGHT = 2333.8;
  double BACK_LEVEL_5_HEIGHT = 3262.8;

  // The lift never drives past these.  HOMING ignores them - it has to go below the old zero.
  double MIN_POSITION = -10;    // just under ZERO_HEIGHT, so raw down stops before the string re-winds
  double MAX_POSITION = 18000;  // about where a LEVEL_7 would be - redo if the heights change

  // Arm angles in rotation sensor degrees, 0 is where the arm is when the program starts.
  // PLACEHOLDERS - tune on the robot
  double FRONT_ANGLE = 297.5;          // MATCHLOADING - picking up from the front, against the hardstop
  double FLOORPIN_ANGLE = 280;       // FLOORPIN
  double FRONT_SCORING_ANGLE = 260;  // leaving MATCHLOADING for a front level, the arm goes here first and stays until the lift
                                     // is at the level.  claw_autoclose also raises the arm here after it grabs a matchload
  double FRONT_LEVEL_ANGLE = 272;    // front LEVEL_1-3 - the arm holds this once the lift is at the level
  double PRESCORING_ANGLE = 135;
  double BACK_ANGLE = 128;
  double LOADING_ANGLE = -5;

  // The arm never drives past these
  double PIVOT_MIN_ANGLE = -10;  // must be the smaller number - if MIN > MAX every PID move gets zeroed
  double PIVOT_MAX_ANGLE = 300;  // just past FRONT_ANGLE - below it the arm can't reach matchloading
};
#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves the lift to and
// holds that height, and swings the pivoting arm to the angle that goes with it.  RAW_UP /
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

  pros::Motor& liftMotor;
  pros::Rotation& liftRotation;
  pros::Motor& pivotMotor;
  pros::Rotation& pivotRotation;
  ez::Piston& claw;  // true is open.  SCORE opens it - opcontrol toggles it the rest of the time
  LiftState state = ZERO;
  LiftState level = ZERO;  // last ZERO..LEVEL_6 picked, so levelUp/levelDown still work after raw

  int speed = 127;     // max power moving to a level
  int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
  double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

  int pivotSpeed = 127;    // max power moving to an angle
  double pivotKP = 1.0;    // power per degree of error  (PLACEHOLDER)
  double pivotTarget = 0;  // starts at 0 so the arm stays put until a state gives it an angle

  // Homing - the lift hangs on a string wound around the axle, so there's no hard stop.
  // Driving down unwinds the string; once it's loose the axle spins freely and the motor
  // barely draws any current.  Keep going and the string wraps the other way and starts
  // picking the lift back up - the motor feels the lift's weight and its current jumps.
  // That jump is the bottom.
  //   1. Drive down at homeSpeed.  Ignore the current for the first homeIgnoreTime ms - it
  //      reads 0 before the motor starts and spikes while it speeds up.
  //   2. Wait until the current drops under homeLooseCurrent - the string's loose (or the
  //      lift is dropping).
  //   3. After that, once the current has been over homeLoadedCurrent for homeLoadedTime
  //      ms straight, it's lifting the lift again - stop, zero and go to ZERO.
  // There's no timeout.  Measured at -45: loose is 50-105 mA (~84 RPM), lifting is
  // 220-350 mA (~70 RPM).  The speed only drops ~15% but the current triples, so we watch
  // the current.  Lower homeLoadedCurrent to stop sooner - keep it above the loose current.
  int homeSpeed = 45;
  int homeLooseCurrent = 130;   // mA
  int homeLoadedCurrent = 160;  // mA
  int homeLoadedTime = 50;      // ms
  int homeIgnoreTime = 100;     // ms - only delays when it starts looking, it's not a timeout
  int homeTime = 0;             // ms since homing started
  bool homeLoose = false;       // has the string gone loose yet
  int loadedTime = 0;
  int homePrintCounter = 0;

  // Score - drops scoreDrop degrees below where the lift is, then if the claw is closed,
  // opens it and waits scoreReleaseTime ms for it to let go (already open skips the wait).
  // Then goes up to scoreRise above where it started and holds there (RAW_HOLD - L1 / L2
  // still step from the old level).  Each move counts as done within scoreTolerance
  // degrees, or after scoreMoveTimeout ms so it can't get stuck.  The arm stays at its
  // angle the whole time.  Any other lift button cancels it.
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
  double LOADING_HEIGHT = 1650;
  double LEVEL_1_HEIGHT = 2950;
  double LEVEL_2_HEIGHT = 5200;
  double LEVEL_3_HEIGHT = 7200;
  double LEVEL_4_HEIGHT = 9200;
  double LEVEL_5_HEIGHT = 13000;
  double LEVEL_6_HEIGHT = 15500;  // LEVEL_5 + 2500, the average step from LEVEL_1 to LEVEL_5
  // The lift never drives past these.  HOMING ignores them - it has to go below the old zero.
  double MIN_POSITION = -10;     // just under ZERO_HEIGHT, so raw down stops before the string re-winds
  double MAX_POSITION = 18000;   // about where a LEVEL_7 would be (LEVEL_6 + 2500) - redo if the heights change

  // Arm angles in rotation sensor degrees, 0 is where the arm is when the program starts.
  // PLACEHOLDERS - tune on the robot, keep MATCHLOADING_ANGLE < SCORING_ANGLE < LOADING_ANGLE
  double MATCHLOADING_ANGLE = 0;
  double SCORING_ANGLE = 90;
  double LOADING_ANGLE = 180;
  // The arm never drives past these  (PLACEHOLDERS - tune on the robot)
  double PIVOT_MIN_ANGLE = -20;
  double PIVOT_MAX_ANGLE = 200;
};

// miguel phonk

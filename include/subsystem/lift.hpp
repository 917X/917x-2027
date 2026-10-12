#pragma once
#include "EZ-Template/piston.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"
#include "subsystem/intake.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves to and holds
// that height; RAW_UP / RAW_DOWN move freely and RAW_HOLD holds where they stopped.
class Lift {
 public:
  Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, ez::Piston& scoringPiston,
       Intake& intake, pros::Motor& clawIntake);
  enum LiftState { ZERO,
                   LOADING,
                   ROLLERS,
                   LEVEL_1,
                   LEVEL_2,
                   LEVEL_3,
                   LEVEL_4,
                   LEVEL_5,
                   RAW_UP,
                   RAW_DOWN,
                   RAW_HOLD,
                   HOMING,
                   SCORE };
  enum ScorePhase { DROPPING,
                    RAISING };

  void liftControl();
  void set(LiftState state);
  void levelUp();
  void levelDown();
  void home();   // starts HOMING - the lift task drives down until it stalls, then zeroes
  void score();  // starts SCORE - drop scoreDrop, then outtake while going back up to where it was

  pros::Motor& liftMotor;
  pros::Rotation& liftRotation;
  ez::Piston& scoringPiston;
  Intake& intake;  // SCORE runs these to outtake - opcontrol leaves them alone while it does
  pros::Motor& clawIntake;
  LiftState state = ZERO;
  LiftState level = ZERO;  // last LOADING/ROLLERS..LEVEL_5 picked, so levelUp/levelDown still work after raw

  int speed = 127;     // max power moving to a level
  int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
  double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

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

  // Score - drops scoreDrop degrees below where the lift is, then goes straight back up
  // outtaking (same as R2) for the first scoreOuttakeTime ms of the climb, and returns to
  // the state it started in.  Each move counts as done within scoreTolerance degrees, or
  // after scoreMoveTimeout ms so it can't get stuck.  Any other lift button cancels it.
  double scoreDrop = 250;       // degrees, same units as the heights below
  int scoreOuttakeTime = 500;   // ms
  double scoreTolerance = 30;   // degrees  (PLACEHOLDER)
  int scoreMoveTimeout = 1000;  // ms
  int scoreClawSpeed = 50;      // clawIntake power while outtaking - same as R2
  ScorePhase scorePhase = DROPPING;
  LiftState scoreReturn = ZERO;  // state to go back to when it's done
  double scoreStart = 0;         // where it was when score() was called
  int scoreTime = 0;             // ms in the current phase

  // Heights in rotation sensor degrees.  PLACEHOLDERS - tune on the robot
  /*
  double LOADING_HEIGHT = 750;
  double ROLLERS_HEIGHT = 2350;
  double LEVEL_1_HEIGHT = 2500;
  double LEVEL_2_HEIGHT = 4750;
  double LEVEL_3_HEIGHT = 6750;
  double LEVEL_4_HEIGHT = 8750;
  double LEVEL_5_HEIGHT = 10750;
  */
  double LOADING_HEIGHT = 1650;
  double ROLLERS_HEIGHT = 2770;
  double ZERO_HEIGHT = 0;
  double LEVEL_1_HEIGHT = 2950;
  double LEVEL_2_HEIGHT = 5200;
  double LEVEL_3_HEIGHT = 7200;
  double LEVEL_4_HEIGHT = 9200;
  double LEVEL_5_HEIGHT = 13000;
  // The lift never drives past these  (PLACEHOLDERS - tune on the robot)
  double MIN_POSITION = -1000000;
  double MAX_POSITION = 100000;  // was 1005000 (typo); set ~2× max level height as hard safety stop
};

// miguel phonk
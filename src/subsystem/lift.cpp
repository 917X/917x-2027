#include "subsystem/lift.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "pros/motors.h"
#include "pros/rtos.hpp"

Lift::Lift(pros::MotorGroup& liftMotors, pros::Rotation& liftRotation, pros::Motor& pivotMotor,
           pros::Rotation& pivotRotation, ez::Piston& claw)
    : liftMotors(liftMotors), liftRotation(liftRotation), pivotMotor(pivotMotor), pivotRotation(pivotRotation), claw(claw) {}

void Lift::liftControl() {
  liftMotors.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);  // _all - plain set_brake_mode() only sets the first motor
  pivotMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  liftRotation.reset_position();   // the lift starts sitting at ZERO_HEIGHT
  pivotRotation.reset_position();  // the arm's angles are measured from where it starts
  int count = 0;
  while (true) {
    pros::delay(10);
    double position = liftRotation.get_position() / 100.0;        // centidegrees -> degrees
    double pivotPosition = pivotRotation.get_position() / 100.0;  // centidegrees -> degrees
    if (count % 10 == 0) {
      std::cout << "Lift position: " << position << "  Level: " << state << "  Pivot position: " << pivotPosition << std::endl;
    }
    count++;
    // A motor that's unplugged when the brake mode is set never gets it, so keep setting it -
    // the second lift motor holds too if it's plugged in after the program starts
    if (count % 50 == 0) liftMotors.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);

    double target = position;  // raw states get no P power
    double levelAngle = isBackMode ? BACK_ANGLE : FRONT_LEVEL_ANGLE;
    // Where SCORE drops to before opening the claw.  driverScore() stops at ZERO_HEIGHT - it ends up there anyway
    double scoreLow = scoreToZero ? std::max(scoreStart - driverScoreDrop, ZERO_HEIGHT) : scoreStart - scoreDrop;
    bool matchloadPush = false;  // MATCHLOADING has swung the arm - see matchloadPushPower

    switch (state) {
      case ZERO:
        target = ZERO_HEIGHT;
        break;
      case MATCHLOADING:
        target = MATCHLOADING_HEIGHT;
        // The arm holds its last angle until the lift is below MATCHLOADING_PIVOT_HEIGHT, then swings
        if (position < MATCHLOADING_PIVOT_HEIGHT) {
          pivotTarget = FRONT_ANGLE;
          matchloadPush = true;
        }
        break;
      case FLOORPIN:
        target = FLOORPIN_HEIGHT;
        pivotTarget = FLOORPIN_ANGLE;
        break;
      case LOADING:
        target = LOADING_HEIGHT;
        pivotTarget = LOADING_ANGLE;
        break;
      case LEVEL_1:
        target = LEVEL_1_HEIGHT;
        pivotTarget = FRONT_LEVEL_ANGLE;
        break;
      case LEVEL_2:
        target = isBackMode ? BACK_LEVEL_2_HEIGHT : LEVEL_2_HEIGHT;
        pivotTarget = levelAngle;
        break;
      case LEVEL_3:
        target = isBackMode ? BACK_LEVEL_3_HEIGHT : LEVEL_3_HEIGHT;
        pivotTarget = levelAngle;
        break;
      case LEVEL_4:
        target = BACK_LEVEL_4_HEIGHT;
        pivotTarget = BACK_ANGLE;
        break;
      case LEVEL_5:
        target = BACK_LEVEL_5_HEIGHT;
        pivotTarget = BACK_ANGLE;
        break;
      case SCORE:
        target = scorePhase == RAISING ? scoreStart + scoreRise : scoreLow;
        break;
      case CUSTOM_HEIGHT:
        target = customHeight;  // the arm holds its last angle
        break;
      case RAW_UP:
      case RAW_DOWN:
      case RAW_HOLD:
      case HOMING:
        break;
    }
    // Front level from MATCHLOADING - the arm stays at FRONT_SCORING_ANGLE until the lift is at the level.
    // Only checked at a level, so a set() that hasn't changed the state yet doesn't clear it.
    if (scoringAngleHold && state >= LEVEL_1 && state <= LEVEL_5) {
      bool frontLevel = state == LEVEL_1 || ((state == LEVEL_2 || state == LEVEL_3) && !isBackMode);
      if (frontLevel && std::abs(position - target) > levelAngleTolerance) {
        pivotTarget = FRONT_SCORING_ANGLE;
      } else {
        scoringAngleHold = false;  // the lift's there (or it's a back level) - the arm goes to the level's angle
      }
    }
    if (pivotManual) pivotTarget = pivotManualTarget;  // setPivot() or manual hold wins over the state's angle

    int power;
    if (target - position > 0) {
      power = std::clamp(static_cast<int>(kP * (target - position)), -speed, speed);
    } else {
      power = std::clamp(static_cast<int>(kP * (target - position)), -lowerSpeed, lowerSpeed);
    }
    if (state == RAW_UP) power = rawSpeed;
    if (state == RAW_DOWN) power = -rawSpeed;
    if (state == RAW_HOLD) power = 0;  // let go - the HOLD brake keeps it where it stopped

    // Never drive past the ends of travel
    if (position >= MAX_POSITION && power > 0) power = 0;
    if (position <= MIN_POSITION && power < 0) power = 0;

    // Back scoring (back LEVEL_2-5) uses pivotLoadedKP - the arm's carrying the pin over to the back.  Checks level,
    // not state, so it stays on through SCORE, the raw states and manual pivot there.  Everything else uses pivotKP
    bool backScoring = level == LEVEL_4 || level == LEVEL_5 || ((level == LEVEL_2 || level == LEVEL_3) && isBackMode);
    double armKP = backScoring ? pivotLoadedKP : pivotKP;
    int pivotPower = std::clamp(static_cast<int>(armKP * (pivotTarget - pivotPosition)), -pivotSpeed, pivotSpeed);
    if (pivotPosition >= PIVOT_MAX_ANGLE && pivotPower > 0) pivotPower = 0;
    if (pivotPosition <= PIVOT_MIN_ANGLE && pivotPower < 0) pivotPower = 0;

    // At MATCHLOADING the arm swings to FRONT_ANGLE on raw matchloadPushPower - P is too weak near the end to get it
    // there.  Once it lands it keeps pushing into the hardstop for matchloadPushTime ms, then P holds FRONT_ANGLE.
    // Knocked more than matchloadPushTolerance short, it goes back to pushing.  After the limit check so
    // PIVOT_MAX_ANGLE can't cut it off.  Off once setPivot() or manual pivot takes the arm, e.g. claw_autoclose
    // raising it after a grab
    if (matchloadPush && !pivotManual) {
      bool landed = pivotPosition > FRONT_ANGLE - matchloadPushTolerance;  // past it counts too - that's the hardstop
      if (!landed) {
        matchloadHolding = false;  // not there yet, or knocked off - push it in
        matchloadPushLeft = matchloadPushTime;
      } else if (!matchloadHolding) {
        matchloadPushLeft -= 10;
        if (matchloadPushLeft <= 0) matchloadHolding = true;
      }
      if (!matchloadHolding) pivotPower = matchloadPushPower;
    } else {
      matchloadHolding = false;  // next time A swings the arm, it pushes from the start
      matchloadPushLeft = matchloadPushTime;
    }

    if (pivotRawPower != 0) {
      pivotPower = pivotRawPower;
      pivotManualTarget = pivotPosition;  // follows the arm while it moves, so it holds where you let go
      pivotManual = true;
    }

    pivotMotor.move(pivotPower);

    if (state == SCORE) {
      scoreTime += 10;

      if (scorePhase == DROPPING) {
        if (position <= scoreLow + scoreTolerance || scoreTime >= scoreMoveTimeout) {
          if (claw.get()) {
            scorePhase = RAISING;  // already open - nothing to wait for
          } else {
            claw.set(true);
            scorePhase = RELEASING;
          }
          scoreTime = 0;
        }
      } else if (scorePhase == RELEASING) {
        // Give the claw time to let go before pulling away
        if (scoreTime >= scoreReleaseTime) {
          scorePhase = RAISING;
          scoreTime = 0;
        }
      } else {
        // Done once it's up - hold right there, since going back to a level would just drop
        // it back down to that height
        if (position >= scoreStart + scoreRise - scoreTolerance || scoreTime >= scoreMoveTimeout)
          set(RAW_HOLD);
      }
      // driverScore() - it's let go, so go all the way down instead.  ZERO aims there.  Handing over on the
      // same frame it reaches RAISING means it never drives up
      if (scoreToZero && scorePhase == RAISING) set(ZERO);
    }

    if (state == HOMING) {
      power = -homeSpeed;
      homeTime += 10;

      int current = std::abs(liftMotors.get_current_draw());  // mA - the first motor (LIFT) only
      if (++homePrintCounter % 5 == 0)                        // every 50ms, for tuning
        printf("homing rpm: %.1f  current: %d mA  loose: %d  loadedTime: %d\n",
               fabs(liftMotors.get_actual_velocity()), current, homeLoose, loadedTime);

      // The current reads 0 before the motor starts and spikes while it speeds up, so
      // don't look at it until that's over
      if (homeTime > homeIgnoreTime) {
        // String's loose - from here on, a current jump means it's lifting the lift
        if (current < homeLooseCurrent) homeLoose = true;

        if (homeLoose && current > homeLoadedCurrent) {
          loadedTime += 10;
        } else {
          loadedTime = 0;
        }
      }

      if (loadedTime >= homeLoadedTime) {
        printf("homed\n");
        liftMotors.move(0);
        liftMotors.tare_position_all();
        liftRotation.reset_position();
        set(ZERO);
        continue;
      }
    }

    liftMotors.move(power);
  }
}

void Lift::set(LiftState state) {
  // A front level from MATCHLOADING keeps the arm at FRONT_SCORING_ANGLE until the lift gets there.  The lift
  // goes up right away.  Another level picked while the arm's still waiting keeps it waiting, for that level.
  bool toLevel = state >= LEVEL_1 && state <= LEVEL_5;
  scoringAngleHold = toLevel && (this->state == MATCHLOADING || scoringAngleHold);

  if (state <= LEVEL_5) {
    level = state;
    // LEVEL_1 only exists in front and LEVEL_4-5 only in back.  LEVEL_2-3 keep the current mode.
    // MATCHLOADING, FLOORPIN and LOADING leave it alone - nothing reads it until the next level button sets it,
    // and writing it here could let the lift task see "front LEVEL_3" for a frame and swing the arm
    // early when A is pressed at back LEVEL_2-3.
    if (state == LEVEL_1) {
      isBackMode = false;
    } else if (state >= LEVEL_4) {
      isBackMode = true;
    }
  }
  pivotManual = false;        // a new lift state picks the arm angle again
  matchloadHolding = false;   // pressing A again pushes the arm in again, even if it's already there
  matchloadPushLeft = matchloadPushTime;
  this->state = state;
}

void Lift::setLevel(LiftState level, bool back) {
  // Only LEVEL_2-3 have two sides.  set() picks the side for LEVEL_1 and LEVEL_4-5, and the other
  // states don't use it - leaving it alone there means the lift never aims at the wrong side for a frame.
  if (level == LEVEL_2 || level == LEVEL_3) isBackMode = back;
  set(level);
}

void Lift::setHeight(double height) {
  // Set the height before the state, so the lift task never sees CUSTOM_HEIGHT with the old height.
  // Clamped because the lift stops at the ends of travel anyway, and targetHeight() should match where it ends up.
  customHeight = std::clamp(height, MIN_POSITION, MAX_POSITION);
  set(CUSTOM_HEIGHT);
}

bool Lift::waitUntilCompletion(int timeout, bool waitForArm) {
  // The lift task works out the arm angle for a new state on its next frame.  Waiting 20ms first means it
  // has run at least once since the last set(), so the check below never uses the old state's angle.
  pros::delay(20);
  for (int waited = 20;; waited += 10) {
    double position = liftRotation.get_position() / 100.0;
    double arm = pivotRotation.get_position() / 100.0;
    // These end on their own - wait for them to finish.  scoringAngleHold means the arm hasn't gone to the level's
    // angle yet - only matters when waiting for the arm.  Read before pivotTarget, so once it's false pivotTarget
    // is already the level's angle.
    bool running = state == SCORE || state == HOMING || (waitForArm && scoringAngleHold);
    // Clamped like the lift task does, so an angle past the arm's limits can't make this wait forever
    double armTarget = std::clamp(pivotManual ? pivotManualTarget : pivotTarget, PIVOT_MIN_ANGLE, PIVOT_MAX_ANGLE);
    bool armThere = !waitForArm || std::abs(arm - armTarget) <= pivotWaitTolerance;
    if (!running && std::abs(position - targetHeight()) <= waitTolerance && armThere)
      return true;
    if (waited >= timeout) {
      printf("lift wait timed out - lift %.0f (wants %.0f)  arm %.0f (wants %.0f)  state %d\n",
             position, targetHeight(), arm, armTarget, state);
      return false;
    }
    pros::delay(10);
  }
}

void Lift::setPivot(double angle) {
  pivotManualTarget = angle;
  pivotManual = true;
}

double Lift::levelHeight(LiftState level, bool back) {
  switch (level) {
    case LEVEL_1:
      return LEVEL_1_HEIGHT;
    case LEVEL_2:
      return back ? BACK_LEVEL_2_HEIGHT : LEVEL_2_HEIGHT;
    case LEVEL_3:
      return back ? BACK_LEVEL_3_HEIGHT : LEVEL_3_HEIGHT;
    case LEVEL_4:
      return BACK_LEVEL_4_HEIGHT;
    case LEVEL_5:
      return BACK_LEVEL_5_HEIGHT;
    case MATCHLOADING:
      return MATCHLOADING_HEIGHT;
    case FLOORPIN:
      return FLOORPIN_HEIGHT;
    case LOADING:
      return LOADING_HEIGHT;
    default:
      return ZERO_HEIGHT;
  }
}

double Lift::targetHeight() {
  if (state <= LEVEL_5) return levelHeight(state, isBackMode);
  if (state == CUSTOM_HEIGHT) return customHeight;
  return liftRotation.get_position() / 100.0;  // raw, score and homing - use where it is now
}

void Lift::toggleFrontBack() {
  if (state == HOMING || state == SCORE) return;  // don't cut either one short

  // Keeps the same level when the other side has it.  Front is LEVEL_1-3, back is LEVEL_2-5.
  if (level <= LEVEL_1) {
    // Zero, MATCHLOADING, FLOORPIN, LOADING or LEVEL_1 - none of these are on the back, so go to back LEVEL_2
    isBackMode = true;
    set(LEVEL_2);
  } else {
    isBackMode = !isBackMode;
    if (!isBackMode && level >= LEVEL_4) {
      set(LEVEL_3);  // LEVEL_4-5 aren't on the front - go to the highest front level
    } else {
      set(level);  // LEVEL_2 or LEVEL_3 - same level, other side
    }
  }
}

void Lift::levelUp() {
  // MATCHLOADING, FLOORPIN and LOADING are picked on their own - up from any of them is LEVEL_1
  LiftState next = level < LEVEL_1 ? LEVEL_1 : static_cast<LiftState>(level + 1);
  if (next > LEVEL_5) return;
  // Only LEVEL_1 is on the front - every other level the buttons reach is on the back.
  // The toggle is the only way to the front LEVEL_2-3.
  if (next >= LEVEL_2) isBackMode = true;
  set(next);  // set() puts LEVEL_1 on the front
}

void Lift::levelDown() {
  if (level <= LOADING) return;
  LiftState next = static_cast<LiftState>(level - 1);
  if (next >= LEVEL_2) isBackMode = true;  // same as levelUp - only LEVEL_1 is on the front
  set(next);                               // set() puts LEVEL_1 and LOADING on the front
}

void Lift::home() {
  if (state == HOMING) return;  // already homing - holding the buttons shouldn't restart it
  homeTime = 0;
  homeLoose = false;
  loadedTime = 0;
  homePrintCounter = 0;
  set(HOMING);
}

void Lift::score() { startScore(false); }

void Lift::driverScore() { startScore(true); }

void Lift::startScore(bool toZero) {
  if (state == SCORE || state == HOMING) return;  // don't stack drops or cut homing short
  // At zero there's no room to drop, so just open the claw and leave the lift where it is
  if (targetHeight() <= ZERO_HEIGHT + scoreTolerance) {
    claw.set(true);
    return;
  }
  scoreStart = liftRotation.get_position() / 100.0;  // centidegrees -> degrees
  scorePhase = DROPPING;
  scoreTime = 0;
  scoreToZero = toZero;
  set(SCORE);
}
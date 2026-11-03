#include "subsystem/lift.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "pros/motors.h"
#include "pros/rtos.hpp"

Lift::Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, pros::Motor& pivotMotor,
           pros::Rotation& pivotRotation, ez::Piston& claw)
    : liftMotor(liftMotor), liftRotation(liftRotation), pivotMotor(pivotMotor),
      pivotRotation(pivotRotation), claw(claw) {}

void Lift::liftControl() {
  liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  pivotMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  liftRotation.reset_position();   // the lift starts sitting at ZERO_HEIGHT
  pivotRotation.reset_position();  // the arm's angles are measured from where it starts

  while (true) {
    pros::delay(10);
    double position = liftRotation.get_position() / 100.0;        // centidegrees -> degrees
    double pivotPosition = pivotRotation.get_position() / 100.0;  // centidegrees -> degrees

    double target = position;  // raw states get no P power
    switch (state) {
      case ZERO:
        target = ZERO_HEIGHT;
        break;
      case MATCHLOADING:
        target = MATCHLOADING_HEIGHT;
        pivotTarget = MATCHLOADING_ANGLE;
        break;
      case LOADING:
        target = LOADING_HEIGHT;
        pivotTarget = LOADING_ANGLE;
        break;
      case LEVEL_1:
        target = LEVEL_1_HEIGHT;
        pivotTarget = SCORING_ANGLE;
        break;
      case LEVEL_2:
        target = LEVEL_2_HEIGHT;
        pivotTarget = SCORING_ANGLE;
        break;
      case LEVEL_3:
        target = LEVEL_3_HEIGHT;
        pivotTarget = SCORING_ANGLE;
        break;
      case LEVEL_4:
        target = LEVEL_4_HEIGHT;
        pivotTarget = SCORING_ANGLE;
        break;
      case LEVEL_5:
        target = LEVEL_5_HEIGHT;
        pivotTarget = SCORING_ANGLE;
        break;
      case SCORE:
        target = scorePhase == RAISING ? scoreStart + scoreRise : scoreStart - scoreDrop;
        break;
      case RAW_UP:
      case RAW_DOWN:
      case RAW_HOLD:
      case HOMING:
        break;
    }
    int power = std::clamp(static_cast<int>(kP * (target - position)), -speed, speed);
    if (state == RAW_UP) power = rawSpeed;
    if (state == RAW_DOWN) power = -rawSpeed;
    if (state == RAW_HOLD) power = 0;  // let go - the HOLD brake keeps it where it stopped

    // Never drive past the ends of travel
    if (position >= MAX_POSITION && power > 0) power = 0;
    if (position <= MIN_POSITION && power < 0) power = 0;

    int pivotPower = std::clamp(static_cast<int>(pivotKP * (pivotTarget - pivotPosition)), -pivotSpeed, pivotSpeed);
    if (pivotPosition >= PIVOT_MAX_ANGLE && pivotPower > 0) pivotPower = 0;
    if (pivotPosition <= PIVOT_MIN_ANGLE && pivotPower < 0) pivotPower = 0;
    pivotMotor.move(pivotPower);

    if (state == SCORE) {
      scoreTime += 10;

      if (scorePhase == DROPPING) {
        if (position <= scoreStart - scoreDrop + scoreTolerance || scoreTime >= scoreMoveTimeout) {
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
    }

    if (state == HOMING) {
      power = -homeSpeed;
      homeTime += 10;

      int current = std::abs(liftMotor.get_current_draw());  // mA
      if (++homePrintCounter % 5 == 0)  // every 50ms, for tuning
        printf("homing rpm: %.1f  current: %d mA  loose: %d  loadedTime: %d\n",
               fabs(liftMotor.get_actual_velocity()), current, homeLoose, loadedTime);

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
        liftMotor.move(0);
        liftMotor.tare_position();
        liftRotation.reset_position();
        set(ZERO);
        continue;
      }
    }

    liftMotor.move(power);
  }
}

void Lift::set(LiftState state) {
  if (state <= LEVEL_5) level = state;
  this->state = state;
}

void Lift::levelUp() {
  // MATCHLOADING and LOADING are picked with their own buttons - up from either is LEVEL_1
  LiftState next = level < LEVEL_1 ? LEVEL_1 : static_cast<LiftState>(level + 1);
  if (next <= LEVEL_5) set(next);
}

void Lift::levelDown() {
  if (level > LOADING) set(static_cast<LiftState>(level - 1));
}


void Lift::home() {
  if (state == HOMING) return;  // already homing - holding the buttons shouldn't restart it
  homeTime = 0;
  homeLoose = false;
  loadedTime = 0;
  homePrintCounter = 0;
  set(HOMING);
}

void Lift::score() {
  if (state == SCORE || state == HOMING) return;  // don't stack drops or cut homing short
  scoreStart = liftRotation.get_position() / 100.0;  // centidegrees -> degrees
  scorePhase = DROPPING;
  scoreTime = 0;
  set(SCORE);
}

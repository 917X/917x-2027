#include "subsystem/lift.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "pros/motors.h"
#include "pros/rtos.hpp"

Lift::Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, ez::Piston& scoringPiston,
           Intake& intake, pros::Motor& clawIntake)
    : liftMotor(liftMotor), liftRotation(liftRotation), scoringPiston(scoringPiston),
      intake(intake), clawIntake(clawIntake) {}

void Lift::liftControl() {
  liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  liftRotation.reset_position();  // the lift starts sitting at LOADING

  while (true) {
    pros::delay(10);
    double position = liftRotation.get_position() / 100.0;  // centidegrees -> degrees

    double target = position;  // raw states get no P power
    switch (state) {
      case ZERO:
        target = ZERO_HEIGHT;
        break;
      case LOADING:
        target = LOADING_HEIGHT;
        break;
      case ROLLERS:
        target = ROLLERS_HEIGHT;
        break;
      case LEVEL_1:
        target = LEVEL_1_HEIGHT;
        break;
      case LEVEL_2:
        target = LEVEL_2_HEIGHT;
        break;
      case LEVEL_3:
        target = LEVEL_3_HEIGHT;
        break;
      case LEVEL_4:
        target = LEVEL_4_HEIGHT;
        break;
      case LEVEL_5:
        target = LEVEL_5_HEIGHT;
        break;
      case SCORE:
        target = scorePhase == RAISING ? scoreStart : scoreStart - scoreDrop;
        break;
      case MATCHLOADING:
        target = MATCHLOADING_HEIGHT;
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

    if (state == SCORE) {
      scoreTime += 10;

      // Outtake for the first scoreOuttakeTime ms of the way back up - stopped on the way down
      bool outtaking = scorePhase == RAISING && scoreTime < scoreOuttakeTime;
      intake.set(outtaking ? Intake::OUTTAKE : Intake::STOP);
      clawIntake.move(outtaking ? scoreClawSpeed : 0);

      if (scorePhase == DROPPING) {
        if (position <= scoreStart - scoreDrop + scoreTolerance || scoreTime >= scoreMoveTimeout) {
          scorePhase = RAISING;
          scoreTime = 0;
        }
      } else {
        // Done once it's back up and the outtake has run its full time
        bool raised = position >= scoreStart - scoreTolerance || scoreTime >= scoreMoveTimeout;
        if (raised && scoreTime >= scoreOuttakeTime)
          state = scoreReturn;  // straight back, not through set(), so the piston isn't touched
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
  // scoringPiston goes off heading to LOADING and back on leaving it - otherwise it's the driver's.
  // Mid-SCORE it's still really at the state SCORE will return to.
  LiftState from = this->state == SCORE ? scoreReturn : this->state;
  if (state == LOADING) scoringPiston.set(false);
  else if (from == LOADING) scoringPiston.set(true);

  if (state <= LEVEL_5) level = state;
  this->state = state;
}

void Lift::levelUp() {
  LiftState next = static_cast<LiftState>(level + 1);
  if (next == ROLLERS) next = static_cast<LiftState>(next + 1);  // ROLLERS only via A
  if (next <= LEVEL_5) set(next);
}

void Lift::levelDown() {
  LiftState next = static_cast<LiftState>(level - 1);
  if (next == ROLLERS) next = static_cast<LiftState>(next - 1);  // ROLLERS only via A
  if (next >= LOADING) set(next);
}


void Lift::home() {
  if (state == HOMING) return;  // already homing - holding the buttons shouldn't restart it
  homeTime = 0;
  homeLoose = false;
  loadedTime = 0;
  homePrintCounter = 0;
  set(HOMING);  // through set() so leaving LOADING still turns the piston on
}

void Lift::score() {
  if (state == SCORE || state == HOMING) return;  // don't stack drops or cut homing short
  scoreReturn = state;
  scoreStart = liftRotation.get_position() / 100.0;  // centidegrees -> degrees
  scorePhase = DROPPING;
  scoreTime = 0;
  state = SCORE;  // not through set() - it comes back to scoreReturn, so the piston stays put
}

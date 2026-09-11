#include "subsystem/lift.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"

Lift::Lift(pros::MotorGroup& motors, ez::Piston& claw, ez::Piston& pivot)
    : motors(motors), claw(claw), pivot(pivot) {
    this->holdPower = 0;
}

void Lift::initialize() {
    motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void Lift::moveFor(int power, int duration_ms) {
    motors.move(power);
    pros::delay(duration_ms);
    holdPower = power > 0 ? HOLD_POWER : 0;
    motors.move(holdPower);
}

void Lift::manual(int power) {
    motors.move(power);
}

void Lift::raiseFull() {
    moveFor(RAISE_POWER, MACRO_TIME_MS);
}

void Lift::lowerFull() {
    moveFor(LOWER_POWER, MACRO_TIME_MS * 2);
}

void Lift::raiseClearance() {
    moveFor(RAISE_POWER, CLEARANCE_TIME_MS);
}

void Lift::lowerClearance() {
    moveFor(LOWER_POWER, CLEARANCE_TIME_MS * 2);
}

void Lift::setPivot(bool state) {
    pivot.set(state);
    pros::delay(PNEUMATIC_DELAY_MS);
}

void Lift::togglePivot() {
    pivot.set(!pivot.get());
}

void Lift::openClaw() {
    claw.set(true);
    pros::delay(PNEUMATIC_DELAY_MS);
}

void Lift::closeClaw() {
    claw.set(false);
    pros::delay(PNEUMATIC_DELAY_MS);
}

void Lift::toggleClaw() {
    claw.set(!claw.get());
}

void Lift::intake() {
    raiseClearance();
    setPivot(PIVOT_INTAKE_STATE);
    openClaw();
}

void Lift::matchload() {
    raiseClearance();
    setPivot(PIVOT_STARTING_STATE);
    openClaw();
}

void Lift::raise() {
    closeClaw();
    setPivot(PIVOT_STARTING_STATE);
    raiseFull();
}

void Lift::lower() {
    setPivot(PIVOT_STARTING_STATE);
    lowerFull();
}

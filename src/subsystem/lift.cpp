#include "subsystem/lift.hpp"
#include <mutex>
#include "pros/motors.h"
#include "pros/rtos.hpp"

// Macro scripts.  These reproduce the original blocking sequences step for step.
namespace {

using Step = Lift::Step;

// clear, pivot out, open claw
const Step INTAKE_SCRIPT[] = {
    {Step::MOVE, Lift::RAISE_POWER, Lift::CLEARANCE_TIME_MS},
    {Step::PIVOT, Lift::PIVOT_INTAKE_STATE, Lift::PNEUMATIC_DELAY_MS},
    {Step::CLAW, true, Lift::PNEUMATIC_DELAY_MS},
};

// clear, pivot home, open claw
const Step MATCHLOAD_SCRIPT[] = {
    {Step::MOVE, Lift::RAISE_POWER, Lift::CLEARANCE_TIME_MS},
    {Step::PIVOT, Lift::PIVOT_STARTING_STATE, Lift::PNEUMATIC_DELAY_MS},
    {Step::CLAW, true, Lift::PNEUMATIC_DELAY_MS},
};

// grab, pivot home, lift up
const Step RAISE_SCRIPT[] = {
    {Step::CLAW, false, Lift::PNEUMATIC_DELAY_MS},
    {Step::PIVOT, Lift::PIVOT_STARTING_STATE, Lift::PNEUMATIC_DELAY_MS},
    {Step::MOVE, Lift::RAISE_POWER, Lift::MACRO_TIME_MS},
};

// pivot home, lift down
const Step LOWER_SCRIPT[] = {
    {Step::PIVOT, Lift::PIVOT_STARTING_STATE, Lift::PNEUMATIC_DELAY_MS},
    {Step::MOVE, Lift::LOWER_POWER, Lift::MACRO_TIME_MS * 2},
};

const Step RAISE_FULL_SCRIPT[] = {{Step::MOVE, Lift::RAISE_POWER, Lift::MACRO_TIME_MS}};
const Step LOWER_FULL_SCRIPT[] = {{Step::MOVE, Lift::LOWER_POWER, Lift::MACRO_TIME_MS * 2}};
const Step RAISE_CLEARANCE_SCRIPT[] = {{Step::MOVE, Lift::RAISE_POWER, Lift::CLEARANCE_TIME_MS}};
const Step LOWER_CLEARANCE_SCRIPT[] = {{Step::MOVE, Lift::LOWER_POWER, Lift::CLEARANCE_TIME_MS * 2}};

}  // namespace

const Lift::Step* Lift::scriptFor(Macro macro, int& length) {
#define LIFT_SCRIPT(arr)                              \
    length = static_cast<int>(sizeof(arr) / sizeof(arr[0])); \
    return arr;

    switch (macro) {
        case INTAKE: LIFT_SCRIPT(INTAKE_SCRIPT)
        case MATCHLOAD: LIFT_SCRIPT(MATCHLOAD_SCRIPT)
        case RAISE: LIFT_SCRIPT(RAISE_SCRIPT)
        case LOWER: LIFT_SCRIPT(LOWER_SCRIPT)
        case RAISE_FULL: LIFT_SCRIPT(RAISE_FULL_SCRIPT)
        case LOWER_FULL: LIFT_SCRIPT(LOWER_FULL_SCRIPT)
        case RAISE_CLEARANCE: LIFT_SCRIPT(RAISE_CLEARANCE_SCRIPT)
        case LOWER_CLEARANCE: LIFT_SCRIPT(LOWER_CLEARANCE_SCRIPT)
        case NONE:
        default:
            length = 0;
            return nullptr;
    }
#undef LIFT_SCRIPT
}

Lift::Lift(pros::MotorGroup& motors, ez::Piston& claw, ez::Piston& pivot)
    : motors(motors), claw(claw), pivot(pivot) {
    this->holdPower = 0;
}

void Lift::initialize() {
    motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

void Lift::liftControl() {
    while (true) {
        update();
        pros::delay(UPDATE_DELAY_MS);
    }
}

/**
 * Advances the running script.  Everything that writes to the lift on behalf of
 * a macro happens here, so a macro and the driver never write in the same
 * breath - manual() cancels first.
 *
 * The loop means a step boundary doesn't cost an extra tick: when one step's
 * wait is up, the next one starts in the same call.
 */
void Lift::update() {
    std::lock_guard<pros::Mutex> lock(mutex);

    while (active != NONE && script != nullptr) {
        const Step& current = script[stepIndex];

        if (!stepStarted) {
            switch (current.kind) {
                case Step::MOVE: motors.move(current.value); break;
                case Step::PIVOT: pivot.set(current.value != 0); break;
                case Step::CLAW: claw.set(current.value != 0); break;
            }
            stepStart = pros::millis();
            stepStarted = true;
        }

        // Still waiting out this step
        if (pros::millis() - stepStart < static_cast<std::uint32_t>(current.duration_ms)) return;

        // Step is done.  A lift move falls back to hold power, same as before.
        if (current.kind == Step::MOVE) {
            holdPower = current.value > 0 ? HOLD_POWER : 0;
            motors.move(holdPower);
        }

        stepIndex++;
        stepStarted = false;

        if (stepIndex >= scriptLength) {
            active = NONE;
            script = nullptr;
            scriptLength = 0;
            stepIndex = 0;
            return;
        }
    }
}

void Lift::run(Macro macro) {
    if (macro == NONE) {
        cancel();
        return;
    }

    std::lock_guard<pros::Mutex> lock(mutex);
    script = scriptFor(macro, scriptLength);
    active = script != nullptr ? macro : NONE;
    stepIndex = 0;
    stepStarted = false;
}

void Lift::cancel() {
    bool wasRunning = false;
    {
        std::lock_guard<pros::Mutex> lock(mutex);
        wasRunning = active != NONE;
        active = NONE;
        script = nullptr;
        scriptLength = 0;
        stepIndex = 0;
        stepStarted = false;
    }

    // Don't leave the lift driving after an aborted move.  Outside the lock so
    // we're not holding it across a device write.
    if (wasRunning) motors.move(0);
}

bool Lift::busy() {
    std::lock_guard<pros::Mutex> lock(mutex);
    return active != NONE;
}

void Lift::waitUntilDone(int timeout_ms) {
    std::uint32_t start = pros::millis();
    while (busy()) {
        if (timeout_ms > 0 && pros::millis() - start >= static_cast<std::uint32_t>(timeout_ms)) {
            cancel();
            return;
        }
        pros::delay(UPDATE_DELAY_MS);
    }
}

void Lift::manual(int power) {
    if (power != 0) {
        cancel();  // driver override beats a running macro
        motors.move(power);
    } else if (!busy()) {
        motors.move(0);
    }
    // busy and power == 0: leave the macro alone
}

void Lift::intake() { run(INTAKE); }
void Lift::matchload() { run(MATCHLOAD); }
void Lift::raise() { run(RAISE); }
void Lift::lower() { run(LOWER); }
void Lift::raiseFull() { run(RAISE_FULL); }
void Lift::lowerFull() { run(LOWER_FULL); }
void Lift::raiseClearance() { run(RAISE_CLEARANCE); }
void Lift::lowerClearance() { run(LOWER_CLEARANCE); }

void Lift::setPivot(bool state) { pivot.set(state); }
void Lift::togglePivot() { pivot.set(!pivot.get()); }
void Lift::setClaw(bool state) { claw.set(state); }
void Lift::toggleClaw() { claw.set(!claw.get()); }
void Lift::openClaw() { setClaw(true); }
void Lift::closeClaw() { setClaw(false); }

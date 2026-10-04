#include "subsystem/lift.hpp"
#include <algorithm>
#include "pros/motors.h"
#include "pros/rtos.hpp"

Lift::Lift(pros::Motor& liftMotor, pros::Rotation& liftRotation, ez::Piston& scoringPiston)
    : liftMotor(liftMotor), liftRotation(liftRotation), scoringPiston(scoringPiston) {}

void Lift::liftControl() {
    liftMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    liftRotation.reset_position();  // the lift starts sitting at LOADING

    while (true) {
        pros::delay(10);

        double position = liftRotation.get_position() / 100.0;  // centidegrees -> degrees

        if (pistonPending && pros::millis() - pistonStart >= SCORING_PISTON_DELAY) {
            scoringPiston.set(true);
            pistonPending = false;
        }

        double target = position;  // raw states get no P power
        switch (state) {
            case LOADING:
                scoringPiston.set(false);
                target = LOADING_HEIGHT;
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
            case RAW_UP:
            case RAW_DOWN:
                rawTarget = position;  // RAW_HOLD holds wherever the button was let go
                break;
            case RAW_HOLD:
                target = rawTarget;
                break;
        }

        int power = std::clamp(static_cast<int>(kP * (target - position)), -speed, speed);
        if (state == RAW_UP) power = rawSpeed;
        if (state == RAW_DOWN) power = -rawSpeed;

        // Never drive past the ends of travel
        if (position >= MAX_POSITION && power > 0) power = 0;
        if (position <= MIN_POSITION && power < 0) power = 0;

        liftMotor.move(power);
    }
}

void Lift::set(LiftState state) {
    // The scoring piston fires on the first move up out of LOADING.  Only LOADING
    // retracts it, so "still retracted" means we're coming up from loading - even
    // if the driver tapped raw down at the bottom first.
    bool goingUp = (state >= LEVEL_1 && state <= LEVEL_5) || state == RAW_UP;
    if (goingUp && !scoringPiston.get() && !pistonPending) {
        pistonStart = pros::millis();  // set before pistonPending so liftControl never sees a stale start
        pistonPending = true;
    }
    if (state == LOADING) pistonPending = false;
    if (state <= LEVEL_5) level = state;
    this->state = state;
}

void Lift::levelUp() {
    if (level < LEVEL_5) set(static_cast<LiftState>(level + 1));
}

void Lift::levelDown() {
    if (level > LEVEL_1) set(static_cast<LiftState>(level - 1));
}

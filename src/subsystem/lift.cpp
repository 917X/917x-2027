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

        // On once we're up and away from loading, off once we're back down near it
        if (position > PISTON_ON_POSITION) scoringPiston.set(true);
        if (position < PISTON_OFF_POSITION) scoringPiston.set(false);

        double target = position;  // raw states get no P power
        switch (state) {
            case LOADING:
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
    if (state <= LEVEL_5) level = state;
    this->state = state;
}

void Lift::levelUp() {
    if (level < LEVEL_5) set(static_cast<LiftState>(level + 1));
}

void Lift::levelDown() {
    if (level > LEVEL_1) set(static_cast<LiftState>(level - 1));
}

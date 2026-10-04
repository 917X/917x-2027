#pragma once
#include <cstdint>
#include "EZ-Template/piston.hpp"
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"

// Lift state machine, same idea as Intake.  lift.set(Lift::LEVEL_3) moves to and holds
// that height; RAW_UP / RAW_DOWN move freely and RAW_HOLD holds where they stopped.
class Lift {
    public:

        Lift(pros::MotorGroup& liftMotors, pros::Rotation& liftRotation, ez::Piston& scoringPiston);
        enum LiftState{ LOADING, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5, RAW_UP, RAW_DOWN, RAW_HOLD };

        void liftControl();
        void set(LiftState state);
        void levelUp();
        void levelDown();

        pros::MotorGroup& liftMotors;
        pros::Rotation& liftRotation;
        ez::Piston& scoringPiston;
        LiftState state = LOADING;
        LiftState level = LOADING;  // last LOADING..LEVEL_5 picked, so levelUp/levelDown still work after raw

        int speed = 127;     // max power moving to a level
        int rawSpeed = 127;  // power for RAW_UP / RAW_DOWN - lower it to make raw slower
        double kP = 1.0;     // power per degree of error  (PLACEHOLDER)

        // Heights in rotation sensor degrees.  PLACEHOLDERS - tune on the robot
        double LOADING_HEIGHT = 0;
        double LEVEL_1_HEIGHT = 100;
        double LEVEL_2_HEIGHT = 200;
        double LEVEL_3_HEIGHT = 300;
        double LEVEL_4_HEIGHT = 400;
        double LEVEL_5_HEIGHT = 500;

        // The lift never drives past these  (PLACEHOLDERS)
        double MIN_POSITION = 0;
        double MAX_POSITION = 550;

        std::uint32_t SCORING_PISTON_DELAY = 500;  // ms after leaving LOADING before scoringPiston fires
        bool pistonPending = false;
        std::uint32_t pistonStart = 0;

        double rawTarget = 0;  // where RAW_HOLD holds - the position when the raw button was let go
};

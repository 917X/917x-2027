#pragma once
#include "pros/motors.hpp"

class Intake {
    public:

        Intake(pros::Motor& intakeMotor);
        enum IntakeState{ INTAKE, OUTTAKE, STOP };

        void intakeControl();
        void set(IntakeState state, int speed = 127);

        pros::Motor& intakeMotor;
        IntakeState state = STOP;
        int speed = 127;
};

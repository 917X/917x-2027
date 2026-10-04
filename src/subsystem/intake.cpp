#include "subsystem/intake.hpp"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"

Intake::Intake(pros::Motor& intakeMotor)
    : intakeMotor(intakeMotor) {}

void Intake::intakeControl() {
    while (true) {
        pros::delay(10);

        switch (state) {
            case INTAKE:
                intakeMotor.move(speed);
                break;
            case OUTTAKE:
                intakeMotor.move(-speed);
                break;
            case STOP:
                intakeMotor.move(0);
                break;
        }
    }
}

void Intake::set(IntakeState state, int speed) {
    this->state = state;
    this->speed = speed;
}

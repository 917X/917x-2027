#include "config.hpp"

#include "pros/adi.h"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// controller - shared by both robots below
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// ports
constexpr int LEFT_F = 13;
constexpr int LEFT_M = 15;
constexpr int LEFT_B = -16;

constexpr int RIGHT_F = -11;
constexpr int RIGHT_M = -12;
constexpr int RIGHT_B = 14;

constexpr int IMU = 7;

constexpr int INTAKE = -1;
constexpr int CLAW_INTAKE = 10;

constexpr int LIFT = 20;           // was LIFT_L when the lift had two motors - check this is the one left
constexpr int LIFT_ROTATION = 18;  // PLACEHOLDER - set to the real port (negative reverses it)

constexpr char CLAW = 'B';
constexpr char CLAW_PIVOT = 'A';
constexpr char SCORING_PISTON = 'C';  // PLACEHOLDER

// drivetrain geometry
constexpr double WHEEL_DIAMETER = 2.75;  // 4" wheels without screw holes are actually 4.125
constexpr double WHEEL_RPM = 450;        // cartridge * (motor gear / wheel gear)

// intake
pros::Motor intakeMotor(INTAKE);
Intake intake(intakeMotor);
pros::Motor clawIntake(CLAW_INTAKE);

// lift + scoring piston
pros::Motor liftMotor(LIFT);
pros::Rotation liftRotation(LIFT_ROTATION);
ez::Piston scoringPiston(SCORING_PISTON);

Lift lift(liftMotor, liftRotation, scoringPiston);

// claw - not part of the Lift, opcontrol toggles these directly
ez::Piston claw(CLAW);
ez::Piston clawPivot(CLAW_PIVOT);

// drivetrain
//  - the first motor in each side is the one used for sensing
//  - built last so its gearing wins on the ports shared with leftMotors above
ez::Drive chassis(
    {LEFT_F, LEFT_M, LEFT_B},
    {RIGHT_F, RIGHT_M, RIGHT_B},
    IMU,
    WHEEL_DIAMETER,
    WHEEL_RPM);

// Uncomment the trackers you're using here!
// - `8` and `9` are smart ports (making these negative will reverse the sensor)
//  - you should get positive values on the encoders going FORWARD and RIGHT
// - `2.75` is the wheel diameter
// - `4.0` is the distance from the center of the wheel to the center of the robot
// ez::tracking_wheel horiz_tracker(8, 2.75, 4.0);  // This tracking wheel is perpendicular to the drive wheels
// ez::tracking_wheel vert_tracker(9, 2.75, 4.0);   // This tracking wheel is parallel to the drive wheels

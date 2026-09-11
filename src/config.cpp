#include "config.hpp"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"

// ports
constexpr int LEFT_F = 13;
constexpr int LEFT_M = 15;
constexpr int LEFT_B = -16;

constexpr int RIGHT_F = -11;
constexpr int RIGHT_M = -12;
constexpr int RIGHT_B = 14;

constexpr int IMU = 7;

constexpr int INTAKE = -1;

constexpr int LIFT_L = -5;
constexpr int LIFT_R = 20;

constexpr char CLAW = 'B';
constexpr char CLAW_PIVOT = 'A';

// drivetrain geometry
constexpr double WHEEL_DIAMETER = 2.75;  // 4" wheels without screw holes are actually 4.125
constexpr double WHEEL_RPM = 450;        // cartridge * (motor gear / wheel gear)

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// drivetrain
//  - the first motor in each side is the one used for sensing
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

// intake
pros::Motor intakeMotor(INTAKE);

// lift + claw
pros::MotorGroup liftMotors({LIFT_L, LIFT_R});
ez::Piston claw(CLAW);
ez::Piston clawPivot(CLAW_PIVOT);

Lift lift(liftMotors, claw, clawPivot);

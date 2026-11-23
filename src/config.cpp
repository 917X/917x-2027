#include "config.hpp"

#include "pros/adi.h"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/rotation.hpp"

// controller - shared by both robots below
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// ports
constexpr int RIGHT_F = -8;
constexpr int RIGHT_M = -9;
constexpr int RIGHT_B = 20;

constexpr int LEFT_F = 7;
constexpr int LEFT_M = 6;
constexpr int LEFT_B = -5;

constexpr int IMU = 10;

constexpr int INTAKE = -19;

constexpr int LIFT = -18;  // was LIFT_L when the lift had two motors - check this is the one left
constexpr int LIFT_2 = 2;  // second lift motor, only plugged in sometimes.  If it fights LIFT, make it -2
constexpr int LIFT_ROTATION = 17;

constexpr int TOGGLE = 3;          // PLACEHOLDER

constexpr int PIVOT = -11;           // PLACEHOLDER
constexpr int PIVOT_ROTATION = 15;  // PLACEHOLDER

constexpr char CLAW = 'H';
constexpr int CLAW_DISTANCE = 12;

// distance sensors for DistReset - no back sensor yet
constexpr int LEFT_DIST = 13;
constexpr int RIGHT_DIST = 14;
constexpr int FRONT_DIST = 4;

// inches from the center of rotation to each distance sensor
constexpr double LEFT_DIST_OFFSET = 5.5;
constexpr double RIGHT_DIST_OFFSET = 5.125;
constexpr double FRONT_DIST_OFFSET = 4.75;

// drivetrain geometry
constexpr double WHEEL_DIAMETER = 2.75;  // 4" wheels without screw holes are actually 4.125
constexpr double WHEEL_RPM = 450;        // cartridge * (motor gear / wheel gear)

// intake
pros::Motor intakeMotor(INTAKE);
Intake intake(intakeMotor);
// lift + pivoting arm
// LIFT goes first - it's always plugged in, and get_current_draw() / get_actual_velocity() read the first motor.
// move() sends to every port and skips one that isn't there, so the lift still runs on LIFT alone.
pros::MotorGroup liftMotors({LIFT, LIFT_2});
pros::Rotation liftRotation(LIFT_ROTATION);
pros::Motor pivotMotor(PIVOT);
pros::Rotation pivotRotation(PIVOT_ROTATION);

pros::Motor toggleMotor(TOGGLE);
pros::Distance clawDistance(CLAW_DISTANCE);

// claw - true is open.  opcontrol toggles it directly, and the lift opens it while scoring.
// If it's backwards, make this ez::Piston claw(CLAW, true) - that flips the output.
ez::Piston claw(CLAW);

Lift lift(liftMotors, liftRotation, pivotMotor, pivotRotation, claw);

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
ez::tracking_wheel horiz_tracker(21, 2.0, 2.56);  // dist 2

// distance sensor position reset - back is nullptr until it has a sensor, so back() always gives its fallback.
// When it gets one: add a pros::Distance backDist(port) and swap nullptr, 0.0 for &backDist, its offset
pros::Distance leftDist(LEFT_DIST);
pros::Distance rightDist(RIGHT_DIST);
pros::Distance frontDist(FRONT_DIST);
DistReset distReset(chassis,
                    &leftDist, LEFT_DIST_OFFSET,
                    &rightDist, RIGHT_DIST_OFFSET,
                    &frontDist, FRONT_DIST_OFFSET,
                    nullptr, 0.0);
// ez::tracking_wheel vert_tracker(9, 2.75, 4.0);   // This tracking wheel is parallel to the drive wheels

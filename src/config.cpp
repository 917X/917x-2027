#include "config.hpp"
#include "pros/adi.h"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"

// controller - shared by both robots below
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// ===========================================================================
// Last season (Push Back) - reference only, nothing starts these
// ===========================================================================
//
// Declared FIRST on purpose.  rightMotors / leftMotors call set_gearing on
// construction, and ports 13, 14 and 15 are also this year's drive.  Static
// init inside one file runs top to bottom, so the chassis below is built last
// and its gearing is the one that sticks on the shared ports.
//
// Heads up on the other overlaps: PB_MIDDLE_ROLLER is port 7, which is this
// year's IMU, and PB_DISTANCE is port 1, which is this year's intake.  Those
// objects will just fail their reads and writes.  Don't start intakeControl()
// or call last season's autons without sorting the ports out first.

constexpr int PB_RIGHT_F = -18;
constexpr int PB_RIGHT_M = 19;
constexpr int PB_RIGHT_B = -17;

constexpr int PB_LEFT_F = 15;
constexpr int PB_LEFT_M = -14;
constexpr int PB_LEFT_B = 13;

constexpr int PB_INDEXER = -10;
constexpr int PB_MIDDLE_ROLLER = 7;
constexpr int PB_BOTTOM_ROLLER = -8;
constexpr int PB_FRONT_ROLLER = 16;

constexpr int PB_DISTANCE = 1;

constexpr char PB_INTAKE_PISTON = 'H';
constexpr char PB_FLAPPER_PISTON = 'G';
constexpr char PB_TOP_SORT = 6;

constexpr char PB_IMU = 21;

// optical
pros::Optical colorSort(PB_TOP_SORT);

pros::adi::DigitalOut intakePiston(PB_INTAKE_PISTON);
pros::adi::DigitalOut flapperPiston(PB_FLAPPER_PISTON);

pros::Motor indexerMotor(PB_INDEXER);
pros::Motor bottomRoller(PB_BOTTOM_ROLLER);
pros::Motor middleRoller(PB_MIDDLE_ROLLER);
pros::Motor frontRoller(PB_FRONT_ROLLER);

Intake intake(indexerMotor, colorSort, frontRoller, middleRoller, bottomRoller);

pros::Imu imu(PB_IMU);
pros::Distance distance(PB_DISTANCE);

// drivetrain
pros::MotorGroup rightMotors({PB_RIGHT_F, PB_RIGHT_M, PB_RIGHT_B}, pros::MotorGearset::blue);
pros::MotorGroup leftMotors({PB_LEFT_F, PB_LEFT_M, PB_LEFT_B}, pros::MotorGearset::blue);

// ===========================================================================
// 2026-27 robot - this is the one that runs
// ===========================================================================

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

// intake
pros::Motor intakeMotor(INTAKE);

// lift + claw
pros::MotorGroup liftMotors({LIFT_L, LIFT_R});
ez::Piston claw(CLAW);
ez::Piston clawPivot(CLAW_PIVOT);

Lift lift(liftMotors, claw, clawPivot);

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

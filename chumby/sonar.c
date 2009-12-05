/* -----------------------------------------------------------------------------
 * File: sonar.c
 * Description: Defines functions to read from the sonar, and to avoid obstacles
 * 	based on data from it. We expect main() to call these functions and help
 *  pass values from one to the other.
 * Functions:
 *  - open_sonar(): Main calls this to get the tty for the sonar sensors.
 *  - ReadSonarData(): Main calls this to get what the sonar at tty reads. It
 *  	passes back an array of possible values that main needs to average.
 *  - GetSonarStep(): Main passes in a bunch of information about the cone, its
 *  	current position, where it first encountered an obstacle (as a
 *  	precondition we assume that this function is not called unless we have
 *  	detected an obstacle and are trying to avoid it now), the direction it's
 *  	facing, the direction it wants to face, as well as the current sonar
 *  	data. The sonar "step" is what part of the obstacle avoidance routine
 *  	we're on, which changes based on the context. They are documented below.
 *  - AvoidObstacle(): Based on the sonar step, sets the direction we want to
 *  	face. We should use SteerTo() in main to determine what the actual
 *  	steering should be in order for us to face that direction.
 *  - OnGoalLine(): Called by GetSonarStep() to see if we're on the goal line.
 *  	Note the obstacle avoidance algorithm referenced in the function header
 *  	for AvoidObstacle() explains what the goal line is.
 * -------------------------------------------------------------------------- */

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "main.h"

// Software - obstacle avoidance. //////////////////////////////////////////////
/* -----------------------------------------------------------------------------
 * Function: AvoidObstacle()
 * Purpose: To set the intended direction of the robot to correctly avoid an
 *  obstacle, if it even has to. What this function actually does depends on
 *  step. See the notes on step below. The calling function is responsible for
 *  setting the step.
 * Description: The algorithm is a "bug" algorithm. See page 22 of
 *   http://www.cs.jhu.edu/~hager/Teaching/cs336/Notes/Chap2-Bug-Alg.pdf or page
 *   1 of http://ai.stanford.edu/~latombe/cs26n/2007/scribe2.pdf.
 * Parameters:
 *  - frontDist: The distance to the nearest object in front of us, according to
 *  	the sonar.
 *  - rightDist: The distance to the nearest object to the right of us,
 *  	according to the sonar.
 *  - carDirection: The direction the car is currently facing.
 *  - step: What step of the avoidance routine we're on.
 *  	Step -1: The null step. Don't change our intended direction.
 *		Step 0: No obstacles in the way, let gps do the driving.
 *		Step 1: Obstacle encountered, turn left.
 *		Step 2: Follow obstacle, keeping it to our right side. Branches to
 *			either Step 3, Step 4, or Step 5.
 *		Step 3: We were following an obstacle, but don't see it on our right
 *			side anymore. So, we turn right and go to Step 3.
 *		Step 4: We were following an obstacle, but see another obstacle in front
 *			of us. Turn left and go to Step 2.
 *		Step 5: We were following an obstacle, but have just crossed the goal
 *			line. Orient ourselves back towards cone and let gps do the driving
 *			again (go to Step 0).
 *		Step 6: We're clear of the cone. Same as the null step.
 *	- oldIntendedDirection: The intended direction before the function call.
 *		Usually, we won't be changing the intended direction, so we just set the
 *		intended direction to the old one.
 *	- carLat: The current latitude of the car. Used to drive back to cone when
 *		the goal line is clear. Same for the below three variables.
 *	- carLong: The current longitude of the car.
 *	- coneLat: The latitude of the cone.
 *	- coneLong: The longitude of the cone.
 * Return value: The direction we want the robot to be facing.
 * -------------------------------------------------------------------------- */
double AvoidObstacle (double frontDist, double rightDist, double carDirection,
	int step, double oldIntendedDirection, double carLat, double carLong,
	double coneLat, double coneLong) {
	// Local variables. ----------------------------------------------------- //
	double intendedDirection; // The direction we want. This is the return
							  // value.

	// Normalize all direction variables.
	while (oldIntendedDirection >= 360)
		oldIntendedDirection -= 360;
	while (oldIntendedDirection < 0)
		oldIntendedDirection += 360;

	// Function body. ------------------------------------------------------- //
	switch (step) {
	// Do not change the intended direction, just do nothing.
	case SN_STEP_NULL_STEP: {
		intendedDirection = oldIntendedDirection;
		break;
	}

	// The program shouldn't even enter this case.
	case SN_STEP_NO_OBS: {
		intendedDirection = oldIntendedDirection;
		break;
	}

	// If there's an obstacle in front of us, turn left.
	case SN_STEP_ENCOUNTERED_OBS: {
		intendedDirection = carDirection + SN_LEFT_TURN;
		break;
	}

	// We want the obstacle to be to our right by a certain distance.
	// If we're too close, then we drift slightly left. If we're too far away,
	// then drift slightly right. But overall, we should keep it next to us.
	case SN_STEP_FOLLOW_OBS: {
		if (rightDist < SN_FOLLOW_DISTANCE) {
			intendedDirection = carDirection + SN_SMALL_LEFT;
		}
		else {
			intendedDirection = carDirection + SN_SMALL_RIGHT;
		}

		break;
	}

	// Turn right.
	case SN_STEP_LOST_OBS_ON_RIGHT: {
		intendedDirection = carDirection + SN_RIGHT_TURN;
		break;
	}

	// Turn left.
	case SN_STEP_OBS_WHILE_FOLLOW: {
		intendedDirection = carDirection + SN_LEFT_TURN;
		break;
	}

	// Orient ourselves on the line pointing directly to the cone.
	// NOTE: Or should the GPS turn() function just handle this?
	case SN_STEP_CROSSED_GOAL_LINE: {
		intendedDirection = DirectionTo (carLat, carLong, coneLat, coneLong);
		break;
	}

	case SN_STEP_CLEAR: {
		intendedDirection = oldIntendedDirection;
		break;
	}

	// The program shouldn't even enter this case.
	default: {
		intendedDirection = oldIntendedDirection;
		break;
	}
	}

	while (intendedDirection >= 360)
		intendedDirection -= 360;
	while (intendedDirection < 0)
		intendedDirection += 360;

	return intendedDirection;
}

/* -----------------------------------------------------------------------------
 * Function: GetSonarStep()
 * Purpose: To figure out what sonar step we should be on.
 * Description: Tells us what sonar step we should be on based on the given
 * 	information. Each step has its own logic for when we should proceed to the
 * 	next or loop back to a previous step, but in general, in Steps 1, 3, 4, and
 * 	5, we proceed to the next step if we're facing the intended direction (set
 *  by AvoidObstacle()), and do nothing if we're not yet facing the intended
 *  direction. In Step 2, we are constantly changing the intended value unless
 *  we meet some condition that takes us into Step 3, 4, or 5.
 * Parameters:
 *  - currentStep: The step we're currently on.
 *  - carLat: The latitude of the car (used to do GPS checks).
 *  - carLong: The longitude of the car (used to do GPS checks).
 * 	- encObsLat: The latitude of the car when we encountered the obstacle at
 * 		first. Used to get the goal line.
 *  - encObsLong: The longitude of the car when we encountered the obstacle at
 * 		first. Used to get the goal line.
 *  - coneLat: The latitude of the cone when we encountered the obstacle at
 * 		first. Used to get the goal line.
 *  - coneLong: The longitude of the cone when we encountered the obstacle at
 * 		first. Used to get the goal line.
 *  - carDirection: The direction the car's currently facing.
 *  - intendedDirection: The direction AvoidObstacle() wants the car to be
 *  	facing. We use this to know when to proceed to the next step.
 * Return value: Returns what sonar step we should be on.
 * -------------------------------------------------------------------------- */
// NOTE: I get the feeling this can be done inside AvoidObstacle(), given the
// massive parameter list.
int GetSonarStep (int currentStep, double carLat, double carLong,
	double encObsLat, double encObsLong, double coneLat, double coneLong,
	double carDirection, double intendedDirection, double frontDist,
	double rightDist)
{
	int sonarStep = SN_STEP_NO_OBS; // Return value.

	// Normalize all direction values.
	while (carDirection >= 360)
		carDirection -= 360;
	while (carDirection < 0)
		carDirection += 360;
	while (intendedDirection >= 360)
		intendedDirection -= 360;
	while (intendedDirection < 0)
		intendedDirection += 360;

	// If we're on Step 1, then after we've completed the turn, we want to move
	// directly to Step 2.
	if (sonarStep == SN_STEP_ENCOUNTERED_OBS) {
		// If we're facing the correct direction, then go to the next step.
		if ((abs(carDirection - intendedDirection) < DIR_EPSILON_SMALL) ||
			(abs(carDirection - intendedDirection) > DIR_EPSILON_LARGE)) {
			sonarStep = SN_STEP_FOLLOW_OBS;
		}
		else {
			sonarStep = SN_STEP_NULL_STEP;
		}
	}

	// If we're in Step 2, we can branch to either Step 3, 4, or 5.
	if (sonarStep == SN_STEP_FOLLOW_OBS) {
		// If we've lost the obstacle on the right, jump to Step 3 (turn right).
		if (rightDist < SN_ALMOST_ZERO) {
			sonarStep = SN_STEP_LOST_OBS_ON_RIGHT;
		}

		// If we encounter an obstacle in front of us while following the
		// current obstacle, then go to Step 4 (turn left).
		if (frontDist < SN_TRIP_DISTANCE) {
			sonarStep = SN_STEP_OBS_WHILE_FOLLOW;
		}

		// If we've crossed the goal line, check if we're closer than when we
		// started. If we are, then go to Step 5. Otherwise, keep following the
		// obstacle.
		if (OnGoalLine (carLat, carLong, encObsLat, encObsLong, coneLat,
			coneLong) == TRUE) {
			if (DistanceBetween(carLat, carLong, coneLat, coneLong) <
				DistanceBetween(encObsLat, encObsLong, coneLat, coneLong)) {
				sonarStep = SN_STEP_CROSSED_GOAL_LINE;
			}
			else {
				sonarStep = SN_STEP_FOLLOW_OBS;
			}
		}
	}

	// If we're in Step 3, then we need to turn right until we're facing the
	// intended direction.
	if (sonarStep == SN_STEP_LOST_OBS_ON_RIGHT) {
		// If we're facing the correct direction, then go to the next step.
		if (abs(carDirection - intendedDirection) < DIR_EPSILON_SMALL ||
			abs(carDirection - intendedDirection) > DIR_EPSILON_LARGE)
			sonarStep = SN_STEP_FOLLOW_OBS;
		else
			sonarStep = SN_STEP_NULL_STEP;
	}

	// If we're in Step 4, then we need to turn left until we're facing the
	// correct direction.
	if (sonarStep == SN_STEP_OBS_WHILE_FOLLOW) {
		// If we're facing the correct direction, then go to the next step.
		if (abs(carDirection - intendedDirection) < DIR_EPSILON_SMALL ||
			abs(carDirection - intendedDirection) > DIR_EPSILON_LARGE) {
			sonarStep = SN_STEP_FOLLOW_OBS;
		}
		else {
			sonarStep = SN_STEP_NULL_STEP;
		}
	}

	// If we're in Step 5, reset ourselves once we're back on track towards the
	// cone.
	if (sonarStep == SN_STEP_CROSSED_GOAL_LINE) {
		// Check if we've oriented ourselves properly. If so, then set step to
		// SN_STEP_NO_OBS.
		if (abs(carDirection - intendedDirection) < DIR_EPSILON_SMALL ||
			abs(carDirection - intendedDirection) > DIR_EPSILON_LARGE) {
			sonarStep = SN_STEP_CLEAR;
		}
		else
			sonarStep = SN_STEP_NULL_STEP;
	}

	return sonarStep;
}

/* -----------------------------------------------------------------------------
 * Function: OnGoalLine()
 * Purpose: To check if the robot has crossed the goal line, +/- some wiggle
 * 	room GL_EPSILON.
 * Description: We treat all the positions as vectors from the origin. We
 * 	calculate the difference vector between the cone's position and the car's
 * 	"obstacle encountered" position. We call this vector the goal vector.Then
 * 	we calculate the difference vector between the car's current position and
 *  the "obstacle encountered" position. We call this the car vector.
 *  First we check to make sure that the longitude component of the car vector
 *  is not 0. If it is, then the error is the latitude component. Otherwise, we
 *  divide the longitude component of the goal vector by the longitude of the
 *  car vector, and call the result of this alpha. Then (latitude of goal
 *  vector) - (alpha*latitude of car vector) is the error. Overall, we want
 *  |error| < SN_GL_EPSILON
 * Parameters:
 *  - carLat: The latitude of the car.
 *  - carLong: The longitude of the car.
 * 	- encObsLat: What the car's latitude was when it encountered the obstacle.
 *  - encObsLong: What the car's longitude was when it encountered the obstacle.
 *  - coneLat: What the cone's latitude is.
 *  - coneLong: What the cone's longitude is.
 * Return value: Returns TRUE if we're on the goal line, FALSE if we're not.
 * -------------------------------------------------------------------------- */
int OnGoalLine (double carLat, double carLong, double encObsLat,
	double encObsLong, double coneLat, double coneLong) {
	double goalX = coneLong - encObsLong;
	double goalY = coneLat - encObsLat;
	double carX = carLong - encObsLong;
	double carY = carLat - encObsLat;
	double error;
	double alpha;

	if (carX == 0)
		error = carY;
	else {
		alpha = goalX / carX;
		error = goalY - (alpha * carY);
	}

	// Now convert the error to inches.
	error = error * INCHES_PER_DEG_LAT;

	return (abs(error) < SN_GL_EPSILON);
}

// Hardware interface. /////////////////////////////////////////////////////////
/* -----------------------------------------------------------------------------
 * Function: ReadSonarData()
 * Purpose: To copy data from the sonar sensor into sonarData.
 * Description: Uses read to get sonar data, and sscanf to parse the data.
 * Parameters:
 * 	- sonarData: An array where we place sonar sensor data in. The data is in
 * 		inches. Currently array is of size 10.
 *  - tty: tty so we can read sonar sensor data.
 * Return value: None. The intended output is placed in sonarData.
 * -------------------------------------------------------------------------- */
void ReadSonarData (float sonarData[], int tty) {
	char buf[BUFSIZ]; // The buffer we put the string from read() into.

	(void) read(tty, buf, BUFSIZ); // Get data from sonar sensor, put it in buf.

	// Parse buf and get the 10 values from it.
	// NOTE: This needs to be modified if the number of values we get from the
	// sonar changes.
	sscanf(buf, "%f %f %f %f %f %f %f %f %f %f", &sonarData[0], &sonarData[1],
		&sonarData[2], &sonarData[3], &sonarData[4], &sonarData[5],
		&sonarData[6], &sonarData[7], &sonarData[8], &sonarData[9]);
}

/* -----------------------------------------------------------------------------
 * Function: open_sonar()
 * Purpose: To get the tty for the sonar sensor, which is used in
 * 	ReadSonarData().
 * Description: Copied directly from open_gps().
 * Parameters: None.
 * Return value: tty for the sonar sensor.
 * -------------------------------------------------------------------------- */
// NOTE: probably needs to take a param indicating whether this is the front or
// the right sonar.
int open_sonar() {
	struct termios sonar_term;
	int TTY;

	// NOTE: This line below is probably wrong.
	TTY = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	tcgetattr(TTY, &sonar_term);

	// NOTE: In fact, all these lines may be wrong.
	sonar_term.c_cflag = B4800;
	sonar_term.c_cflag |= CS8;
	sonar_term.c_cflag |= CREAD;
	sonar_term.c_iflag = IGNPAR | IGNBRK;
	sonar_term.c_cflag |= CLOCAL;
	tcsetattr(TTY, TCSANOW, &sonar_term);
	tcflush(TTY, TCOFLUSH);
	tcflush(TTY, TCIFLUSH);
	return TTY;
}


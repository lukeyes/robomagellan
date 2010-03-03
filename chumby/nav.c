
//Structure to store goal position
struct Location
{
	int distance;
	int heading;
};

//Structure to store current state
struct State
{
	double accel;//Acceleration
	double accel_time; //Time between accelerometer readings;
	double accel_error[2][2]; //Estimated accelerometer error from Kalman filter
	double velocity;
	double position; //0 corresponds to starting point, feet from starting point is position
	double gps_position; // GPS calculated position from start point
};

//Convert G-force output from sensor to acceleration in ft/sec^2
//Resolution is ~.5ft/sec^2
int accel_convert(struct State *c_state, struct Location *c_location)
{	
}


//Fill struct with dummy values
void initialize_state(struct State* c_state)
{
	c_state->accel_time = 1; //Need to change
	int dt = c_state->accel_time;
	int an = 1; //Need to change 
	c_state->accel_error[0][0] = (an*dt*dt*dt*dt/4); 
	c_state->accel_error[1][0] = (an*dt*dt*dt/2); 
	c_state->accel_error[0][1] = c_state->accel_error[1][0];
	c_state->accel_error[1][1] = (an*dt*dt);

	c_state->position = 0;
	c_state->velocity = 0;		
}

//Need updated accelerometer value and GPS position before calling
void kalman_filter( struct State* c_state)
{
	//Tweakable values
	static double gpsnoise = 10;
	static double accelnoise = 1;	
	double dt = c_state->accel_time;

	//Set values - Note [row][column]
	static double an = accelnoise * accelnoise;
	double accel = c_state->accel;
	static double A[2][2] = { {1,dt},{0,1} };
	static double B[2][1] = {{(dt*dt)/2}, {dt}};
	double x[2][1] = {{c_state->position},{c_state->velocity}};
	double y[1][2] = {c_state->gps_position, 0};
	double Inn;
	double K[2][1];

	static double accel_noise_covariance[2][2] = { {an*dt*dt*dt*dt/4, an*dt*dt*dt/2},{an*dt*dt*dt/2, an*dt*dt} };
	static double gps_noise_covariance = gpsnoise*gpsnoise;
	double P[2][2];
	P[0][0] = c_state->accel_error[0][0];
	P[1][0] = c_state->accel_error[1][0];
	P[0][1] = c_state->accel_error[0][1];
	P[1][1] = c_state->accel_error[1][1];   

	//Implement Filter

	//Estimate new position/velocity from current accelerometer values
	x[0][0] = (A[0][0]*x[0][0] + A[0][1]*x[1][0] ) + accel*B[0][0];
	x[1][0] = (A[1][0]*x[0][0] + A[1][1]*x[1][0] ) + accel*B[1][0];

	Inn = y[0][0] - x[0][0];

	K[0][0] = (A[0][0]*P[0][0]+ A[0][1]*P[1][0])/(P[0][0] + gps_noise_covariance);
	K[1][0] = (A[1][0]*P[0][0] + A[1][1]*P[1][0])/(P[0][0] + gps_noise_covariance);

	//Estimate new position accounting for errors
	x[0][0] = x[0][0] + K[0][0]*Inn;
	x[1][0] = x[1][0] + K[1][0]*Inn;

	//Recalculate accelerometer error
	P[0][0] = (1 + K[0][0])*(P[0][0] + P[0][1]*A[0][1]) + P[1][0]*A[0][1] + P[1][1]*A[0][1]*A[0][1] + accel_noise_covariance[0][0];
	P[0][1] = (1 + K[0][0])*P[0][1] + A[0][1]*P[1][1] + accel_noise_covariance[0][1];
	P[1][0] = P[1][0] + P[1][1]*A[0][1] +accel_noise_covariance[1][0] + K[1][0]*(P[0][0] + P[0][1]*A[0][1]);
	P[1][1] = P[1][1] + accel_noise_covariance[1][1] + K[1][0]*P[0][1];

	c_state->accel_error[0][0] = P[0][0];
	c_state->accel_error[1][0] = P[1][0];
	c_state->accel_error[0][1] = P[0][1];
	c_state->accel_error[1][1] = P[1][1];
	c_state->position = x[0][0];
	c_state->velocity = x[1][0];
}

void navigation()
{
	struct Location goal;
	struct State current_state;

	//Fill up current state information(position relative to goal,acceleration, GPS calculation)

	//Calculate distance and heading between current location and goal location

	//Navigate towards goal
}

int main()
{
	struct State current_state;
	initialize_state(&current_state);
	kalman_filter(&current_state);
	return 0;
}


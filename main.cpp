//base includes :)
#include "main.h"

/* TODO:
 * - [DONE?] one auton with vars to switch sides
 * - [DONE] update arcade to match 'smartness' of tank
 * - [DONE] make auton use optical detection to sort >> use for loops to update
 * - [DONE] Make points scored by auton - that button - roll over & etc
 * - [NO] Persistent variables smile (after some reflection we dont need it)
 * - i think thats it
*/

//team enum so we switch autons & for ball sorting
enum team{
	red,
	blue,
};
team current_team = team::red; //by default is red

//enum for the side of the field we are on, so we can switch :)
enum side{
	left,
	right,
};
side auton_side = side::left; //default = left (duh, you can read too)

//enum for the amount of goal we're visiting
enum goal_auton_switch {
	one = 1,
	two = 2,
	three = 3,
};
goal_auton_switch goals_auton = goal_auton_switch::one; //the default is one++ = two

/*
 * LLEMU functions for the buttons:
 * Left = Auton Side
 * Centre = Team Colour
 * Right = Goal Autons 1/2/3
 */
void on_left_button(){
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(1, "Left Side Auton");
		auton_side = side::left;
	} else {
		lcd::set_text(1, "Right Side Auton");
		auton_side = side::right;
	}
}
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(2, "Red Team");
		current_team = team::red;
	} else {
		lcd::set_text(2, "Blue Team");
		current_team = team::blue;
	}
}
void on_right_button() {
	static int i = 1;
	i++;
	if(i > 3 || i < 1) i = 1;
	switch (i){
		case 1:
			goals_auton = goal_auton_switch::one;
			lcd::set_text(3, "Auton Type: One Goal Auton");
			break;
		case 2:
			goals_auton = goal_auton_switch::two;
			lcd::set_text(3, "Auton Type: Two Goal Auton");
			break;
		case 3:
			goals_auton = goal_auton_switch::three;
			lcd::set_text(3, "Auton Type: Three Goal Auton");
			break;
	}
}

//Default init function, inits LLEMU, sets first line text, and links buttons to corresponding functions
void initialize() {
	lcd::initialize();
	lcd::set_text(0, "2918H GCEC");
	lcd::register_btn0_cb(on_left_button);
	lcd::register_btn1_cb(on_center_button);
	lcd::register_btn2_cb(on_right_button);
	on_left_button();
	on_center_button();
	on_right_button();
}

//some other default voids
void disabled() {}
void competition_initialize() {}

//what used to be the old auton, now is new auton, but not auton auton
void old_auton(side team_side){
	// all the arc variables, that change depending on what side and/or team we're on
	//first arc backs into middle goal
	int arc1_int1 = 0;
	if (team_side == side::left) arc1_int1 = -100;
	if (team_side == side::right) arc1_int1 = -20;
	int arc1_int2 = 0;
	if (team_side == side::left) arc1_int2 = -20;
	if (team_side == side::right) arc1_int2 = -100;

	//second arc drives to side goal
	int arc2_int1 = 0;
	if (team_side == side::left) arc2_int1 = 90;
	if (team_side == side::right) arc2_int1 = 55;
	int arc2_int2 = 0;
	if (team_side == side::left) arc2_int2 = 55;
	if (team_side == side::right) arc2_int2 = 90;

	//third arc turns the robot towards the opposite side
	int arc3_int1 = 0;
	if (team_side == side::left) arc3_int1 = 70;
	if (team_side == side::right) arc3_int1 = -40;
	int arc3_int2 = 0;
	if (team_side == side::left) arc3_int2 = -40;
	if (team_side == side::right) arc3_int2 = 70;
	int arc3_int3 = 0;
	if (team_side == side::left) arc3_int3 = -69;
	if (team_side == side::right) arc3_int3 = 0;
	int arc3_int4 = 0;
	if (team_side == side::left) arc3_int4 = 0;
	if (team_side == side::right) arc3_int4 = -69;

	//forth arc drives the robot to the opposite side goal
	int arc4_int1 = 0;
	if (team_side == side::left) arc4_int1 = 100;
	if (team_side == side::right) arc4_int1 = 125;
	int arc4_int2 = 0;
	if (team_side == side::left) arc4_int2 = 125;
	if (team_side == side::right) arc4_int2 = 100;

	//declare all the things
	Controller master(E_CONTROLLER_MASTER); //probs dont need this, but hey ctrl+c ctrl+v
	Motor left_mtrf(11, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_ROTATIONS);
	Motor left_mtrb(12, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_ROTATIONS);
	Motor right_mtrf(13, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_ROTATIONS);
	Motor right_mtrb(14, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_ROTATIONS);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical optical_sensor(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H'); //mmmmmm very fitting (this is the limit switch for the ball sorter)
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD); //setting both the intake motors to brake=hold so they hold open
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	//roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	optical_sensor.set_led_pwm(90);

	//drive backwards into the middle goal, to align the flipper
	right_mtrb.move_velocity(arc1_int1);
	right_mtrf.move_velocity(arc1_int1);
	left_mtrb.move_velocity(arc1_int2);
	left_mtrf.move_velocity(arc1_int2);
	delay(500);
	//unhook the flap & release the intakes
	roller1.move_velocity(190);
	roller2.move_velocity(190);
	intake1.move_velocity(-70);
	intake2.move_velocity(-70);
	delay(200);
	//stop driving backwards
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	delay(500);
	//stop the rollers & intakes
	roller1.move_velocity(0);
	roller2.move_velocity(0);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	//we just scored the middle goal!

	if(goals_auton == goal_auton_switch::one) return;

	//drive in 2nd arc
	right_mtrb.move_velocity(arc2_int1);
	right_mtrf.move_velocity(arc2_int1);
	left_mtrb.move_velocity(arc2_int2);
	left_mtrf.move_velocity(arc2_int2);
	delay(500);
	//sort the balls when we get to the side goal
	bool sort = false;
	for (int i = 0; i < 96; i++){
		//print some debug stuff, im sure you can figure it out
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(optical_sensor.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(optical_sensor.get_proximity()));
		lcd::set_text(7, "Auton Iteration Value: " + std::to_string(i));
		lcd::set_text(8, "Auton Sorting Value: " + std::to_string(sort));
		if (sort && lmt_switch.get_value() == 0){ //when the backpanel isn't up properly and we're sorting
			roller1.move_velocity(190); //drive the rollers upward
			roller2.move_velocity(190);
		} else { //or
			roller1.move_velocity(190); //drive the rollers upward
			roller2.move_velocity(190);
			sort = false; //and say we aren't sorting anymore
		}
		if((((current_team == team::blue) && ((optical_sensor.get_hue() >= 0 && optical_sensor.get_hue() <= 0) \
					 || (optical_sensor.get_hue() >= 350))) || (current_team == team::red && (optical_sensor.get_hue() >= 200
					 && optical_sensor.get_hue() <= 290))) && optical_sensor.get_proximity() >= 130){
			//if the ball is red & we're blue, do, or if we're red and the ball is blue, do
			roller1.move_velocity(-190); //roll the top roller back, pushing the ball out the back
			roller2.move_velocity(90); //roll the top roller forwards, prevents jamming (i think)
			intake1.move_velocity(0); //stop the intakes so as not to jam as well
			intake2.move_velocity(0);
			sort = true; //say that we're sorting
		}
		switch (i){ //after time intervals (from start), do this:
			default:
				break;
			case 0: //after time = 0 (0 * 50) ms
				//spin the rollers at the start
				roller1.move_velocity(150);
				roller2.move_velocity(150);
				break;
			case 30: //after time = 1500 (30 * 50) ms
				//spin the intake to intake the balls
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				break;
			case 55: //after time = 2750 (55 * 50) ms
				//stop driving in the arc (dont damage the field)
				right_mtrb.move_velocity(0);
				right_mtrf.move_velocity(0);
				left_mtrb.move_velocity(0);
				left_mtrf.move_velocity(0);
				break;
			case 75: //after time = 3750 (75 * 50) ms
				//open the intake to allow the balls to fall downwards
				intake1.move_velocity(-70);
				intake2.move_velocity(-70);
				break;
			case 85: //after time = 4250 (85 * 50) ms
				//intake the balls that fell down, and drive forwards to make sure we grip them
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				right_mtrb.move_velocity(100);
				right_mtrf.move_velocity(100);
				left_mtrb.move_velocity(100);
				left_mtrf.move_velocity(100);
				break;
			case 90: //after time = 4500 (90 * 50) ms
				//stop driving forwards to not destroy the goal :)
				right_mtrb.move_velocity(0);
				right_mtrf.move_velocity(0);
				left_mtrb.move_velocity(0);
				left_mtrf.move_velocity(0);
				break;
			case 95: //after time = 4750 (95 * 50) ms
				//at the end, stop the rollers
				roller1.move_velocity(0);
				roller2.move_velocity(0);
				intake1.move_velocity(0);
				intake2.move_velocity(0);
				break;
		}
		delay(50);
	}
	//after the sorting
	delay(750);
	if(goals_auton != goal_auton_switch::three){ //if doing a two-goal auton, travel back a smaller amount
		right_mtrb.move_velocity(-50);
		right_mtrf.move_velocity(-50);
		left_mtrb.move_velocity(-50);
		left_mtrf.move_velocity(-50);
	}
	if(goals_auton == goal_auton_switch::three){ //drive backwards ~amount~ for three-goal auton
		right_mtrb.tare_position();
		right_mtrf.tare_position();
		left_mtrb.tare_position();
		left_mtrf.tare_position();
		right_mtrb.move_relative(-5, 75);
		right_mtrf.move_relative(-5, 75);
		left_mtrb.move_relative(-5, 75);
		left_mtrf.move_relative(-5, 75);
	}
	delay(250);
	//open the intake and spin the bottom roller to release any balls that maybe stuck in the intake
	intake1.move_velocity(-50);
	intake2.move_velocity(-50);
	roller2.move_velocity(-50);
	delay(900);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	roller2.move_velocity(0);
	//stop doing ^
	delay(700);
	//stop driving backwards
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);

	if (goals_auton == goal_auton_switch::two) return; //if we arent doing a three-goal auton, stop here

	//bruhhhhhhhh turn another ~amount~
	right_mtrb.tare_position();
	right_mtrf.tare_position();
	left_mtrb.tare_position();
	left_mtrf.tare_position();
	right_mtrb.move_relative(arc3_int1 * (5/3) + arc3_int3, arc3_int1);
	right_mtrf.move_relative(arc3_int1 * (5/3) + arc3_int3, arc3_int1);
	left_mtrb.move_relative(arc3_int2 * (5/3) + arc3_int4, arc3_int2);
	left_mtrf.move_relative(arc3_int2 * (5/3) + arc3_int4, arc3_int2);
	delay(1000);

	//drive in a loose arc *around* our teammate, not hitting them in anyway and successfully making it to the opposite side goal
	right_mtrb.move_velocity(arc4_int1);
	right_mtrf.move_velocity(arc4_int1);
	left_mtrb.move_velocity(arc4_int2);
	left_mtrf.move_velocity(arc4_int2);
	delay(2250);
	right_mtrb.move_velocity(50);
	right_mtrf.move_velocity(50);
	left_mtrb.move_velocity(50);
	left_mtrf.move_velocity(50);
	delay(500);

	//more sorting, at the other side goal, opposite to where we started
	sort = false;
	for (int i = 0; i < 50; i++){
		//print debug stuff
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(optical_sensor.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(optical_sensor.get_proximity()));
		lcd::set_text(7, "Auton Iteration Value: " + std::to_string(i));
		lcd::set_text(8, "Auton Sorting Value: " + std::to_string(sort));
		if (sort && lmt_switch.get_value() == 0){//when the backpanel isn't up properly and we're sorting
			roller1.move_velocity(190); //run the rollers
			roller2.move_velocity(190);
		} else {
			roller1.move_velocity(190); //still run the rollers
			roller2.move_velocity(190);
			sort = false; //but say we'ren't sorting anymore
		}
		if((((current_team == team::blue) && ((optical_sensor.get_hue() >= 0 && optical_sensor.get_hue() <= 5) \
					 || (optical_sensor.get_hue() >= 340))) || (current_team == team::red && (optical_sensor.get_hue() >= 210
					 && optical_sensor.get_hue() <= 280))) && optical_sensor.get_proximity() >= 130){
			//if the ball is red & we're blue, do, or if we're red and the ball is blue, do
			roller1.move_velocity(-190); //roll the top roller back, pushing the ball out the back
			roller2.move_velocity(90); //roll the top roller forwards, prevents jamming (i think)
			intake1.move_velocity(0); //stop the intakes so as not to jam as well
			intake2.move_velocity(0);
			sort = true; //say that we're sorting
		}
		switch (i){ //do the time interval stuff (all time is from starting the loop, so they stack(ish))
			default:
				break;
			case 0: //at 0 ms (0*50)
				//at the start, start the rollers
				roller1.move_velocity(150);
				roller2.move_velocity(150);
				break;
			case 5: //at 250 ms (5*50)
				//just after the start, run the intake to intake
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				break;
			case 35: //at 1750 ms (35*50)
				//slowly advance on the goal, stalking it as thy prey
				right_mtrb.move_velocity(30);
				right_mtrf.move_velocity(30);
				left_mtrb.move_velocity(30);
				left_mtrf.move_velocity(30);
				break;
			//stops at case 49
		}
		delay(50);
	}
	delay(500);
	//drive back a bit, to make sure we aren't descoring anything
	right_mtrb.move_velocity(-70);
	right_mtrf.move_velocity(-70);
	left_mtrb.move_velocity(-70);
	left_mtrf.move_velocity(-70);
	delay(500);
	//consequently stop doing that ^
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	delay(750);
	//stop EVERYTHING
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	roller1.move_velocity(0);
	roller2.move_velocity(0);
}

//DO NOT the use of this function, is the slightly bigly broke  (+ and no nice comments)
void auton(side team_side){
	//first drive arc (doesn't really matter but hey)
	int arc1_int1 = 0;
	if (team_side == side::left) arc1_int1 = -100;
	if (team_side == side::right) arc1_int1 = -20;
	int arc1_int2 = 0;
	if (team_side == side::left) arc1_int2 = -20;
	if (team_side == side::right) arc1_int2 = -100;

	int arc2_int1 = 0;
	if (team_side == side::left) arc2_int1 = 90;
	if (team_side == side::right) arc2_int1 = 55;
	int arc2_int2 = 0;
	if (team_side == side::left) arc2_int2 = 55;
	if (team_side == side::right) arc2_int2 = 90;

	int arc3_int1 = 0;
	if (team_side == side::left) arc3_int1 = 100;
	if (team_side == side::right) arc3_int1 = -100;
	int arc3_int2 = 0;
	if (team_side == side::left) arc3_int2 = -100;
	if (team_side == side::right) arc3_int2 = 100;

	Controller master(E_CONTROLLER_MASTER);
	Motor left_mtrf(11, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
	Motor left_mtrb(12, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_ROTATIONS);
	Motor right_mtrf(13, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_ROTATIONS);
	Motor right_mtrb(14, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_ROTATIONS);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical optical_sensor(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H');
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	//roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	optical_sensor.set_led_pwm(90);

	/*
	right_mtrb.move_relative(arc1_int1 / .2, arc1_int1);
	right_mtrf.move_relative(arc1_int1 / .2, arc1_int1);
	left_mtrb.move_relative(arc1_int2 / .2, arc1_int2);
	left_mtrf.move_relative(arc1_int2 / .2, arc1_int2);
	delay(1000);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);*/

	//unhook the flap & release the intakes
	roller1.move_velocity(190);
	roller2.move_velocity(190);
	intake1.move_velocity(-70);
	intake2.move_velocity(-70);
	delay(700);
	roller1.move_velocity(0);
	roller2.move_velocity(0);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	//drive in 2nd arc
	right_mtrb.tare_position();
	right_mtrf.tare_position();
	left_mtrf.tare_position();
	left_mtrb.tare_position();
	//right_mtrf.move_relative(arc2_int1 / 3.250 * (5/3), arc2_int1);
	//right_mtrb.move_relative(arc2_int1 / 3.250 * (5/3), arc2_int1);
	//left_mtrb.move_relative(arc2_int2 / 3.250 * (5/3), arc2_int2);
	//left_mtrf.move_relative(arc2_int2 / 3.250 * (5/3), arc2_int2);
	//delay(3000);
	/*delay(500);
	bool sort = false;
	for (int i = 0; i < 96; i++){
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(optical_sensor.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(optical_sensor.get_proximity()));
		lcd::set_text(7, "Auton Iteration Value: " + std::to_string(i));
		lcd::set_text(8, "Auton Sorting Value: " + std::to_string(sort));

		if (sort && lmt_switch.get_value() == 0){
			roller1.move_velocity(190);
			roller2.move_velocity(190);
		} else {
			roller1.move_velocity(190);
			roller2.move_velocity(190);
			sort = false;
		}
		if((((current_team == team::blue) && ((optical_sensor.get_hue() >= 0 && optical_sensor.get_hue() <= 0) \
					 || (optical_sensor.get_hue() >= 350))) || (current_team == team::red && (optical_sensor.get_hue() >= 200
					 && optical_sensor.get_hue() <= 290))) && optical_sensor.get_proximity() >= 130){
			roller1.move_velocity(-190);
			roller2.move_velocity(90);
			intake1.move_velocity(0);
			intake2.move_velocity(0);
			sort = true;
		}
		switch (i){
			default:
				break;
			case 0:
				roller1.move_velocity(150);
				roller2.move_velocity(150);
				break;
			case 30:
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				break;
			case 55:
				right_mtrb.move_velocity(0);
				right_mtrf.move_velocity(0);
				left_mtrb.move_velocity(0);
				left_mtrf.move_velocity(0);
				break;
			case 75:
				intake1.move_velocity(-70);
				intake2.move_velocity(-70);
				break;
			case 85:
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				right_mtrb.move_velocity(100);
				right_mtrf.move_velocity(100);
				left_mtrb.move_velocity(100);
				left_mtrf.move_velocity(100);
				break;
			case 90:
				right_mtrb.move_velocity(0);
				right_mtrf.move_velocity(0);
				left_mtrb.move_velocity(0);
				left_mtrf.move_velocity(0);
				break;
			case 95:
				roller1.move_velocity(0);
				roller2.move_velocity(0);
				intake1.move_velocity(0);
				intake2.move_velocity(0);
				break;
		}
		delay(50);
	}
	delay(750);
	right_mtrb.move_velocity(-150);
	right_mtrf.move_velocity(-150);
	left_mtrb.move_velocity(-150);
	left_mtrf.move_velocity(-150);
	delay(500);
	intake1.move_velocity(-50);
	intake2.move_velocity(-50);
	roller2.move_velocity(-50);
	delay(500);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	roller2.move_velocity(0);
	delay(250);
	right_mtrb.move_velocity(arc3_int1);
	right_mtrf.move_velocity(arc3_int1);
	left_mtrb.move_velocity(arc3_int2);
	left_mtrf.move_velocity(arc3_int2);
	delay(555);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);

	if (!three_goal_auton) return;

	right_mtrb.move_velocity(100);
	right_mtrf.move_velocity(100);
	left_mtrb.move_velocity(100);
	left_mtrf.move_velocity(100);
	delay(2000);
	right_mtrb.move_velocity(50);
	right_mtrf.move_velocity(50);
	left_mtrb.move_velocity(50);
	left_mtrf.move_velocity(50);
	delay(500);

	sort = false;
	for (int i = 0; i < 76; i++){
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(optical_sensor.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(optical_sensor.get_proximity()));
		lcd::set_text(7, "Auton Iteration Value: " + std::to_string(i));
		lcd::set_text(8, "Auton Sorting Value: " + std::to_string(sort));
		if (sort && lmt_switch.get_value() == 0){
			roller1.move_velocity(190);
			roller2.move_velocity(190);
		} else {
			roller1.move_velocity(190);
			roller2.move_velocity(190);
			sort = false;
		}
		if((((current_team == team::blue) && ((optical_sensor.get_hue() >= 0 && optical_sensor.get_hue() <= 5) \
					 || (optical_sensor.get_hue() >= 340))) || (current_team == team::red && (optical_sensor.get_hue() >= 210
					 && optical_sensor.get_hue() <= 280))) && optical_sensor.get_proximity() >= 130){
			roller1.move_velocity(-190);
			roller2.move_velocity(90);
			intake1.move_velocity(0);
			intake2.move_velocity(0);
			sort = true;
		}
		switch (i){
			default:
				break;
			case 0:
				roller1.move_velocity(150);
				roller2.move_velocity(150);
				break;
			case 5:
				intake1.move_velocity(70);
				intake2.move_velocity(70);
				break;
			case 55:
				right_mtrb.move_velocity(30);
				right_mtrf.move_velocity(30);
				left_mtrb.move_velocity(30);
				left_mtrf.move_velocity(30);
				break;
		}
		delay(50);
	}
	delay(500);
	right_mtrb.move_velocity(-30);
	right_mtrf.move_velocity(-30);
	left_mtrb.move_velocity(-30);
	left_mtrf.move_velocity(-30);
	delay(500);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	delay(750);
	intake1.move_velocity(0);
	intake2.move_velocity(0);*/
}

//we need to pass an argument to the auton func, so make a custom one
void autonomous() {
	old_auton(auton_side); //yay
}

//we have two drive types available :) so we are the best team
enum driveType{
	tank,
	arcade,
};

//driver control method
void opcontrol() {
	//declare all the variables
	//pluggies
	Controller master(E_CONTROLLER_MASTER);
	Motor left_mtrf(11, true);
	Motor left_mtrb(12, false);
	Motor right_mtrf(13, false);
	Motor right_mtrb(14, true);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical optical_sensor(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H');

	//set motor break types
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);

	//set the optical sensor's LED to 90%
	optical_sensor.set_led_pwm(90);

	//multiply all the drive values by 90% to make sure the motors dont burn out in a match
	float drive_multiplier = 0.9f;

	//do NOT the display methods once to init them
	//on_center_button();
	//on_left_button();
	//on_right_button();

	//drive types for switching drive types (Tank/Arcade Controls)
	driveType currentDriveType = driveType::tank;
	driveType previousDriveType = driveType::tank;

	//intake timing system vars
	bool R1_pressed = false;
	float now = millis();
	float now1 = millis();
	float now2 = millis();

	//the sorting bool
	bool sort = false;

	/*						     Controller Map + Controls
	 *					       _________       _________
	 *		 -Rollers Up	/ [ L2 ]  \-----/  [ R2 ] \ -Outtake
	 *	  -Rollers Down	| [ L1 ]  # ### #  [ R1 ] | -Intake
	 *	 (with sorting)	\_________________________/
	 *
	 *							 _======___________======_
	 *							/  Y   [###########]   Y  \
	 *-Tank : Left Drive  | X(L)  [###########] X(R) |   -Tank : Right Drive / -Arcade : Entire Drive
	 *  -^ : Rollers Up   /  ^    [###########]   X  \   -X : Enable Tank Controls / Disable Arcade
	 *			 -> : ### |  < >       [%%]       Y A  |  -A : Enable Arcade Controls / Disable Tank
	 *			 -< : ### /   v /-------""-------\ B   \  -Y : Activate Autonomous
	 * -v : Rollers Down |     |                |     |  -B : ###
	 *					    \_____/                \_____/
	 */
	//all hail the almighty while(true) loop!
	while (true) {
		//Joysticks (Analogue Sticks)
		if(currentDriveType == driveType::tank){ //Tank Controls
			left_mtrf.move(master.get_analog(ANALOG_LEFT_Y) * drive_multiplier);
			left_mtrb.move(master.get_analog(ANALOG_LEFT_Y) * drive_multiplier);
			right_mtrf.move(master.get_analog(ANALOG_RIGHT_Y) * drive_multiplier);
			right_mtrb.move(master.get_analog(ANALOG_RIGHT_Y) * drive_multiplier);
		} else if (currentDriveType == driveType::arcade){ //Arcade Control
			left_mtrf.move((master.get_analog(ANALOG_RIGHT_Y) + master.get_analog(ANALOG_RIGHT_X)) * drive_multiplier);
			left_mtrb.move((master.get_analog(ANALOG_RIGHT_Y) + master.get_analog(ANALOG_RIGHT_X)) * drive_multiplier);
			right_mtrf.move((master.get_analog(ANALOG_RIGHT_Y) - master.get_analog(ANALOG_RIGHT_X)) * drive_multiplier);
			right_mtrb.move((master.get_analog(ANALOG_RIGHT_Y) - master.get_analog(ANALOG_RIGHT_X)) * drive_multiplier);

			//in case of analogue controlling other systems, uncomment:
			//intake1.move(master.get_analog(ANALOG_LEFT_Y) * 0.9f);
			//intake2.move(master.get_analog(ANALOG_LEFT_Y) * 0.9f);
			//roller1.move(master.get_analog(ANALOG_LEFT_X) * 0.9f);
			//roller2.move(master.get_analog(ANALOG_LEFT_X) * 0.9f);
		}

		//Buttons arent effected by the drive type
		//Right buttons control the intake arms
		if (master.get_digital(DIGITAL_R1)) {  //top goes in
			if (millis() > now1 + 500){
				now1 = millis();
				intake1.move_velocity(95);
				intake2.move_velocity(95);
			}
			if (millis() > now2 + 1250) { //every so often, open them to let balls drop down the goal
				now2 = millis();
				intake1.move_velocity(-45);
				intake2.move_velocity(-45);
			}
			R1_pressed = true; //say that button R1 is pressed

		} else if (master.get_digital(DIGITAL_R2)) { //bottom goes out
			R1_pressed = false; //say that button R1 is not pressed
			intake1.move_velocity(-50); //run the intake back
			intake2.move_velocity(-50);

		} else if (R1_pressed == true) { //else if R1 is still pressed, do some stuff
			intake1.move_velocity(-50);
			intake2.move_velocity(-45);

			if (millis() > now + 500) { //make now reset every 500ms
				now = millis();
			} else if (millis() > now + 350) { //after 350ms, reset R1_pressed to false
				R1_pressed = false;
			}

		} else { //if no buttons, stop, hammer time
			intake1.move_velocity(0);
			intake2.move_velocity(0);
		}

		//Left Buttons control the rollers
		if (master.get_digital(DIGITAL_L1)) { //if top button, do sorting code
			if (sort && lmt_switch.get_value() == 0){ //if backpanel is not up, and we do the sort
				roller1.move_velocity(190); //top roller forward (up)
				roller2.move_velocity(0); //bottom roller stop
			} else { //otherwise,
				roller1.move_velocity(190); //both motors forward (up)
				roller2.move_velocity(190);
				sort = false; //say we do not the sort
			}
			if((((current_team == team::blue) && ((optical_sensor.get_hue() >= 0 && optical_sensor.get_hue() <= 5) \
						 || (optical_sensor.get_hue() >= 340))) || (current_team == team::red && (optical_sensor.get_hue() >= 210
						 && optical_sensor.get_hue() <= 280))) && optical_sensor.get_proximity() >= 130){
				//if the ball is red and we're blue, do, or if we're blue and ball is red, do
				roller1.move_velocity(-190); //run the top motor backwards to output the ball
				roller2.move_velocity(0); //stop the front motor
				sort = true; //say that we're sorting
			}
		} else if (master.get_digital(DIGITAL_L2)) { //if bottom button, and this is the simple, run both motors backwards
			roller1.move_velocity(-190);
			roller2.move_velocity(-190);
		} else { //else, make the rollers stop.
			roller1.move_velocity(0);
			roller2.move_velocity(0);
		}

		//in case of jam, other controls for rollers
		if (master.get_digital(DIGITAL_UP)) { //press up, rollers roll up
			roller1.move_velocity(190);
			roller2.move_velocity(190);
		} else if (master.get_digital(DIGITAL_DOWN)) { //press down, rollers go down
			roller1.move_velocity(-190);
			roller2.move_velocity(-190);
		}

		if(master.get_digital(DIGITAL_A)) currentDriveType = driveType::arcade; //if press A, change drive to arcade
		if(master.get_digital(DIGITAL_X)) currentDriveType = driveType::tank; //if press X, change drive to tank
		if(master.get_digital(DIGITAL_Y)) autonomous(); //if press Y, do auton

		//Display Stuff
		//print what control scheme we are using (Tank/Arcade) to the controller's screen
		std::basic_string driveString = "";
		if (currentDriveType == driveType::tank && previousDriveType != driveType::tank){
			driveString = "Drive Type: Tank";
			master.clear_line(0);
			delay(150);
			master.print(0, 0, "Tank Control");
			previousDriveType = currentDriveType;
		}
		if (currentDriveType == driveType::arcade && previousDriveType != driveType::arcade){
			driveString = "Drive Type: Arcade";
			master.clear_line(0);
			delay(150);
			master.print(0, 0, "Arcade Control");
			previousDriveType = currentDriveType;
		}
		lcd::set_text(4, driveString); //and, for good measure, to the brain
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(optical_sensor.get_hue())); //im sure you can figure these out
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(optical_sensor.get_proximity()));
		lcd::set_text(7, "Limit Switch Value: " + std::to_string(lmt_switch.get_value()));
		lcd::set_text(8, "Sort Value: " + std::to_string(sort));

		delay(20); //a delay of 20ms for the while(true) loop, supplied by default
	}
}

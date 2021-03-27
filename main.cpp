#include "main.h"
/* TODO:
 * - [DONE?] one auton with vars to switch sides
 * - [DONE] update arcade to match 'smartness' of tank
 * - make auton use optical detection to sort >> use for loops to update
 * - i think thats it
*/
enum team{
	red,
	blue,
};
team current_team = team::red;

enum side{
	left,
	right,
};
side auton_side = side::left;

bool three_goal_auton;

void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(3, "Red Team");
		current_team = team::red;
	} else {
		lcd::set_text(3, "Blue Team");
		current_team = team::blue;
	}
}

void on_right_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(2, "3 Goal Auton: Active");
		three_goal_auton = true;
	} else {
		lcd::set_text(2, "3 Goal Auton: Disabled");
		three_goal_auton = false;
	}
}

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

void initialize() {
	lcd::initialize();
	lcd::set_text(0, "2918H GCEC");
	lcd::register_btn0_cb(on_left_button);
	lcd::register_btn1_cb(on_center_button);
	lcd::register_btn2_cb(on_right_button);
}

void disabled() {}
void competition_initialize() {}

void old_auton(){
	Controller master(E_CONTROLLER_MASTER);
	Motor left_mtrf(11, true);
	Motor left_mtrb(12, false);
	Motor right_mtrf(13, false);
	Motor right_mtrb(14, true);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical oppy(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H');
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	oppy.set_led_pwm(90);

	right_mtrb.move_velocity(-20);
	right_mtrf.move_velocity(-20);
	left_mtrb.move_velocity(-100);
	left_mtrf.move_velocity(-100);
	delay(200);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);

	//unhook the flap
	roller1.move_velocity(190);
	roller2.move_velocity(190);
	delay(850);
	roller1.move_velocity(0);
	roller2.move_velocity(0);

	//release the rollers
	intake1.move_velocity(-50);
	intake2.move_velocity(-50);
	delay(250);
	intake1.move_velocity(0);
	intake2.move_velocity(0);

	//drive forward
	right_mtrb.move_velocity(190);
	right_mtrf.move_velocity(190);
	left_mtrb.move_velocity(190);
	left_mtrf.move_velocity(190);
	delay(250);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);

	delay(100);
}

void auton(side team_side){
	//YOU FUCKER REMOVE THIS!!!!!!!!!!
	current_team = team::blue;

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
	Motor left_mtrf(11, true);
	Motor left_mtrb(12, false);
	Motor right_mtrf(13, false);
	Motor right_mtrb(14, true);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical oppy(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H');
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	//roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	oppy.set_led_pwm(90);

	right_mtrb.move_velocity(arc1_int1);
	right_mtrf.move_velocity(arc1_int1);
	left_mtrb.move_velocity(arc1_int2);
	left_mtrf.move_velocity(arc1_int2);
	delay(200);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);

	//unhook the flap & release the intakes
	roller1.move_velocity(190);
	roller2.move_velocity(190);
	intake1.move_velocity(-70);
	intake2.move_velocity(-70);
	delay(500);
	roller1.move_velocity(0);
	roller2.move_velocity(0);
	intake1.move_velocity(0);
	intake2.move_velocity(0);

	//drive in 2nd arc
	right_mtrb.move_velocity(arc2_int1);
	right_mtrf.move_velocity(arc2_int1);
	left_mtrb.move_velocity(arc2_int2);
	left_mtrf.move_velocity(arc2_int2);
	delay(500);
	bool sort = false;
	for (int i = 0; i < 91; i++){
		//lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(oppy.get_hue()));
		//lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(oppy.get_proximity()));
		//lcd::set_text(7, "Auton Iteration Value: " + std::to_string(i));
		//lcd::set_text(8, "Auton Sorting Value: " + std::to_string(sort));
		if (sort && lmt_switch.get_value() == 0){
			roller1.move_velocity(190);
			roller2.move_velocity(190);
		} else {
			roller1.move_velocity(190);
			roller2.move_velocity(190);
			sort = false;
		}
		if((((current_team == team::blue) && ((oppy.get_hue() >= 0 && oppy.get_hue() <= 5) \
					 || (oppy.get_hue() >= 340))) || (current_team == team::red && (oppy.get_hue() >= 210
					 && oppy.get_hue() <= 280))) && oppy.get_proximity() >= 130){
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
				roller1.move_velocity(0);
				roller2.move_velocity(0);
				break;
		}
		delay(50);
	}
	delay(750);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	right_mtrb.move_velocity(-150);
	right_mtrf.move_velocity(-150);
	left_mtrb.move_velocity(-150);
	left_mtrf.move_velocity(-150);
	intake1.move_velocity(-50);
	intake2.move_velocity(-50);
	roller2.move_velocity(-50);
	delay(500);
	intake1.move_velocity(0);
	intake2.move_velocity(0);
	roller2.move_velocity(0);
	delay(1000);
	right_mtrb.move_velocity(arc3_int1);
	right_mtrf.move_velocity(arc3_int1);
	left_mtrb.move_velocity(arc3_int2);
	left_mtrf.move_velocity(arc3_int2);
	delay(600);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	/*if(!three_goal_auton){
		right_mtrb.move_velocity(0);
		right_mtrf.move_velocity(0);
		left_mtrb.move_velocity(0);
		left_mtrf.move_velocity(0);
		return;
	}
	right_mtrb.move_velocity(150);
	right_mtrf.move_velocity(150);
	left_mtrb.move_velocity(150);
	left_mtrf.move_velocity(150);
	delay(1500);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	delay(100);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(190);
	left_mtrf.move_velocity(190);
	delay(600);
	right_mtrb.move_velocity(0);
	right_mtrf.move_velocity(0);
	left_mtrb.move_velocity(0);
	left_mtrf.move_velocity(0);
	delay(250);
	right_mtrb.move_velocity(30);
	right_mtrf.move_velocity(30);
	left_mtrb.move_velocity(30);
	left_mtrf.move_velocity(30);
	delay(500);

	sort = false;
	for (int i = 0; i < 20; i++){
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(oppy.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(oppy.get_proximity()));
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
		if((((current_team == team::blue) && ((oppy.get_hue() >= 0 && oppy.get_hue() <= 5) \
					 || (oppy.get_hue() >= 340))) || (current_team == team::red && (oppy.get_hue() >= 210
					 && oppy.get_hue() <= 280))) && oppy.get_proximity() >= 130){
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
				break;
			case 90:
				right_mtrb.move_velocity(0);
				right_mtrf.move_velocity(0);
				left_mtrb.move_velocity(0);
				left_mtrf.move_velocity(0);
				roller1.move_velocity(0);
				roller2.move_velocity(0);
				break;
		}
		delay(50);
	}
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
	intake2.move_velocity(0);
	*/
}

void autonomous() {
	auton(auton_side);
	//old_auton();
}

enum driveType{
	tank,
	arcade,
};

void opcontrol() {
	Controller master(E_CONTROLLER_MASTER);
	Motor left_mtrf(11, true);
	Motor left_mtrb(12, false);
	Motor right_mtrf(13, false);
	Motor right_mtrb(14, true);
	Motor intake1(15, MOTOR_GEARSET_36, false);
	Motor intake2(16, MOTOR_GEARSET_36, true);
	Motor roller1(17, true);
	Motor roller2(18, false);
	Optical oppy(20); //Blue balls are values between 200-250, red is around 0-50
	ADIDigitalIn lmt_switch ('H');
	intake1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intake2.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	roller1.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	oppy.set_led_pwm(90);

	float driveMultiplier = 0.9f;

	//do the display methods once to init them
	on_center_button();
	on_left_button();
	on_right_button();

	driveType currentDriveType = driveType::tank;
	driveType previousDriveType = driveType::tank;

	bool r1Pressed = false;
	float now = millis();
	float now1 = millis();
	float now2 = millis();
	bool sort = false;

	/*					     Controller Map + Controls
						       _________       _________
			 -Rollers Up	/ [ L2 ]  \-----/  [ R2 ] \ -Outtake
		  -Rollers Down	| [ L1 ]  # ### #  [ R1 ] | -Intake
								\_________________________/

								 _======___________======_
								/  Y   [###########]   Y  \
	-Tank : Left Drive  | X(L)  [###########] X(R) |   -Tank : Right Drive / -Arcade : Entire Drive
				 -^ : ###  /  ^    [###########]   X  \   -X : Tank Controls
				 -> : ### |  < >       [%%]       Y A  |  -A : Arcade Controls
				 -< : ### /   v /-------""-------\ B   \  -Y : Activate Autonomous
				 -v : ### |     |                |     |  -B : ###
						    \_____/                \_____/
						*/

	while (true) {
		//Tank Drive Controls
		if(currentDriveType == driveType::tank){
			//analog sticks
			left_mtrf.move(master.get_analog(ANALOG_LEFT_Y) * driveMultiplier);
			left_mtrb.move(master.get_analog(ANALOG_LEFT_Y) * driveMultiplier);
			right_mtrf.move(master.get_analog(ANALOG_RIGHT_Y) * driveMultiplier);
			right_mtrb.move(master.get_analog(ANALOG_RIGHT_Y) * driveMultiplier);


			if (master.get_digital(DIGITAL_R1)) {
				if (millis() > now1 + 500){
					now1 = millis();
					intake1.move_velocity(95);
					intake2.move_velocity(95);
				}

				if (millis() > now2 + 1250) {
					now2 = millis();
					intake1.move_velocity(-45);
					intake2.move_velocity(-45);
				}
				r1Pressed = true;
			} else if (master.get_digital(DIGITAL_R2)) {
				r1Pressed = false;
				intake1.move_velocity(-50);
				intake2.move_velocity(-50);

			} else if (r1Pressed == true) {
				intake1.move_velocity(-50);
				intake2.move_velocity(-45);

				if (millis() > now + 500) {
					now = millis();
				} else if (millis() > now + 350) {
					r1Pressed = false;
				}
			} else {
				intake1.move_velocity(0);
				intake2.move_velocity(0);
			}

			if (master.get_digital(DIGITAL_L1)) {
				if (sort && lmt_switch.get_value() == 0){
					roller1.move_velocity(190);
					roller2.move_velocity(0);
				} else {
					roller1.move_velocity(190);
					roller2.move_velocity(190);
					sort = false;
				}
				if((((current_team == team::blue) && ((oppy.get_hue() >= 0 && oppy.get_hue() <= 5) \
							 || (oppy.get_hue() >= 340))) || (current_team == team::red && (oppy.get_hue() >= 210
							 && oppy.get_hue() <= 280))) && oppy.get_proximity() >= 130){
					roller1.move_velocity(-190);
					roller2.move_velocity(0);
					sort = true;
				}
			} else if (master.get_digital(DIGITAL_L2)) {
				roller1.move_velocity(-190);
				roller2.move_velocity(-190);
			} else {
				roller1.move_velocity(0);
				roller2.move_velocity(0);
			}
		} else if (currentDriveType == driveType::arcade){ //Arcade Control
			left_mtrf.move((master.get_analog(ANALOG_RIGHT_Y) + master.get_analog(ANALOG_RIGHT_X)) * driveMultiplier);
			left_mtrb.move((master.get_analog(ANALOG_RIGHT_Y) + master.get_analog(ANALOG_RIGHT_X)) * driveMultiplier);
			right_mtrf.move((master.get_analog(ANALOG_RIGHT_Y) - master.get_analog(ANALOG_RIGHT_X)) * driveMultiplier);
			right_mtrb.move((master.get_analog(ANALOG_RIGHT_Y) - master.get_analog(ANALOG_RIGHT_X)) * driveMultiplier);

			if (master.get_digital(DIGITAL_R1)) {
				if (millis() > now1 + 500){
					now1 = millis();
					intake1.move_velocity(95);
					intake2.move_velocity(95);
				}

				if (millis() > now2 + 1250) {
					now2 = millis();
					intake1.move_velocity(-45);
					intake2.move_velocity(-45);
				}
				r1Pressed = true;

			} else if (master.get_digital(DIGITAL_R2)) {
				r1Pressed = false;
				intake1.move_velocity(-50);
				intake2.move_velocity(-50);

			} else if (r1Pressed == true) {
				intake1.move_velocity(-50);
				intake2.move_velocity(-45);

				if (millis() > now + 500) {
					now = millis();
				} else if (millis() > now + 350) {
					r1Pressed = false;
				}
			} else {
				intake1.move_velocity(0);
				intake2.move_velocity(0);
			}

			if (master.get_digital(DIGITAL_L1)) {
				roller1.move_velocity(190);
				roller2.move_velocity(190);
			} else if (master.get_digital(DIGITAL_L2)) {
				roller1.move_velocity(-190);
				roller2.move_velocity(-190);
			} else {
				roller1.move_velocity(0);
				roller2.move_velocity(0);
			}
			//intake1.move(master.get_analog(ANALOG_LEFT_Y) * 0.9f);
			//intake2.move(master.get_analog(ANALOG_LEFT_Y) * 0.9f);
			//roller1.move(master.get_analog(ANALOG_LEFT_X) * 0.9f);
			//roller2.move(master.get_analog(ANALOG_LEFT_X) * 0.9f);
		}

		if(master.get_digital(DIGITAL_A)) currentDriveType = driveType::arcade;
		if(master.get_digital(DIGITAL_X)) currentDriveType = driveType::tank;
		if(master.get_digital(DIGITAL_Y)) autonomous();

		//Display Stuff

		//lcd::print(0, "red: %d center: %d blue: %d", (lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		//                 (lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		//                 (lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
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
		lcd::set_text(4, driveString);
		lcd::set_text(5, "Optical Sensor Hue Value: " + std::to_string(oppy.get_hue()));
		lcd::set_text(6, "Optical Sensor Prox Value: " + std::to_string(oppy.get_proximity()));
		lcd::set_text(7, "Limit Switch Value: " + std::to_string(lmt_switch.get_value()));
		//lcd::set_text(7, "Roller 1 Power Value: " + std::to_string(intake1.get_power()));
		lcd::set_text(8, "Sort Value: " + std::to_string(sort));
		delay(20);
	}
}

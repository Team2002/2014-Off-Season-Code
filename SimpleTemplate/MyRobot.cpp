#include "WPILib.h"
#include <math.h>

class RobotDemo: public SimpleRobot {
	Joystick* joy; // only joystick
	
	Jaguar* driveRight;
	Jaguar* driveLeft;
	
	Jaguar* elevation;
	DigitalInput* elevationTop;
	DigitalInput* elevationBottom;
	
	Relay* barrels[4];
	int barrelState[4];
	Timer* safteyTimer;
	
	DriverStationLCD* LCD;
	
	Jaguar* LEDs;
	Timer* LEDTimer;
	
public:
	RobotDemo(void) {
		joy = new Joystick(2); // as they are declared above.
		
		driveRight = new Jaguar(1);
		driveLeft = new Jaguar(7);
		
		
		elevation = new Jaguar(2);
		elevationTop = new DigitalInput(1);
		elevationBottom = new DigitalInput(2); 
		
		barrels[0] =  new Relay(1);
		barrels[1] =  new Relay(2);
		barrels[2] =  new Relay(3);
		barrels[3] =  new Relay(4);
		safteyTimer = new Timer();
	
		LCD->DriverStationLCD::GetInstance();
		
		LEDs = new Jaguar(3);
		LEDTimer = new Timer();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void) {
		Wait(2.0); //    for 2 seconds
		
		
	}
	
	void LED(Jaguar* jag, double x, double speed = 5) {
		jag->Set((sin(x/speed)+2)/4);
	}
	
	void cannon(int barrel, double time = 2, double openTime = 0.5) {
		DriverStationLCD::Line line;
		switch(barrel) {
			case 0:
				line = DriverStationLCD::kUser_Line1;
			break;
			case 1:
				line = DriverStationLCD::kUser_Line2;
			break;
			case 2:
				line = DriverStationLCD::kUser_Line3;
			break;
			case 3:
				line = DriverStationLCD::kUser_Line4;
			break;
			default:
				line = DriverStationLCD::kUser_Line6;
			break;
		}
		switch(barrelState[barrel]) {
			//Wait for button
			case 0:
				safteyTimer->Stop();
				safteyTimer->Reset();
				LCD->PrintfLine(line, "Barrel %i: Wating for button Press", (barrel + 1));
				if(joy->GetRawButton(barrel + 1) && safteyTimer->Get() <= 0) {
					barrelState[barrel] = 1;
					safteyTimer->Start();
				}
			break;
			//Wait for a set amount of time, and fire
			case 1:
				LCD->PrintfLine(line, "Barrel %i: Second until fire: %4.2f", (barrel + 1), (time - safteyTimer->Get()));
				if(safteyTimer->HasPeriodPassed(time)) {
					barrels[barrel]->Set(Relay::kForward);
					barrelState[barrel] = 2;
				}
			break;
			//Wait and additional amount of time and close the barrel & reset state
			case 2:
				LCD->PrintfLine(line, "Barrel %i: Fired!", (barrel + 1));
				if(safteyTimer->HasPeriodPassed(time + openTime)) {
					barrels[barrel]->Set(Relay::kOff);
					barrelState[barrel] = 0;
				}
			break;
			default:
				barrelState[barrel] = 0;
			break;
		}
		//if we let go of the button and the barel is not fired
		if(!joy->GetRawButton(1) && safteyTimer->Get() < 2) {
			barrelState[barrel] = 0;
		}
	}

	void OperatorControl(void) {
		//Can't seem to set all at once for some reason...
		// barrelState = {0,0,0,0};
		barrelState[0] = 0;
		barrelState[1] = 0;
		barrelState[2] = 0;
		barrelState[3] = 0;
		
		LEDTimer->Start();
		
		while (IsOperatorControl()) {
			//Yes I *could* do it in a loop, but meh..
			cannon(0);
			cannon(1);
			cannon(2);
			cannon(3);
			
			//Don't remember how to program limit switches assiming this is it..
			//Would like to use D-pad for hight (Don't know buttons without controller)
			if(joy->GetRawButton(7) && elevationTop->Get() == 0) {
				elevation->Set(1);
			} else if(joy->GetRawButton(8) && elevationBottom->Get() == 0) {
				elevation->Set(-1);
			} else {
				elevation->Set(0);
			}
			
			LED(LEDs, LEDTimer->Get());
			LCD->PrintfLine(DriverStationLCD::kUser_Line5, "Test");
			LCD->UpdateLCD();
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, in2,    gyrob,          sensorGyro)
#pragma config(Sensor, in4,    leftLine,       sensorLineFollower)
#pragma config(Sensor, in5,    rightLine,      sensorLineFollower)
#pragma config(Sensor, dgtl1,  armAngle,       sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  poleDist,       sensorSONAR_cm)
#pragma config(Sensor, dgtl5,  rightDist,      sensorSONAR_cm)
#pragma config(Sensor, dgtl7,  clawAngle,      sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  dataIn,         sensorDigitalOut)
#pragma config(Sensor, dgtl10, turnLight,      sensorLEDtoVCC)
#pragma config(Sensor, dgtl11, clock,          sensorDigitalOut)
#pragma config(Sensor, dgtl12, load,           sensorDigitalOut)
#pragma config(Motor,  port1,           left1,         tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           frontLeft,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           frontRight,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           left2,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           claw1,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           claw2,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           right2,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           backLeft,      tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           backRight,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          right1,        tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//-------------------------------------------------------------------------------------------*/

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

int seekAngle=-1;
int startTile=1;


#define CLOSE 1
#define OPEN 250

#include "gyro.c"												//include gyro code
#include "AUTONOMOUS.c"
#include "eyes.c"
//#include "Sounds.c"

#define	JITTER 			2										//Max value to ignore from the gyro - increase accuracy, prevent drift
#define	JOYZONE 		15									//Deadzone for the joystick
#define	CSCALE 			1.01237							//Clockwise scale adjustments to counteract rotation errors
#define	ASCALE 			1.00637							//Anti-clockwise scale adjustments to counteract rotation errors
#define VALIDFIRE 500										//Time in miliseconds to count balls shot
#define SLEWRATE1 8											//Slew rate for drive motors
#define SLEWRATE2 4											//Slew rate for conveyor motors
#define SLEWRATE3 2											//Slew rate for flywheel


#define MINSPEED 22											//Minimum rotation speed for autonomous -------- was 20 (30)

#define DEADZONE 1											//Value to stop rotation for auto-aim
#define REFRESHRATE 0										//How quick to recalculate flywheel speed
int LEFTWHITELINE = 1000;									//Value to sense white line
int RIGHTWHITELINE = 1000;									//Value to sense white line
#define CONVTIME 0											//Value to get ball to correct position
#define LIGHTLIMIT 170									//Value to see green ball is there
#define LIGHTLIMIT2 190									//Value to see orange ball is there
#define UP 1											//Value at which to stop tipping
#define DOWN 2											//Value at which to stop tipping
#define DEPLOYHEIGHT 45

int deployed = 0;
int counter=0;

float motorSlew[10];										//Table for motor slewing


//float truedir,lastgyro;									//gyro variables
int doneAutonomous = 0;									//Flag for if calibrated sensors												//Tile we start on
int doneSetup=0;												//Have calibrated?
float	MoveDir=0;												//Which way to drive
float	FaceDir=0;												//Which way to face
float	DriveTime=0;											//How long to drive for
float	DriveSpeed=0;											//How fast to drive
int	turnPulse=0;												//Nudge turn speed to find angle more accurately.
float PauseTime = 0;
int armPercent=0;
int clawPercent=0;
int armSeek=-1;
int clawSeek=-1;
int clawSpeed=0;
int armSpeed=0;
int throwMode=0;
int sonarDist=0;
int TIPDIST=80;											//Value at which to stop tipping
int eyeState = 0;

float *autoTab;   											//Points at autonomous table being used
int TabPoint=0;      										//Current position in table
int nextCommand=0; 												//Flag to set when ready for next command.#
int lineFound=0;												//Flag to check if on a white line
int angleAdjust=0;

int hangmode=0;
int hanggo=0;
float turn=0;
int displaymode = 1;

int XACCEL=0;
int YACCEL=0;
int ZACCEL=0;

int turnMode=1;



float minBattery=100000000;



/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////



void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                General functions and tasks used by
//																Autonomous and User control.
//
/////////////////////////////////////////////////////////////////////////////////////////

int calibrating=0;

task debug()
{
	//while (true)
	{
		//clearDebugStream();
		//wait10Msec(100);
	}
}

task turnCircle()
{
	int lightOn = 0;
	int last7L = 0;
	while (true)
	{
		if (abs(XACCEL)>120)
		{
			turnMode=1;
		}
		if (vexRT[Btn7L]==1)
		{
			if (last7L==0) turnMode++;
			last7L=1;
			}else{
			last7L=0;
		}
		if (turnMode%2==1&&calibrating==0)
		{
			//SensorValue(turnLight)=0;
			}else{
			lightOn++;
			//if (lightOn<5) SensorValue(turnLight)=1; else SensorValue(turnLight)=0;
			//if (lightOn>10) lightOn=0;
		}

		wait1Msec(50);
	}
}


task loading()
{
	calibrating=1;
	e_command=LOADING;
	clearLCDLine(1);
	displayLCDString(1,0,"0               ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"00              ");
	displayLCDString(1,0,"0               ");

	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"000             ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"0000            ");

	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"00000           ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"000000          ");

	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"0000000         ");

	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"00000000        ");

	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"000000000       ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"0000000000      ");


	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"00000000000     ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"000000000000    ");


	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"0000000000000   ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"00000000000000  ");

	wait1Msec(188);
	clearLCDLine(1);
	displayLCDString(1,0,"000000000000000 ");


	wait1Msec(187);
	clearLCDLine(1);
	displayLCDString(1,0,"0000000000000000");
	playTone(1000,10);
	playTone(1100,10);
	playTone(1300,20);

	calibrating=0;
}

void InitAll()
{
	if (doneSetup==0)
	{
		startTask(loading);
		clearLCDLine(0);
		displayLCDString(0,2,"CALIBRATING!");
		SensorValue(turnLight)=1;
		for (int i =0; i<10;i++){
			motorSlew[i]=0;
		}
		deployed=0;
		counter=0;
		SensorType[gyro] = sensorNone;
		SensorType[gyrob] = sensorNone;
		wait1Msec(1000);
		SensorType[gyro] = sensorGyro;
		SensorType[gyrob] = sensorGyro;
		wait1Msec(2000);
		SensorScale[gyro] = 142;
		SensorScale[gyrob] = 142;
		SensorValue(armAngle)=0;
		SensorValue(clawAngle)=0;
		clearTimer(T2);
		SensorValue(turnLight)=0;
		//LEFTWHITELINE = SensorValue(leftLine)+700;
		//RIGHTWHITELINE = SensorValue(rightLine)+700;
		resetGyro();
	}
	doneSetup=1;
}

float lastAng=0;

int pulse1=7;
int pulse2=7;

task newDrive()
{
	float upDrive;
	float rightDrive;
	float driveAngle=1;
	float driveMag;
	float gyroAng;
	float seek,angle,ang1;

	lastAng=0;

	while (true)
	{
		angleAdjust=270;
		turn = -vexRT[Ch1];
		/*if (turn<0)
		{
		turn=-(turn*turn)/127;
		}else{
		turn=(turn*turn)/127;
		}*/
		upDrive=-vexRT[Ch3];
		rightDrive=vexRT[Ch4];//*-1;		//cheaterd- shouldn't be neg (-)

		//		if (SensorValue(groundUp)>TIPDIST)
		//		{
		//			upDrive=127;
		//			rightDrive=0;
		//			turn=0;
		//		}


		if (rightDrive==0){rightDrive=0.1;}	//This is to prevent divide by 0 error
		if (rightDrive*upDrive>=0)
		{
			driveAngle = atan((upDrive) / (rightDrive)); 			//Find angle of joystick
			}else{
			driveAngle = atan2((upDrive),  (rightDrive));
		}
		driveAngle = radiansToDegrees(driveAngle);						//Change it to degrees
		if (rightDrive<0 && upDrive<0)
		{
			driveAngle=driveAngle+180;
		}
		driveMag = sqrt((rightDrive*rightDrive) + (upDrive*upDrive));	//Find magnitude of joystick
		if (driveMag>127){driveMag=127;}

		if (driveMag<JOYZONE){
			driveMag=0;
		}
		if (abs(turn)<JOYZONE){
			turn=0;
		}

		if (seekAngle>=0)		//user control angle seek override
		{
			DriveTime=USER;
			DriveSpeed=0;
			MoveDir=0;
			FaceDir=seekAngle;
			seekAngle=-1;
			turnPulse=0;
		}

		if (DriveTime!=0)   //autonomous taking over
		{
			driveMag=DriveSpeed;
			driveAngle=MoveDir;

			seek = FaceDir; //value in degrees to rotate to
			angle=gyroDir/10;

			//de-rotate 'angle' so seek in range +/- 180
			ang1=seek-angle;          			//de-rotate
			if (ang1<0) {ang1=360+ang1;}    //correct de-rotation
			if (ang1>180) {ang1=ang1-360;}  //+=clockwise -=anti-clockwise

			if (driveMag<MINSPEED)
			{
				ang1=(ang1/180)*127;            //dividing by 90 means full speed for first 90 degrees of turn. Alter this for different turn agression
			}
			else
			{
				if (hangmode>=1)
				{
					ang1=(ang1/320)*127;
					}else{
					ang1=(ang1/90)*127;
				}
			}
			if (ang1>127) {ang1=127;}       //clamp max speed
			if (ang1<-127) {ang1=-127;}     //to +/- 127

			if (driveMag>=MINSPEED) {					//robot moving at least MINSPEED so can find angle more accuratly
				if (ang1<0){
					if (ang1>-2){ang1=0;}else{if (ang1>-4){ang1=-4;}}
					}else {
					if (ang1<2){ang1=0;}else{if (ang1<4){ang1=4;}}
				}
			}
			else
			{
				{
					turn=ang1;
					ang1=abs(ang1);
					if (ang1<MINSPEED)
					{
						if (((lastAng>0)&&(turn<0))||((lastAng<0)&&(turn>0))) ang1=0;
						else
						{
							if (ang1>MINSPEED/5)
							{
								ang1=MINSPEED;
							}
							else
							{
								turnPulse++;
								if (turnPulse<pulse1) ang1=MINSPEED;		//2
								else
								{
									ang1=1;
									if (turnPulse>pulse2) turnPulse=0;		//4
								}
							}
						}
					}
					if (turn<0) ang1=-ang1;
				}
			}

			if ((ang1==0)&&(driveMag==0)) {
				nextCommand=1;
				DriveTime=0;
			}
			turn=-ang1;
			lastAng=ang1;
		}
		//if (SensorValue(groundUp)<TIPDIST)
		{
			gyroAng=gyroDir/10;
			driveAngle=driveAngle-gyroAng;//-90;			//cheated (no -90)
			if (driveAngle<0){driveAngle=360+driveAngle;}
			//Set motors equal to correct variation of sin & cos
			if(driveMag>=JOYZONE && DriveTime==0){
				driveAngle+=angleAdjust;
				if (driveAngle>360){driveAngle-=360;}
			}
		}
		/*else
		{
		driveMag=127;			//force motors forward to try and stabilise robot
		turn=0;
		driveAngle=0;
		}*/


		if (throwMode<=0 && calibrating==0 && hangmode<=0 && vexRT[Btn8U]==0)
		{
			if (armPercent>80)
			{
				e_command=LOOKUP;
				}else{
				e_command=LOOKSTRAIGHT;
			}

			if (turn>DEADZONE)
			{
				e_command=LOOKRIGHT;
				}else{
				if (turn<-DEADZONE)
				{
					e_command=LOOKLEFT;
				}
			}

			if (driveMag>DEADZONE)
			{
				if ((driveAngle>=0 && driveAngle<45) || (driveAngle>=315 && driveAngle<360))
				{
					e_command=LOOKUP;
				}

				if (driveAngle>=45 && driveAngle<135)
				{
					e_command=LOOKRIGHT;
				}
				if (driveAngle>=135 && driveAngle<225)
				{
					e_command=LOOKDOWN;
				}
				if (driveAngle>=225 && driveAngle<315)
				{
					e_command=LOOKLEFT;
				}
			}
		}
		if (turnMode%2==1)
		{
			//regular turning circle
			motorSlew[frontLeft] = driveMag*( cosDegrees(driveAngle) - sinDegrees(driveAngle) ) - turn;			//+		1			/3
			motorSlew[frontRight] =  driveMag*( (-cosDegrees(driveAngle)) - sinDegrees(driveAngle) ) - turn;	//-	 -1			/3
			motorSlew[backLeft] = -driveMag*( cosDegrees(driveAngle) + sinDegrees(driveAngle) ) + turn;		//+	 -1
			motorSlew[backRight] =  -driveMag*( sinDegrees(driveAngle) - cosDegrees(driveAngle) ) + turn;	//-	  1
			}else{
			//extended turning circle
			motorSlew[frontLeft] = driveMag*( cosDegrees(driveAngle) - sinDegrees(driveAngle) ) + turn/5;			//+		1			/3
			motorSlew[frontRight] =  driveMag*( (-cosDegrees(driveAngle)) - sinDegrees(driveAngle) ) + turn/5;	//-	 -1			/3
			motorSlew[backLeft] = -driveMag*( cosDegrees(driveAngle) + sinDegrees(driveAngle) ) + turn;		//+	 -1
			motorSlew[backRight] =  -driveMag*( sinDegrees(driveAngle) - cosDegrees(driveAngle) ) + turn;	//-	  1
		}

		wait1Msec(20);
	}
}

void slewMotor(int index, int rate)
{
	int motorValue = motor[index];
	int difference = (motorValue - motorSlew[index])/rate;
	if (difference == 0)
	{
		motor[index] = motorSlew[index];
	}
	else
	{
		motor[index] = motorValue-difference;
	}
}


task slewMotors()
{
	while (true)
	{
		int slewRate=SLEWRATE1;
		if (DriveTime==USER) slewRate=1;			//override for fine angle seeking control
			slewMotor(frontRight, slewRate);
		slewMotor(backRight,slewRate);
		slewMotor(frontLeft,slewRate);
		slewMotor(backLeft,slewRate);
		slewMotor(right1,slewRate);
		slewMotor(right2,slewRate);
		slewMotor(left1,slewRate);
		slewMotor(left2,slewRate);
		slewMotor(claw1,slewRate);
		slewMotor(claw2,slewRate);
		wait1Msec(15);
	}
}

int holdArm=-1;

int HOLDPERCENT=60;

int THROWBASE=900;
int THROWSCALE=50;
int CLAWPERCENTZERO = 1;
int THROWPOINT = 0;
int ACTUALTHROWPOINT = 160;
int THROWPOINTLIMIT=165;
int THROWEND=0;
int ACTUALTHROWEND=165;
int THROWENDLIMIT=175;
int lastClawPercent=34;
int clawDifference=0;
int hardGrab=0;
int newSeek=0;
int doneCleared=0;

int count=0;
int armDownCount=0;


int armFind=-1;


int HANGFINISH=240;


task doAutoArm()
{
	while(true)
	{


		armPercent=SensorValue(armAngle);
		clawPercent=SensorValue(clawAngle)+90;

		if (vexRT[Btn6D]==1)
		{
			armSeek=1;
			holdArm=-1;
			newSeek=-1;
			hardGrab=-1;
			clawSeek=OPEN;
			if (deployed==0)
			{
				DriveSpeed=127;
				DriveTime=1000;
				MoveDir=180+240;
				FaceDir=90;
				counter++;
				if (counter>15)
				{
					armSeek=200;
				}
				if (counter>25)
				{
					armSeek=1;
					clawSeek=OPEN;
				}
				if (counter>30)
				{
					DriveTime=0;
					deployed=1;
				}
			}
		}

		if (vexRT[Btn6U]==1)
		{
			deployed=1;
			armSeek=1;
			clawSeek=CLOSE;
			throwMode=0;
			holdArm=-1;
		}
		if (vexRT[Btn8R]==1)
		{
			deployed=1;
			if (throwMode==0) throwMode=1;
			armSeek=-1;
			clawSeek=-1;
			holdArm=-1;
		}
		if (vexRT[Btn7U]==1)
		{
			holdArm=-1;
			armSeek=-1;
			clawSeek=-1;
			throwMode=0;
			clawSpeed=0;
			armSpeed=0;
			armFind=-1;
			hangmode=0;
			DriveTime=0;
			hardGrab=0;
		}


		if (vexRT[Btn8L]==1)	//do arm
		{
			if (armPercent<HOLDPERCENT)
			{
				holdArm=HOLDPERCENT;
				}else{
				armSpeed=127;
				armSeek=-1;
				holdArm=-1;
			}
			}else if (vexRT[Btn8D]==1){
			armSpeed=-127;
			armSeek=-1;
			holdArm=-1;
			}else{
			if (armSeek<0)
			{
				armSpeed=0;
			}
		}

		if (holdArm>0)
		{
			if (armPercent<holdArm)
			{
				armSpeed=127;
			}
		}


		if (vexRT[Btn5U]==1)	//do claw
		{
			clawSpeed=127;
			clawSeek=-1;
			hardGrab=0;
			}else if (vexRT[Btn5D]==1){
			clawSpeed=-127;
			clawSeek=-1;
			hardGrab=0;
			}else{
			if (clawSeek<0)
			{
				clawSpeed=0;
			}
		}



		if (armSeek>0)
		{
			holdArm=-1;
			if (abs(armSeek-armPercent)>5)
			{
				armSpeed=(armSeek-armPercent)*5;
				}else{
				armSeek=-1;
			}
		}

		//		if (armFind>armPercent)
		//		{
		//			armSpeed=127;
		//			}else{
		//			armFind=-1;
		//		}


		if (hardGrab>0)
		{
			if (hardGrab==1)
			{
				clawSpeed=127;
			}
			clawDifference=clawPercent-lastClawPercent;
			if (abs(clawDifference)<1&&hardGrab==1)
			{
				count++;
				if (count>5 || clawPercent<=0)
				{
					count=0;
					newSeek=clawPercent+15;
					hardGrab=2;
					clawSeek=CLOSE;
				}
			}
		}


		/*if (hardGrab>0)
		{
		hardGrab=hardGrab-1;
		if (hardGrab<1) hardGrab=1;
		if (hardGrab==1 || clawPercent>newSeek)
		{
		clawSeek=-1;
		clawSpeed=127;
		if (clawPercent<newSeek) newSeek=clawPercent;

		}else{
		clawSpeed=0;
		}
		clawDifference=clawPercent-lastClawPercent;
		if (abs(clawDifference)<1&&hardGrab==1)
		{
		count++;
		if (count>5)
		{
		//clawSeek=clawPercent-10;
		//if (clawSeek<2)clawSeek=2;
		count=0;
		newSeek=clawPercent;
		if (throwMode<=0){
		hardGrab=25;
		}else{
		hardGrab=15;
		newSeek=1;
		}
		}
		}
		}*/

		if (clawSeek>0)
		{
			if (clawSeek==CLOSE)
			{
				if (hardGrab!=2)
				{
					hardGrab=1;
					newSeek=1;
					clawSeek=-1;
				}
				}else{
				if (hardGrab==1)
				{
					hardGrab=0;
				}
			}
			if (abs(clawSeek-clawPercent)>=10)
			{
				clawSpeed=-(clawSeek-clawPercent)/2;
				if (clawSpeed<30&&clawSpeed>0) clawSpeed=30;
				if (clawSpeed>-30&&clawSpeed<0) clawSpeed=-30;
				}else{
				clawSpeed=0;
				if (clawSeek!=CLOSE && hardGrab!=2)
				{
					clawSeek=-1;
				}
				clawDifference=clawPercent-lastClawPercent;
				if (abs(clawDifference)<1&&(hardGrab==2||clawSeek==CLOSE))
				{
					count++;
					if (count>5)
					{
						count=0;
						clawSeek=clawPercent;
					}
				}
			}

		}


		if (throwMode==1)
		{
			e_command=LOOKUP;
			holdArm=-1;
			THROWEND=ACTUALTHROWEND;
			THROWPOINT=ACTUALTHROWPOINT;
			throwMode=2;
			doneCleared=0;
			CLAWPERCENTZERO=clawPercent;
			clawSpeed=127;
			armSeek=-1;
			hardGrab=1;
			newSeek=1;
		}
		if (throwMode==2)
		{
			if (armPercent>75&&doneCleared==0)
			{
				clearTimer(T3);
				doneCleared=1;
			}
			armSpeed=127;
			//if (clawSpeed<10) clawSpeed=10;
			if (clawPercent>CLAWPERCENTZERO)
			{
				//clawSpeed=75;
				//}else{
				//clawSpeed=20;
			}
			if (armPercent>ACTUALTHROWPOINT)
			{
				writeDebugStreamLine("%d",time1[T3]);
				if (time1[T3]<=THROWBASE)
				{
					THROWPOINT=ACTUALTHROWPOINT;
					}else{
					THROWPOINT=ACTUALTHROWPOINT + ((time1[T3]-THROWBASE)/THROWSCALE);
					THROWEND=ACTUALTHROWEND + ((time1[T3]-THROWBASE)/THROWSCALE);
					if (THROWEND>THROWENDLIMIT) THROWEND=THROWENDLIMIT;
					if (THROWPOINT>THROWPOINTLIMIT) THROWPOINT=THROWPOINTLIMIT;
				}
			}
			if (armPercent>THROWPOINT)
			{
				throwMode=3;
				writeDebugStreamLine("%d",THROWPOINT);
				writeDebugStreamLine("%d",THROWEND);
				writeDebugStreamLine("%d",0);
			}
		}
		if (throwMode==3)
		{
			hardGrab=-1;
			newSeek=-1;
			clawSeek=-1;
			armSpeed=127;
			if (clawPercent<150)
				clawSpeed=-127;
			if (armPercent>THROWEND)
			{
				throwMode=4;
			}
		}
		if (throwMode==4)
		{
			throwMode=0;
			clawSpeed=0;
			armSpeed=0;
			clawSeek=150;
			holdArm=-1;
		}

		if (throwMode>=5 || throwMode<0) throwMode=0;

		if (vexRT[Btn7R]==1)
		{
			if (armPercent>28)
			{
				armSpeed=-127;
			}
		}


		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//	Auto High Hang
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (vexRT[Btn7D]==1)
		{
			holdArm=-1;
			if (hanggo==0) hanggo=1;
			else hanggo=2;
		}
		else if (vexRT[Btn7D]==0)
		{
			hanggo=0;
		}
		if ((hanggo==1)&&(hangmode==0))
		{
			e_command=DIZZY;
			playSoundFile("LIFT_ACTIVATED.wav");
			hanggo=2;
			hangmode=1;
			if ((startTile==1)||(startTile==2)||(startTile==5)||(startTile==6)||(startTile==9)||(startTile==11))
			{
				setGyro(2250);
				MoveDir=180+45;		//90
				FaceDir=225;	//////57
			}
			else
			{
				setGyro(1350);
				MoveDir=180+315;
				FaceDir=135;	//////57
			}
			//			resetGyro();
			DriveSpeed=127;
			DriveTime=1000;
			clearTimer(T2);
		}

		if (hangmode>=1)
		{
			if (hanggo==1)	//cancel!!!
			{
				clearTimer(T2);
				DriveTime=0;
				armSpeed=0;
				clawSpeed=0;
				hangmode=0;
				//				setGyro(1800);
			}
			TIPDIST=100000;	//override TIPDIST
		}

		if (hangmode==1)
		{
			//if ((SensorValue(poleDist)!=-1 && SensorValue(poleDist)<15)||time1(T2)>1000)
			{
				DriveTime=0;
				hangmode=2;
			}
		}

		if (hangmode==2)
		{
			clawSeek=450;
			if ((startTile==1)||(startTile==2)||(startTile==5)||(startTile==6)||(startTile==9)||(startTile==11))
			{
				MoveDir=180+45;		//90
				FaceDir=225;	//////57
			}
			else
			{
				MoveDir=180+315;
				FaceDir=135;	//////57
			}
			DriveSpeed=127;
			DriveTime=1000;

			armSpeed=(HANGFINISH-armPercent)*6;
			if (armSpeed>0)
			{
				if (armSpeed<40) armSpeed=40;
			}
			else
			{
				armSpeed=0;
				hangmode=3;
				DriveTime=0;
			}
		}

		if(hangmode==3)
		{
			if ((startTile==1)||(startTile==2)||(startTile==5)||(startTile==6)||(startTile==9)||(startTile==11))
			{
				MoveDir=180+225;		//90
				FaceDir=225;	//////57
			}
			else
			{
				MoveDir=180+135;
				FaceDir=135;	//////57
			}
			DriveSpeed=127;
			DriveTime=1000;

			if (armPercent>5)
			{
				armSpeed=-127;
				}else if (armPercent>0){
				if (clawPercent<390)
				{
					clawSpeed=-127;
				}
				armSpeed=-127;
				}else{
				hangmode=4;
			}
		}
		if (hangmode==4)
		{
			if (clawPercent<430)
			{
				clawSpeed=-127;
				armSeek=1;
			}
			clawSeek=400;
			armSeek=1;
			hangmode=0;
			DriveTime=0;
			armSpeed=0;
		}



		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		///////////////////////////////////////////MANUAL CONTROLS///////////////////////////
		if (vexRT[Btn5U]==1 && vexRT[Btn5D]==1)
		{
			throwMode=1;
			}else{
			if (vexRT[Btn5U]==1)	//do right arm
			{
				clawSpeed=127;
			}else if (vexRT[Btn5D]==1 && throwMode==0)
			{
				clawSpeed=-127;
			}
		}

		if (vexRT[Btn8L]==1)	//do right arm
		{
			armSpeed=127;
			armDownCount=0;
			}else if (vexRT[Btn8D]==1){
			armSpeed=-127;
		}

		motorSlew[claw1]=clawSpeed;
		motorSlew[claw2]=clawSpeed;
		motorSlew[right1] = armSpeed;
		motorSlew[right2]=armSpeed;
		motorSlew[left1] = armSpeed;
		motorSlew[left2]=armSpeed;

		if (vexRT[Btn8U]==1)
		{
			if (vexRT[Btn7LXmtr2]==1)
			{
				motorSlew[left1] = 127;
			}
			if (vexRT[Btn7DXmtr2]==1)
			{
				motorSlew[left1] = -127;
			}
			if (vexRT[Btn7UXmtr2]==1)
			{
				motorSlew[left2] = 127;
			}
			if (vexRT[Btn7RXmtr2]==1)
			{
				motorSlew[left2] = -127;
			}

			if (vexRT[Btn8LXmtr2]==1)
			{
				motorSlew[right1] = 127;
			}
			if (vexRT[Btn8DXmtr2]==1)
			{
				motorSlew[right1] = -127;
			}
			if (vexRT[Btn8UXmtr2]==1)
			{
				motorSlew[right2] = 127;
			}
			if (vexRT[Btn8RXmtr2]==1)
			{
				motorSlew[right2] = -127;
			}
		}
		if (clawSpeed>100)
		{
			SensorValue(turnLight)=1;
			}else{

			SensorValue(turnLight)=0;
		}

		wait1Msec(50);
		lastClawPercent=clawPercent;
	}
}






void screen()
{
	bLCDBacklight=true;
	clearLCDLine(0);
	clearLCDLine(1);



	if (vexRT[Btn8U]==1)
	{
		displayLCDString(1,0," CALIBRATE ME!  ");
		displayLCDString(0,0,"    PLEASE!     ");
		}else{
		//displayLCDNumber(0,0,MoveDir);
		displayLCDNumber(0,0,gyroDir);
		displayLCDNumber(0,14,startTile);

		if (displaymode%3==0)
		{
			displayLCDNumber(1,0,SensorValue(leftLine));
			displayLCDNumber(1,5,SensorValue(rightLine));
			}else if (displaymode%3==1){
			displayLCDNumber(1,0,armPercent);
			displayLCDNumber(1,5,clawPercent);
			minBattery=1000000;
			}else{
			//displayLCDNumber(1,0,SensorValue(leftBackLine));
			//displayLCDNumber(1,5,SensorValue(rightBackLine));
			if (nImmediateBatteryLevel<minBattery)
			{
				minBattery=nImmediateBatteryLevel;
			}
		}
		displayLCDNumber(1,10,SensorValue(poleDist));
		//displayLCDNumber(1,10,DriveTime);
	}
	wait1Msec(10);
}

/*task onWhiteLine()
{
while(true)
{
if (SensorValue[line]<=WHITELINE)
{
lineFound=1;
}else{
lineFound=0;
}
wait1Msec(25);
}
}*/



/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
//task usercontrol()
{
	//playSoundFile("windows.wav");
	int followingLine=0;
	int coolDownR=0;
	int coolDownL=0;
	int StrafeLeft=0;
	int StrafeRight=0;
	int ultraTimeOut=1000;
	//Declare variables
	InitAll();
	doneAutonomous=1;
	deployed=1;
	DriveTime=0;
	clearTimer(T1);
	TabPoint=0;
	nextCommand=1;

	////////////////////
	//    TESTING     //
	//  startTile=9;  //
	////////////////////

	if (startTile==1) {autoTab=&BIGRIGHTFRONT;}		//autonomous commands to use
	if (startTile==2) {autoTab=&BIGRIGHTBACK;}		//autonomous commands to use
	if (startTile==3) {autoTab=&BIGLEFTBACK;}		//autonomous commands to use
	if (startTile==4) {autoTab=&BIGLEFTFRONT;}		//autonomous commands to use

	if (startTile==5) {autoTab=&SMALLRIGHTFRONT;}		//autonomous commands to use
	if (startTile==6) {autoTab=&SMALLRIGHTBACK;}		//autonomous commands to use
	if (startTile==7) {autoTab=&SMALLLEFTBACK;}		//autonomous commands to use
	if (startTile==8) {autoTab=&SMALLLEFTFRONT;}		//autonomous commands to use

	if (startTile==9) {autoTab=&HANGRIGHT;}		//autonomous commands to use
	if (startTile==10) {autoTab=&HANGLEFT;}		//autonomous commands to use

	if (startTile==11) {autoTab=&DESTROYRIGHT;}		//autonomous commands to use
	if (startTile==12) {autoTab=&DESTROYLEFT;}		//autonomous commands to use

	if (startTile==13) {autoTab=&SKILLS;}		//autonomous commands to use


	startTask(readGyro);
	startTask(slewMotors);
	startTask(newDrive);
	startTask(doAutoArm);
	startTask(eyeLoop);

	//set truedir from table
	setGyro((autoTab[0])*10);
	//	truedir=autoTab[0]*10;			//so the robot knows which way it's pointing at the start
	autoTab+=1;//setGyro(500);
	if (true)//attackOrNot==1)
	{
		while (true)
		{
			screen();
			/////////////////////////////////////
			//process autonomous command table
			/////////////////////////////////////
			if (nextCommand) 											//ready for new command. Some commands below will set nextCommand immediately
				//whereas others (e.g. PAUSE) wait for timers to finish before asking for
			//the next command.
			{
				nextCommand=0;											//command processing
				switch(autoTab[TabPoint]){					//read command from table
				case DRIVE:													//process DRIVE command
					MoveDir=autoTab[TabPoint+1];			//read 'direction to move in' from table
					MoveDir=180+MoveDir;
					if (MoveDir>360){MoveDir-=360;}
					if (MoveDir<0){MoveDir+=360;}
					FaceDir=autoTab[TabPoint+2];			//read 'direction to face' from table
					DriveTime=autoTab[TabPoint+3];		//read 'time to drive for' from table. NOTE, if this is negative it
					//represents a conditonal command like looking for a white line
					DriveSpeed=127;    							 	//full speed
					if (DriveTime==RIGHTULTRA||DriveTime==LEFTULTRA)
					{
						sonarDist=autoTab[TabPoint+4];
						ultraTimeOut=autoTab[TabPoint+5];
						TabPoint+=6;
						}else{
						TabPoint+=4;           						//ready for next command in table
					}
					clearTimer(T2);										//reset timer so it can be used to check length of time driving
					break;
				case SLOW:													//process SLOW command. All params same as DRIVE except DriveSpeed.
					MoveDir=autoTab[TabPoint+1];
					MoveDir=180+MoveDir;
					if (MoveDir>360){MoveDir-=360;}
					if (MoveDir<0){MoveDir+=360;}
					FaceDir=autoTab[TabPoint+2];
					DriveTime=autoTab[TabPoint+3];
					DriveSpeed=55; 			   						//slow speed
					if (DriveTime==RIGHTULTRA||DriveTime==LEFTULTRA)
					{
						sonarDist=autoTab[TabPoint+4];
						TabPoint+=5;
						}else{
						TabPoint+=4;           						//ready for next command in table
					}
					clearTimer(T2);
					break;
				case TURN:													//process TURN command. All params same as DRIVE except DriveSpeed
					MoveDir=0;
					FaceDir=autoTab[TabPoint+1];
					DriveTime=autoTab[TabPoint+2];
					DriveSpeed=0; 			    					//zero speed so will just turn on the spot
					TabPoint+=3;
					clearTimer(T2);
					break;
				case PAUSE:													//process PAUSE command.
					PauseTime=autoTab[TabPoint+1];		//read 'pause length' from table
					TabPoint+=2;											//ready for next command in table
					clearTimer(T2);										//reset timer so it can be used to measure length of pause
					break;
				case CLAWPOS:												//process CLAWPOS command.
					clawSeek=autoTab[TabPoint+1];
					if (clawSeek!=CLOSE) hardGrab=0;
					TabPoint+=2;											//ready for next command in table
					nextCommand=1;										//immediately ask to process next command
					break;
				case THROW:												//process CLAWPOS command.
					throwMode=1;
					TabPoint+=1;											//ready for next command in table
					nextCommand=1;										//immediately ask to process next command
					break;
				case HANG:												//process CLAWPOS command.
					TabPoint+=1;											//ready for next command in table
					nextCommand=1;										//immediately ask to process next command
					playSoundFile("LIFT_ACTIVATED.wav");
					hangmode=1;
					if ((startTile==1)||(startTile==2)||(startTile==5)||(startTile==6)||(startTile==9)||(startTile==11))
					{
						setGyro(2250);
						MoveDir=180+45;		//90
						FaceDir=225;	//////57
					}
					else
					{
						setGyro(1350);
						MoveDir=180+315;
						FaceDir=135;	//////57
					}
					//			resetGyro();
					DriveSpeed=127;
					DriveTime=1000;
					clearTimer(T2);
					break;
				case ARMPOS:												//process CLAWPOS command.
					armSeek=autoTab[TabPoint+1];
					TabPoint+=2;											//ready for next command in table
					nextCommand=1;										//immediately ask to process next command
					break;
				case SETGYRO:												//process SETGYRO command.
					setGyro((autoTab[TabPoint+1])*10);
					//					truedir=autoTab[TabPoint+1];			//sets gyro direction based on parameter read from command table.
					//this is used to recalibrate gyro when robot direction is known.
					TabPoint+=2;											//ready for next command in table
					nextCommand=1;										//immediately ask to process next command
					break;
				case LINEFOLLOW:
					MoveDir=autoTab[TabPoint+1];			//read 'direction to move in' from table
					MoveDir=180+MoveDir;
					followingLine=1;
					if (MoveDir>360){MoveDir-=360;}
					if (MoveDir<0){MoveDir+=360;}
					FaceDir=autoTab[TabPoint+2];			//read 'direction to face' from table
					DriveTime=autoTab[TabPoint+3];		//read 'time to drive for' from table. NOTE, if this is negative it
					//represents a conditonal command like looking for a white line
					DriveSpeed=127;    							 	//full speed
					clearTimer(T2);
					TabPoint+=4;
					break;
				case END:
					//Do nothing
					break;
				}
			}




			motorSlew[claw1] = clawSpeed;
			motorSlew[claw2] = clawSpeed;
			motorSlew[right1] = armSpeed;
			motorSlew[right2] = armSpeed;
			motorSlew[left1] = armSpeed;
			motorSlew[left2] = armSpeed;



			if (PauseTime>0)
			{
				if (time1(T2)>=PauseTime*1000)
				{
					PauseTime=0;
					nextCommand=1;
				}
			}

			if (coolDownL>=0) coolDownL--;
			if (coolDownR>=0) coolDownR--;

			if (DriveTime>0 && followingLine==1)
			{
				if (SensorValue(rightLine)<=RIGHTWHITELINE)
				{
					if (coolDownR<0)
					{
						MoveDir=MoveDir-15;
						FaceDir=FaceDir-15;
						coolDownR=60;
						StrafeLeft=1;
					}
				}
				if (SensorValue(leftLine)<=LEFTWHITELINE)
				{
					if (coolDownL<0)
					{
						MoveDir=MoveDir+15;
						FaceDir=FaceDir+15;
						coolDownL=60;
						StrafeRight=1;
					}
				}
			}

			if (StrafeLeft==1)
			{
				StrafeLeft=2;
				MoveDir=MoveDir-30;
			}
			if (StrafeLeft==2)
			{
				if (coolDownR==1)
				{
					MoveDir=MoveDir+30;
					StrafeLeft=0;
				}
			}
			if (StrafeRight==1)
			{
				StrafeRight=2;
				MoveDir=MoveDir+30;
			}
			if (StrafeRight==2)
			{
				if (coolDownL==1)
				{
					MoveDir=MoveDir-30;
					StrafeRight=0;
				}
			}

			if (DriveTime!=0)
			{
				if (DriveTime>0)
				{
					if (time1(T2)>=DriveTime*1000)
					{
						DriveTime=0;
						followingLine=0;
						nextCommand=1;
					}
				}else if (DriveTime==RIGHTWHITE)
				{
					if (SensorValue(rightLine)<=RIGHTWHITELINE)
					{
						DriveTime=0;
						nextCommand=1;
					}
				}else if (DriveTime==LEFTWHITE)
				{
					if (SensorValue(leftLine)<=LEFTWHITELINE)
					{
						DriveTime=0;
						nextCommand=1;
					}
				}else if (DriveTime==LEFTULTRA)
				{
					if ((SensorValue(poleDist)<=sonarDist&&SensorValue(poleDist)!=-1) || (time1(T2)>=ultraTimeOut*1000))
					{
						DriveTime=0;
						nextCommand=1;
					}
				}else if (DriveTime==RIGHTULTRA)
				{
					if ((SensorValue(rightDist)<=sonarDist&&SensorValue(rightDist)!=-1)  || (time1(T2)>=ultraTimeOut*1000))
					{
						DriveTime=0;
						nextCommand=1;
					}
				}else if (DriveTime==BLACK)
				{
					if (lineFound==0)
					{
						DriveTime=0;
						nextCommand=1;
					}
				}else if (DriveTime==HITWALL)
				{
					//if (SensorValue(didHitWall)==1)
					{
						DriveTime=0;
						nextCommand=1;
					}
				}
			}
		}
	}
}






/////////////////////////////////////////////////////////////////////////////////////////

//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
//task autonomous()
{
	if (doneAutonomous==0)
	{
		//playSoundFile("windows.wav");
	}
	throwMode=0;
	PauseTime=0;
	//DriveTime=0;
	int lastPressed=0;
	clearTimer(T1);
	clearTimer(T4);

	//startTask(sounds);
	startTask(turnCircle);
	startTask(debug);
	startTask(slewMotors);
	//setGyro(0);
	//	truedir=0;
	startTask(readGyro);
	startTask(newDrive);
	startTask(doAutoArm);
	startTask(eyeLoop);

	while(1 == 1)
	{

		XACCEL = vexRT[AccelX];
		YACCEL = vexRT[AccelY];
		ZACCEL = vexRT[AccelZ];

		screen();
		//DriveTime=0;
		PauseTime=0;

		///////////////////////
		// Initial setup mode
		///////////////////////

		if (doneAutonomous == 0)
		{
			//		while (doneSetup==0)
			{
				if (nLCDButtons==1 && nLCDButtons!=lastPressed)
				{
					playTone(1000,10);
					if (startTile<13)
					{
						startTile++;
						}else{
						startTile=1;
					}
				}

				if (nLCDButtons==2 && nLCDButtons!=lastPressed)
				{
					displaymode++;
					playTone(1000,10);
				}
				if (nLCDButtons==4 && lastPressed!=nLCDButtons && vexRT[Btn8U]==1)
				{
					playTone(1000,10);
					doneSetup=0;
					InitAll();
				}
				/*if (vexRT[Btn7D]==1)
				{
				doneSetup=0;
				InitAll();
				}*/

				lastPressed=nLCDButtons;
			}
		}

		//		if (vexRT[Btn7D]==1)
		//		{
		//				clawSeek=169;
		//				armFind=127;
		//		}





		if (vexRT[Btn8U]==1)		//reset gyro
		{
			resetGyro();
			e_command=DIZZY;
		}
	}
}

/*

once the robot is connected to Robot C, click on Robot, then lego brick, and then navigate to the file management utility. 
Upload userword.txt with the word that you want the robot to draw in morse code. 

*/
#include "PC_FileIO.c"

const int MAX_CHAR = 20;
const int MAX_DOT_DASH = 6;
const int MOTOR_POWER = 10;
const int MOTOR_POWER_READ = 10;
const int PEN_MOTOR_POWER = -10;
const int PEN_MOVE_WAIT = 1750; // cm
const int DRAW_DIST = 1; // 1 cm per '1' according to the pseudo binary
const int SPACE_WITHIN_LETTERS = 1;
const int SPACE_BETWEEN_LETTERS = 3;
const int DOT_NUM = 1;
const int DASH_NUM = 3;


const string DOT_READ = ".";
const string DASH_READ = "-";
const float DOT_LENGTH = 2.54;
const float WHITE_SPACE_DOT_DASH = 2.54; // distance between dots and dashes in cm
const float TOL = 1;
int shutDown = 0;

//alphabet1: abcdefghijklmnopqrst
//alphabet2: uvwxyz0123456789.,?

const char alphabet1Eng[MAX_CHAR] = {'a',
				     'b',
				     'c',
				     'd',
				     'e',
				     'f',
				     'g',
				     'h',
				     'i',  
				     'j',
				     'k',
				     'l',
				     'm',
				     'n',
				     'o',
				     'p',
				     'q',
				     'r',
				     's',
				     't'};

const string alphabet1Morse[MAX_CHAR] = {".-", //a
					 "-...", //b
					 "-.-.", //c
					 "-..", //d
					 ".", //e
					 "..-.", //f
					 "--.", //g
					 "....", //h
					 "..", //i
					 ".---", //j
					 "-.-", //k
					 ".-..", //l
					 "--", //m
					 "-.", //n
					 "---", //o
					 ".--.", //p
					 "--.-", //q
					 ".-.", //r
					 "...", //s
					 "-"}; //t

const char alphabet2Eng[MAX_CHAR] = {'u',
				     'v',
				     'w',
				     'x',
				     'y',
				     'z',
				     '0',
				     '1',
				     '2',
				     '3',
				     '4',
				     '5',
				     '6',
				     '7',
				     '8',
				     '9',
				     '.',
				     ',',
				     '?',
				     '!'};

const string alphabet2Morse[MAX_CHAR] = {"..-", //u
					 "...-", //v
					 ".--", //w
					 "-..-", //x
					 "-.--", //y
					 "--..", //z
					 "-----", //0
					 ".----", //1
					 "..---", //2
					 "...--", //3
					 "....-", //4
					 ".....", //5
					 "-....", //6
					 "--...", //7
					 "---..", //8
					 "----.", //9
					 ".-.-.-", //.
					 "--..--", //,
					 "..--..", //?
					 "-.-.--"}; //!

void configureSensors();
int readFromFile(char * updateWord);
int pathDecision();
void travel (int motorPower, int dist);
void powerSetting(int motorPower);
void moveRobotForwardWhenDrawing(char * currCode);
void letterMatch(char * userWord, int wordSize);
void travelPen(int PEN_MOTOR_POWER);
void readMorseCode();


task main()
{
	configureSensors();

		while(!getButtonPress(buttonEnter))
		{
			int decision = pathDecision();

				if (decision == 1) // left, reading through color sensor
			  {
					readMorseCode();
				}
				else // right, outputting
				{
					// the max character that you can have is 20 so declare a 1D array of 20 indexes to store letters in word
					char userWord[MAX_CHAR];
					
					// call readfrom file to read in the data and convert into a split array of just letters
					int wordSize = readFromFile(userWord);

					// matches the letters to their morse code + travels based on the words
					letterMatch(userWord, wordSize);
				}
		}
}

void shutdownCheck()
{
	if (getButtonPress(buttonEnter))
	{
		shutDown = 1;
		powerSetting(0);
		motor[motorB]= 0;
		motor[motorA]= 0;
		motor[motorD] = 0;
		while(shutDown == 1)
		{
			displayString(10, "SHUT DOWN ACTIVATED");
		}

	}
}
// Configures the color sensor
void configureSensors()
{
  SensorType[S4] = sensorEV3_Color;
  wait1Msec(50);
  // check which mode
  SensorMode[S4] = modeEV3Color_Color;
  wait1Msec(50);
  shutdownCheck();
}


// Reads the word from the file into a char array
int readFromFile(char * updateWord)
{
	shutdownCheck();
	TFileHandle fin;

	bool fileAvailable = openReadPC(fin,"userword.txt");

	int wordSize = 0;

	if(!fileAvailable)
	{
		displayString(5, "Error!");
		wait1Msec(5000);
		shutdownCheck();
	}
	else
	{

		readIntPC(fin, wordSize);

		char letterFromWord = 'a';

		for(int index = 0; index < wordSize; index++)
		{
			readCharPC(fin, letterFromWord);
			updateWord[index] = letterFromWord;
			shutdownCheck();
		}
	}
	wait1Msec(10000);
	closeFilePC(fin);
	return wordSize;
	shutdownCheck();
}


int pathDecision()
{
    shutdownCheck();
    int right_or_left = 0;
    while (!getButtonPress(buttonRight) && !getButtonPress(buttonLeft) && shutDown == 0)
    {}

    if (!getButtonPress(buttonRight) == true)
    {
        right_or_left = 1;
        shutdownCheck();
    }

    while (getButtonPress(buttonRight) && getButtonPress(buttonLeft) && shutDown == 0)
    {}

    wait1Msec(1000);

    shutdownCheck();
    return right_or_left;
}

void travel (int motorPower, int dist)
{
	  shutdownCheck();
    const float WHEEL_RADIUS = 2.75;
    const int HALF_TURN = 180;

    time1[T1] = 0;
    int ENC_LIMIT = dist * (HALF_TURN / (PI * WHEEL_RADIUS));
		//displayString(2, "%d", ENC_LIMIT);

    nMotorEncoder[motorA] = 0;

    shutdownCheck();
    powerSetting(motorPower);

    while(nMotorEncoder[motorA] < ENC_LIMIT)
    {}
    powerSetting(0);
}

// moves the motor forward based on motorpower

void powerSetting(int motorPower)
{
	  shutdownCheck();
    motor[motorA] = motor[motorD] = motorPower;
}

// breaks the dots and dashes of each letter into the pseudo binary and travels based on the pseudo binary

void moveRobotForwardWhenDrawing(char * currCode)
{
	shutdownCheck();
	for (int count1 = 0; count1 < MAX_DOT_DASH; count1++)
	{
		shutdownCheck();
		char someChar = currCode[count1];
		if (someChar == '.' || someChar == '-')
		{
			// travels forwards three '1's if it is a dash
			if (someChar == '-')
			{
				shutdownCheck();
				travelPen(PEN_MOTOR_POWER);
				travel(MOTOR_POWER, DASH_NUM*DRAW_DIST);
				travelPen(-1*PEN_MOTOR_POWER);
			}
			else // travels forward one '1' if it is a dot
			{
				shutdownCheck();
				travelPen(PEN_MOTOR_POWER);
				travel(MOTOR_POWER, DOT_NUM*DRAW_DIST);
				travelPen(-1*PEN_MOTOR_POWER);
			}
			// travels space between dots and dashes within a letter (1 empty space)
			travel(MOTOR_POWER, SPACE_WITHIN_LETTERS*DRAW_DIST);
		}
	}
	travel(MOTOR_POWER, SPACE_BETWEEN_LETTERS*DRAW_DIST);
	shutdownCheck();
}

void letterMatch(char * userWord, int wordSize)
{
	while(shutDown == 1)
	{
		displayString(10, "SHUT DOWN ACTIVATED");
	}
	for (int count = 0; count < wordSize; count++)
	{
		shutdownCheck();
		for (int count2 = 0; count2 < MAX_DOT_DASH; count2++)
		{
			shutdownCheck();
		}
		displayString(0, "%s", "Word:");
		displayString(1, "%s", userWord);

		displayString(4, "%s", "Current Letter: ");
		displayString(5, "%c", userWord[count]);
		
		// checks every value in the array
		for (int count1 = 0; count1 < MAX_CHAR ; count1++)
		{
			shutdownCheck();
		
			// checks if the letter matches a letter in the alphabet1 array
			if (userWord[count] == alphabet1Eng[count1])
			{
				shutdownCheck();
				// splits the alphabet's morse code into a char array of dots and dashes
				string currCode = alphabet1Morse[count1];
				moveRobotForwardWhenDrawing(currCode);
			}
			// checks if the letter matches a letter in the alphabet2 array
			else if (userWord[count] == alphabet2Eng[count1])
			{
				shutdownCheck();
				// splits the alphabet's morse code into a char array of dots and dashes
				string currCode = alphabet2Morse[count1];
				moveRobotForwardWhenDrawing(currCode);
			}
		}
	}
}

// Move the pen up and down
void travelPen (int PEN_MOTOR_POWER)
{
	shutdownCheck();
	motor[motorB] = PEN_MOTOR_POWER;
	wait1Msec(PEN_MOVE_WAIT);

	motor[motorB] = 0;
}


// Reads morse code
void readMorseCode()
{
	// The robot begins travelling forward until it hits black for the first time
	shutdownCheck();
	powerSetting(MOTOR_POWER_READ);
	while(SensorValue[S4] == (int)colorWhite)
	{}

	string readLetterMorse = "";
	char addLetter = ' ';

	// Main loops starts here, runs until reaches color RED, which indicates to finish reading
	while(SensorValue[S4] != (int)colorRed)
	{
		shutdownCheck();
		string readDotOrDash = "";
		float distTravelled = 0;
		float distTravelledWhiteSpace = 0;

		nMotorEncoder[motorA] = 0;
		powerSetting(MOTOR_POWER_READ);
		while(SensorValue[S4] != (int)colorWhite && SensorValue[S4] != (int)colorRed) // go while seeing black
		{}
    		shutdownCheck();

		powerSetting(0);

		distTravelled = nMotorEncoder[motorA] * (PI*2.75) / 180; //measures the distance of the black dot or dash
		displayString(0, "%s", "Group 34");
		displayString(1, "%s", "Olivia Y, Rijin M, Krish V, Mridul T");
		shutdownCheck();

		wait1Msec(1000);

		// checks the distance against tolerances to determine if dot or dash
		if (fabs(distTravelled - DOT_LENGTH) < TOL)
		{
			shutdownCheck();
			readDotOrDash = DOT_READ; 
			/*
			if the distance travelled is = to a dot length, then a dot would 
			be added to the string called readLetter
			*/ 
		}
		else
		{
			shutdownCheck();
			readDotOrDash = DASH_READ; // otherwise add a dash to readLetter
		}

		displayString(2, "%s", "Current dot or dash:");
		displayString(3, "%s", readDotOrDash);

		nMotorEncoder[motorA] = 0; // reset motor encoder to 0 so white space can be measured

		shutdownCheck();
		powerSetting(MOTOR_POWER_READ);
		while(SensorValue[S4] != (int)colorBlack && SensorValue[S4] != (int)colorRed && shutDown == 0) //go while seeing white
		{}
		powerSetting(0);

		shutdownCheck();
		distTravelledWhiteSpace = nMotorEncoder[motorA] * (PI*2.75) / 180; //measure white space distance;

		shutdownCheck();
		readLetterMorse += readDotOrDash;
		displayString(6, "%s", "Code:");
		displayString(7, "%s", readLetterMorse);

		if (fabs(distTravelledWhiteSpace - WHITE_SPACE_DOT_DASH) > TOL) // white space is more than one inch means letter is done
		{
			shutdownCheck();
			displayString(13, "%s", "white space is big");
			bool match = false;
			// compares the letter's morse against the two alphabet arrays
			for (int count = 0; count < MAX_CHAR && match == false; count++)
			{
				shutdownCheck();
				if (readLetterMorse == alphabet1Morse[count])
				{
			
					displayString(9, "%s", "Letter:");
					readLetterMorse = "";
					addLetter = alphabet1Eng[count];
					displayString(10, "%c", addLetter);
					match = true;
					addLetter = ' ';
				}
				else if (readLetterMorse == alphabet2Morse[count])
				{
					displayString(9, "%s", "Letter: ");
					readLetterMorse = "";
					addLetter = alphabet2Eng[count];
					displayString(10, "%c", addLetter);
					match = true;
					addLetter = ' ';
				}
			}
		}
	}
}

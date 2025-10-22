#include <Arduino.h>
#include <objects.h>
#include <BluetoothSerial.h>
#include <Preferences.h>

//Globals
MotorShield motors;
BluetoothSerial server;
Preferences data;

//VARIABLES
#define NOMBRE_BLUETOOTH "Senku-Bot"
int LEFT_SPEED = 250;
int RIGHT_SPEED = 250;

//Bluetooth stop
#define CONECTION_DELAY 5000
unsigned long time_now=0, last_check=0;
bool running = false;


//==========================================
//Functions
void handleBluetoothStop();
void handleCommand(String command);
void handleConfig(String command);
void spinWeapon(int power);


//==========================================
//MAIN PROGRAM

void setup(){
    Serial.begin(115200);
    server.begin(NOMBRE_BLUETOOTH);
    data.begin("SenkuBot");
    delay(500);

    LEFT_SPEED = data.getInt("L", 250);
    RIGHT_SPEED = data.getInt("R", 250);

    //Init all motors
    motors.init(STNDBY_PIN, PWM_A, A1_PIN, A2_PIN, PWM_B, B1_PIN, B2_PIN);

    ledcSetup(WEAPON_CHANNEL, WEAPON_FREC, WEAPON_RES);
    ledcAttachPin(WEAPON_PIN, WEAPON_CHANNEL);

    ledcWrite(WEAPON_CHANNEL, MIN_POWER);
    delay(2000);
    Serial.println("Systems ready!\n");
}


void loop(){
  time_now = get_time();

  while(server.available()){
    last_check = time_now;
    handleCommand(server.readStringUntil('\n'));
  }

  //Check if bluetooth lost conection
  handleBluetoothStop();
}

//==========================================
//Implementations

void handleCommand(String command){
  //Config
  if(command.length() > 2 && (command[1] >= '0' && command[1] <= '9')){
    handleConfig(command);
    return;
  }

  char letter = command[0];
  //STOP
  if(letter == 'S'){
      motors.stopMotors();
      running = false;
      Serial.println("STOP!\n");
      return;
  }

  running = true;
  switch (letter)
  {
  case 'F':
      motors.controlMotors(LEFT_SPEED, RIGHT_SPEED);
      Serial.println("Forward!\n");
      break;
    case 'B':
      motors.controlMotors(-LEFT_SPEED, -RIGHT_SPEED);
      Serial.println("Backwards!\n");
      break;
    case 'L':
      motors.controlMotors(-LEFT_SPEED, RIGHT_SPEED);
      Serial.println("Turning Left!\n");
      break;
    case 'R':
      motors.controlMotors(LEFT_SPEED, -RIGHT_SPEED);
      Serial.println("Turning right!\n");
      break;
    case 'W':
        Serial.println("Activating weapon!\n");
        spinWeapon(ATTACK_POWER);
        break;
    case 'w':
        Serial.println("WEAPON OFF!\n");
        spinWeapon(0);
        break;
  //------------------------
  default:
      break;
  }
}


void handleBluetoothStop(){
  if(running && (time_now-last_check) > CONECTION_DELAY){
    Serial.println("Disconnect error stop!");
    motors.stopMotors();
    running = false;
    ledcWrite(WEAPON_CHANNEL, MIN_POWER);
    ledcDetachPin(WEAPON_CHANNEL);
  }
}


void spinWeapon(int power){
    int duty_cycle = map(power, 0, 100, MIN_POWER, MAX_POWER);
    ledcWrite(WEAPON_CHANNEL, duty_cycle);
    Serial.println("Weapon PWM: " + String(duty_cycle));
}


void handleConfig(String command){
  command.trim();
  char letter = command[0];
  int split = command.indexOf(" ");

  if(letter == 'M'){
    //Motor config
    LEFT_SPEED = constrain(command.substring(1, split).toInt(), 0, 255);
    RIGHT_SPEED = constrain(command.substring(split+1).toInt(), 0, 255);

    data.putInt("L", LEFT_SPEED);
    data.putInt("R", RIGHT_SPEED);

    Serial.println("Set motor speeds: " + String(LEFT_SPEED) + " " + String(RIGHT_SPEED));
  }
  
}
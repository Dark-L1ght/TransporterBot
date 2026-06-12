#include <Servo.h>
#include <SoftwareSerial.h>

//Define Kontroler
#define CMD_FORWARD 'F'
#define CMD_BACKWARD 'B'
#define CMD_LEFT 'L'
#define CMD_RIGHT 'R'
#define CMD_CIRCLE 'C'
#define CMD_CROSS 'X'
#define CMD_TRIANGLE 'T'
#define CMD_SQUARE 'S'
#define CMD_START 'A'
#define CMD_PAUSE 'P'

// ==================== KONFIGURASI PIN ====================
// Bluetooth HC-05 Configuration
SoftwareSerial bluetooth(11, 12); // RX, TX (menggunakan D11, D12 - bebas konflik Serial)

struct MotorConfig {
    // L298N Pin Connections
    const uint8_t ENA = 2; // PWM motor kiri
    const uint8_t IN1 = 3;
    const uint8_t IN2 = 4;
    const uint8_t IN3 = 5;
    const uint8_t IN4 = 6;
    const uint8_t ENB = 7; // PWM motor kanan
    
    // Motor constants
    const uint8_t DEFAULT_SPEED = 140;
    const uint8_t MAX_SPEED = 255;
    const uint8_t MIN_SPEED = 0;
} motor;

struct GripperConfig {
    // Servo Pin Connections
    const uint8_t GRIPPER_RIGHT_PIN = 8;
    const uint8_t GRIPPER_LEFT_PIN = 9;
    const uint8_t LIFT_PIN = 10;
    
    // Nilai sudut untuk servo (0-180 derajat)
    const uint8_t GRIPPER_LEFT_OPEN = 20;
    const uint8_t GRIPPER_RIGHT_OPEN = 160;
    const uint8_t GRIPPER_LEFT_CLOSE = 51;
    const uint8_t GRIPPER_RIGHT_CLOSE = 129;
    const uint8_t GRIPPER_LEFT_CLB = 0;
    const uint8_t GRIPPER_RIGHT_CLB = 180;
    const uint8_t LIFT_UP = 180;
    const uint8_t LIFT_DOWN = 95;
} gripper;

// ==================== DEKLARASI OBJEK ====================
Servo gripperLeftServo;
Servo gripperRightServo;
Servo liftServo;

// ==================== VARIABEL GLOBAL ====================
uint8_t currentSpeed = motor.DEFAULT_SPEED;
bool gripperOpen = true;    
bool liftUp = false;
bool bluetoothMode = true; 

// ==================== DEKLARASI FUNGSI ====================
void sendBluetooth(String message);
void processBluetoothCommand(char command);

// ==================== CLASS GRIPPER CONTROLLER ====================
class GripperController {
public:
    void init() {
        gripperLeftServo.attach(gripper.GRIPPER_LEFT_PIN);
        gripperRightServo.attach(gripper.GRIPPER_RIGHT_PIN);
        liftServo.attach(gripper.LIFT_PIN);
    }
    
    void openGripper() {
        gripperLeftServo.write(gripper.GRIPPER_LEFT_OPEN);
        gripperRightServo.write(gripper.GRIPPER_RIGHT_OPEN);
        gripperOpen = true;
        sendBluetooth("Gripper OPENED");
    }
    
    void closeGripper() {
        gripperLeftServo.write(gripper.GRIPPER_LEFT_CLOSE);
        gripperRightServo.write(gripper.GRIPPER_RIGHT_CLOSE);
        gripperOpen = false;
        sendBluetooth("Gripper CLOSED");
    }

    void calibrateGripper() {
        gripperLeftServo.write(gripper.GRIPPER_LEFT_CLB);
        gripperRightServo.write(gripper.GRIPPER_RIGHT_CLB);
        sendBluetooth("Kalibrasi Gripper");
    }
    
    void liftUpFunc() {
        liftServo.write(gripper.LIFT_UP);
        liftUp = true;
        sendBluetooth("Lift UP");
    }
    
    void liftDownFunc() {
        liftServo.write(gripper.LIFT_DOWN);
        liftUp = false;
        sendBluetooth("Lift DOWN");
    }
    
    bool getGripperState() { return gripperOpen; }
    bool getLiftState() { return liftUp; }
} gripperCtrl;

// ==================== CLASS MOTOR CONTROLLER ====================
class MotorController {
private:
    void setMotorDirection(bool leftForward, bool rightForward) {
        // Motor kiri (A)
        if (leftForward) {
            digitalWrite(motor.IN1, HIGH);
            digitalWrite(motor.IN2, LOW);
        } else {
            digitalWrite(motor.IN1, LOW);
            digitalWrite(motor.IN2, HIGH);
        }
        
        // Motor kanan (B)
        if (rightForward) {
            digitalWrite(motor.IN3, HIGH);
            digitalWrite(motor.IN4, LOW);
        } else {
            digitalWrite(motor.IN3, LOW);
            digitalWrite(motor.IN4, HIGH);
        }
    }
    
public:
    void setSpeed(uint8_t leftSpeed, uint8_t rightSpeed) {
        analogWrite(motor.ENA, leftSpeed);
        analogWrite(motor.ENB, rightSpeed);
    }
    
    void moveForward(uint8_t speed = currentSpeed) {
        setSpeed(speed, speed);
        setMotorDirection(true, true);
        sendBluetooth("FWD");
    }
    
    void moveBackward(uint8_t speed = currentSpeed) {
        setSpeed(speed, speed);
        setMotorDirection(false, false);
        sendBluetooth("BWD");
    }
    
    void turnLeft(uint8_t speed = currentSpeed) {
        setSpeed(speed, speed);
        setMotorDirection(false, true);
        sendBluetooth("LEFT");
    }
    
    void turnRight(uint8_t speed = currentSpeed) {
        setSpeed(speed, speed);
        setMotorDirection(true, false);
        sendBluetooth("RIGHT");
    }
    
    void stop() {
        digitalWrite(motor.IN1, LOW);
        digitalWrite(motor.IN2, LOW);
        digitalWrite(motor.IN3, LOW);
        digitalWrite(motor.IN4, LOW);
        analogWrite(motor.ENA, 0);
        analogWrite(motor.ENB, 0);
        sendBluetooth("STOP");
    }
} motorCtrl;

// ==================== IMPLEMENTASI FUNGSI BANTU ====================
void sendBluetooth(String message) {
    if (bluetoothMode && bluetooth) {
        bluetooth.println(message);
    }
    // Selalu cetak ke Serial Monitor untuk debugging
    Serial.print("BT OUT: ");
    Serial.println(message);
}

// ==================== INISIALISASI PIN ====================
void initializePins() {
    pinMode(motor.ENA, OUTPUT);
    pinMode(motor.IN1, OUTPUT);
    pinMode(motor.IN2, OUTPUT);
    pinMode(motor.IN3, OUTPUT);
    pinMode(motor.IN4, OUTPUT);
    pinMode(motor.ENB, OUTPUT);
    
    digitalWrite(motor.IN1, LOW);
    digitalWrite(motor.IN2, LOW);
    digitalWrite(motor.IN3, LOW);
    digitalWrite(motor.IN4, LOW);
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(9600);
    Serial.setTimeout(50); // Cegah parseInt() blocking lama
    bluetooth.begin(9600); 
    
    delay(1000);
    Serial.println("SYSTEM START...");

    initializePins();
    gripperCtrl.init(); // Init servo sekali di sini
    
    // Posisi awal
    gripperCtrl.openGripper();
    delay(500);
    gripperCtrl.liftDownFunc();
    delay(500);
    motorCtrl.stop();
    
    Serial.println("READY.");
}

// ==================== LOOP ====================
void loop() {
  if (Serial.available()) {
    char cmd = Serial.peek(); // Intip karakter pertama
    
    // Jika perintah diawali huruf 'V', itu adalah data slider PWM
    if (cmd == 'V') {
      Serial.read(); // Buang huruf 'V'
      int pwmValue = Serial.parseInt(); // Baca angkanya (0-255)
      
      // Update kecepatan motor (Tanpa ganggu servo)
      currentSpeed = constrain(pwmValue, motor.MIN_SPEED, motor.MAX_SPEED);
      motorCtrl.setSpeed(currentSpeed, currentSpeed); 
      
      // Buang newline setelah angka
      while(Serial.available() && Serial.peek() == '\n') Serial.read(); 
    }
    else {
      // Jika bukan V, berarti perintah biasa (F, B, L, R, dll)
      char command = Serial.read();
      processBluetoothCommand(command);
    }
  }
}

// ==================== PROCESS BLUETOOTH COMMANDS ====================
void processBluetoothCommand(char command) {
  switch (command) {
    case CMD_FORWARD:
        motorCtrl.moveForward();
        break;
    case CMD_BACKWARD:
        motorCtrl.moveBackward();
        break;
    case CMD_LEFT:
        motorCtrl.turnLeft();
        break;
    case CMD_RIGHT:
        motorCtrl.turnRight(); 
        break;
    case CMD_CIRCLE:
        gripperCtrl.openGripper();
        break;
    case CMD_CROSS: 
        gripperCtrl.liftDownFunc(); 
        break;
    case CMD_TRIANGLE:
        gripperCtrl.liftUpFunc();
        break;
    case CMD_SQUARE:
        gripperCtrl.closeGripper();
        break;
    case CMD_START:
        gripperCtrl.calibrateGripper();
        break;
    case CMD_PAUSE:
        motorCtrl.stop();
        break;
    default:
        break;
  }
}

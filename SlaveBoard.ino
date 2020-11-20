#include <Servo.h>
#include <SoftwareSerial.h>   //Software Serial Port

#define RxD 7
#define TxD 6
#define ConnStatus A1

#define DEBUG_ENABLED  1

Servo servoLeft;
Servo servoRight;
int shieldPairNumber = 6;


boolean ConnStatusSupported = true;   // Set to "true" when digital connection status is available on Arduino pin


String slaveNameCmd = "\r\n+STNA=Slave";   // This is concatenated with shieldPairNumber later

SoftwareSerial blueToothSerial(RxD,TxD);

char path[600];
int index = 0;
int obstacle = 1;
int back = 0;
int i =0;

void setup() {
    Serial.begin(9600);
    blueToothSerial.begin(38400);                    // Set Bluetooth module to default baud rate 38400
    
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(ConnStatus, INPUT);
    pinMode(10, INPUT);
    pinMode(9, OUTPUT);
    pinMode(3, INPUT);
    pinMode(2, OUTPUT);
    //  Check whether Master and Slave are already connected by polling the ConnStatus pin (A1 on SeeedStudio v1 shield)
    //  This prevents running the full connection setup routine if not necessary.

    if(ConnStatusSupported) Serial.println("Checking Slave-Master connection status.");

    if(ConnStatusSupported && digitalRead(ConnStatus)==1)
    {
        Serial.println("Already connected to Master - remove USB cable if reboot of Master Bluetooth required.");
    }
    else
    {
        Serial.println("Not connected to Master.");
        
        setupBlueToothConnection();   // Set up the local (slave) Bluetooth module

        delay(1000);                  // Wait one second and flush the serial buffers
        Serial.flush();
        blueToothSerial.flush();
    }

}


void loop() {
    char recvChar;
    servoLeft.attach(13);
    servoRight.attach(12);
    while(obstacle == 1)
    {
        if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
        {
            recvChar = blueToothSerial.read();
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300);

            //forward-left
            if (recvChar == 'q'){
              path[index] = 'q';
              index += 1;
              servoLeft.writeMicroseconds(1500);
              servoRight.writeMicroseconds(1300);
              delay(300);
            }
            //forward-right
            else if (recvChar == 'w'){
              path[index] = 'w';
              index += 1;
              servoLeft.writeMicroseconds(1700);
              servoRight.writeMicroseconds(1500);
              delay(300);
            }
            //back-left
            else if (recvChar == 'e'){
              path[index] = 'e';
              index += 1;
              servoLeft.writeMicroseconds(1500);
              servoRight.writeMicroseconds(1700);
              delay(300);
              servoLeft.writeMicroseconds(1300);
              servoRight.writeMicroseconds(1700);
              delay(300);
            }
            //back-right
            else if (recvChar == 'r'){
              path[index] = 'r';
              index += 1;
              servoLeft.writeMicroseconds(1300);
              servoRight.writeMicroseconds(1500);
              delay(300);
              servoLeft.writeMicroseconds(1300);
              servoRight.writeMicroseconds(1700);
              delay(300);
            }
              else if (recvChar == 'g'){
              obstacle = 0;
            }
            else if (recvChar == 'b'){
              back = 1;
            }
            else{
              path[index] = ' ';
              index += 1;
              }
       }

        if(Serial.available())            // Check if there's any data sent from the local serial terminal. You can add the other applications here.
        {
            recvChar  = Serial.read();
            Serial.print(recvChar);
            blueToothSerial.print(recvChar);
        }
    }
    if (obstacle == 0){
      int irLeft = irDetect(9, 10, 38000);
      int irRight = irDetect(2, 3, 38000);
      //move forward
      if (irLeft == 1 && irRight == 1){
          servoLeft.writeMicroseconds(1525);
          servoRight.writeMicroseconds(1475);
          delay(100);
      }
      //move right
      else if (irLeft == 0 && irRight==1){
              servoLeft.writeMicroseconds(1700);
              servoRight.writeMicroseconds(1500);
              delay(300);
      }
      //move left
      else if (irLeft == 1 && irRight == 0){
              servoLeft.writeMicroseconds(1500);
              servoRight.writeMicroseconds(1300);
              delay(300);
        }
      //stop
      else if (irLeft == 0 && irRight == 0){
          servoLeft.writeMicroseconds(1500);
          servoRight.writeMicroseconds(1500);
        }
    }
    if (back == 1 && obstacle == 1){
          if(path [i] == ' '){
          servoLeft.writeMicroseconds(1700);
          servoRight.writeMicroseconds(1300);
          }
          else if(path[i] == 'q'){
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1500);
            delay(300);
            }
            else if (path[i] == 'w'){
            servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1300);
            delay(300);
            }
            else if (path[i] == 'e'){
            servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1500);
            delay(300);
            servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1700);
            delay(300);
            }
            else if (path[i] == 'r'){
            servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1700);
            delay(300);
            servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1700);
            delay(300);
              }
          i += 1;
      }
}

void setupBlueToothConnection()
{
    Serial.println("Setting up the local (slave) Bluetooth module.");

    slaveNameCmd += shieldPairNumber;
    slaveNameCmd += "\r\n";

    blueToothSerial.print("\r\n+STWMOD=0\r\n");      // Set the Bluetooth to work in slave mode
    blueToothSerial.print(slaveNameCmd);             // Set the Bluetooth name using slaveNameCmd
    blueToothSerial.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
    blueToothSerial.print("\r\n+STOAUT=1\r\n");      // Permit paired device to connect me
    
    //  print() sets up a transmit/outgoing buffer for the string which is then transmitted via interrupts one character at a time.
    //  This allows the program to keep running, with the transmitting happening in the background.
    //  Serial.flush() does not empty this buffer, instead it pauses the program until all Serial.print()ing is done.
    //  This is useful if there is critical timing mixed in with Serial.print()s.
    //  To clear an "incoming" serial buffer, use while(Serial.available()){Serial.read();}

    blueToothSerial.flush();
    delay(2000);                                     // This delay is required

    blueToothSerial.print("\r\n+INQ=1\r\n");         // Make the slave Bluetooth inquirable
    
    blueToothSerial.flush();
    delay(2000);                                     // This delay is required
    
    Serial.println("The slave bluetooth is inquirable!");
}

int irDetect(int irLedPin, int irReceiverPin  , long frequency) {
  tone(irLedPin, frequency, 8);
  delay(1);
  int ir = digitalRead(irReceiverPin);
  delay(1);
  return ir;
}

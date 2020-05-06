//Alan Ramirez
//Commands in hexadecimal numbers
//Created 25/10/2015
//Last modified 03/11/2015

//**************************************************************
#include <Wire.h>
#include <SoftwareSerial.h>
/***************************************************************
// MD25
//**************************************************************/

#define SOFTWAREREG         0x0D                              // This register contains the revision number of the software in the modules PIC16F873 controller
#define SPEED1        (byte)0x00                              // Controls speed of right motor
#define SPEED2              0x01                              // Controls speed of left motor
#define ENCODER1            0x02                              // Reads motor encoder 1
#define ENCODER2            0x06                              // Reads motor encoder 2
#define CMD           (byte)0x10                              // Used to reset the encoder counts and module address changes
#define MD25ADDRESS         0x58                              // Address of the MD25
#define RESETENCODERS       0x20                              // Deletes values stored in the encoders

void setup(){
  Wire.begin();                                               // Tells the MD25 that we're talking to him
  delay(200);                                                 // Wait for everything to power up
  byte Softversion();                                         // Gets the software version. Function is defined below
  Resetencoders();                                            // Calls a function that resets the encoder values to 0, defined below
  pinMode(13, OUTPUT);                                        // Declares pin 13 as an output for the LED
}

byte Softversion(){                                           //This is the function that gets the software version
Wire.beginTransmission(MD25ADDRESS);                          //Send byte to read software version
Wire.write(SOFTWAREREG);
Wire.endTransmission();
Wire.requestFrom(MD25ADDRESS, 1);                             //Request 1 byte from MD25 to read the software version
while(Wire.available() < 1);                                  //Wait for it to arrive
byte software = Wire.read();                                  //Now read it
return(software);
}

void Resetencoders(){                                         // This is the function that resets the encoders values to 0
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(CMD);
  Wire.write(RESETENCODERS);                                // Putting the hex value to 0x20 to reset encoders
  Wire.endTransmission(); 
}

long encoder1(){                                            // Function to read and display value of encoder 1 as a long
  Wire.beginTransmission(MD25ADDRESS);                      // Send byte to get a reading from encoder 1 (right motor)
  Wire.write(ENCODER1);
  Wire.endTransmission();
  Wire.requestFrom(MD25ADDRESS, 4);                         // Request 4 bytes from MD25
  while(Wire.available() < 4);                              // While waitig for 4 bytes
  long pos1 = Wire.read();                                  // First byte for encoder 1, HH.
  pos1 <<= 8;
  pos1 += Wire.read();                                      // Second byte for encoder 1, HL
  pos1 <<= 8;
  pos1 += Wire.read();                                      // Third byte for encoder 1, LH
  pos1 <<= 8;
  pos1  +=Wire.read();                                      // Fourth byte for encoder 1, LL
  delay(2);                                                // Wait for everything to make sure everything is sent
  return(pos1);                                             // Returns pos1
}

long encoder2(){                                            // Same as for encoder 1 but now for encoder 2
  Wire.beginTransmission(MD25ADDRESS);           
  Wire.write(ENCODER2);
  Wire.endTransmission();
  Wire.requestFrom(MD25ADDRESS, 4);                         
  while(Wire.available() < 4);                              
  long pos2 = Wire.read();
  pos2 <<= 8;
  pos2 += Wire.read();                
  pos2 <<= 8;
  pos2 += Wire.read();                
  pos2 <<= 8;
  pos2  +=Wire.read();               
  delay(2);                                               
  return(pos2);                                             
}

void stopMotor(){                                           // Function to stop motors
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(SPEED1);                                       // we're going to define speed of R motor
  Wire.write(128);                                          // tells R motor to stop
  Wire.endTransmission();
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(SPEED2);                                       // we're going to define speed of L motor
  Wire.write(128);                                          // tells R motor to stop
  Wire.endTransmission(); 
  digitalWrite(13, HIGH);                                   // turn the LED on (HIGH is the voltage level)
  delay(50);                                                // wait for 50ms
  digitalWrite(13, LOW);                                    // turn the LED off
}  

void Wheelforward(int R,int L,int d){
    Resetencoders();
 do{                                                        // Start loop to drive motors forward
    Wire.beginTransmission(MD25ADDRESS);                    // we're talking to the md25
    Wire.write(SPEED1);                                     // "write" speed of R motor
    Wire.write(R + 128);                                    // (mode 0)
    Wire.endTransmission();
    Wire.beginTransmission(MD25ADDRESS);                    // Starts connection to MD25
    Wire.write(SPEED2);                                     // define the speed of the left motor
    Wire.write(L + 128);                                    // (mode 0)
    Wire.endTransmission();
    encoder1();                                             // Calls a function that reads and displays value of encoder 1 to LCD03
    encoder2();                                             // Calls a function that reads and displays value of encoder 2 to LCD03
   }while(encoder1() < d);                                  // Loop until encoder 2 is less than d (degrees turned by wheel)
   stopMotor();                                             // Stop motors
   delay(200);
}
 
 void Wheelbackward(int R,int L,int d){
  Resetencoders();
   do{                                                        // Start loop to drive motors backwards
    Wire.beginTransmission(MD25ADDRESS);                    // Starts connection to MD25 for R motor
    Wire.write(SPEED1);                                     // define the speed for R motor
    Wire.write(-R - 128);                                    // (mode 0)
    Wire.endTransmission();
    Wire.beginTransmission(MD25ADDRESS);                    // Starts connection to MD25 for L motor
    Wire.write(SPEED2);                                     // tell what speed motor L has to achieve
    Wire.write(-L - 128);                                    // (mode 0)
    Wire.endTransmission();
    encoder1();                                             // Calls a function that reads and displays value of encoder 1 to LCD03
    encoder2();                                             // Calls a function that reads and displays value of encoder 2 to LCD03
    
   }
   while(encoder1() > d);                                 
   
   stopMotor();                                             // stop motors
   delay(200);

}
/***************************************************************
// Here's where the circuit command is. Format: Wheelforward(Rweel rpm, Lweel rpm, Degrees turned);
Right is wheelforward, Wheelbackward is for going left
//**************************************************************/
void loop(){
  Wheelforward(60, 60, 389.1);                             //Forward 340mm
  Wheelforward(60, -60, 130);                             //Right 90 Degrees
  Wheelforward(60, 60, 322);                               //Forward 260mm
  Wheelbackward(60, -60, -130);                              //Left 90 Degrees
  Wheelforward(60, 60, 572.9);                             //Forward 500mm
  Wheelbackward(60, -60, -130);                              //Left 90 Degrees
  Wheelforward(74, 30, 610);                               //Arc 3-4
  Wheelforward(60, -60, 130);                             //Right 90 Degrees
  Wheelforward(60, 60, 756.3);                             //Forward 660mm
  Wheelbackward(60, -60, -130);                              //Left 90 Degrees
  Wheelforward(60, 60, 458.3);                             //Forward 400mm
  Wheelbackward(60, -60, -130);                              //Left 90 Degrees
  Wheelforward(60, 60, 458.3);                             //Forward 400mm
  Wheelbackward(60, -60, -130);                              //Left 90 Degrees
  Wheelforward(60, 60, 458.3);                             //Forward 400mm
  Wheelbackward(60, -60, -30);                              //Left 40 Degrees
  Wheelforward(60, 60, 713.1);                             //Forward 622.3mm
  Wheelbackward(60, -60, -243);                              //Left 140 Degrees
  Wheelforward(60, 60, 206.3);                             //Forward 180mm
  Wheelforward(60, -60, 130);                             //Right 90 Degrees
  Wheelforward(124.3, 40, 1800);                           //Arc 10-11
  Wheelbackward(60, -60, -200);                            //Left 142.59 Degrees
  Wheelforward(60, 60, 408.7);                             //Forward 356.7mm
  Wheelforward(60, 60, 490.4);                             //Forward 428mm
  exit(1);
}

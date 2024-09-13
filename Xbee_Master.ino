
/*
Simple example of how you communicate between 2 x Xbee radios.
Xbee's do not support full duplex comms so this sketch ping pongs data back and forth is a half duplex manner.

Write frequency is set in this Xbee_Master sketch

I tested this on a pair of Teensy 4.0's and Xbee Pro radios. 
You may need to add more write buffer memory if you plan to write long messages.

Xbee is set in transparent mode at 115200 baud rate.
*/



//variables we will be transferring. Set initial values
int int_1 = 1, int_2 = 2, int_3 = 3, int_4 = 4, int_5 = 5;
float float_1 = 1.0, float_2 = 2.0, float_3 = 3.0, float_4 = 4.0, float_5 = 5.0;

void setup() {

  Serial.begin(115200);  // not requred for Teensy
  Serial1.begin(115200); // for the xbee. Using hardware serial port 1 on the Teensy.
}

void loop() {

  Xbee_receive();  //keep checking the hardware serial port for incoming bytes.
}



//----------------this is where the message is received----------------------------------------------------------------------------------------------------------
void Xbee_receive() {

  static bool startofXbee, XBEEdataTerminator = false, terminator_set_flag = false;
  static int count = 0;
  static char receive_string[500];  // make the array big enough to hold the largest expected message

  unsigned long kickstart_Interval = 200;  // you have to kick start the comms. If comms fail this timer will re-start comms. Make this a good bit longer than the send interval.
  static unsigned long kickstart_Timer = millis();

  static unsigned long Xbee_Send_Interval2 = 100;  // this is the data send interval (in milliseconds)
  static unsigned long Xbee_Send_Timer2 = millis();


  // kickstart the serial comms if there hasnt been anything received for a long time
  if (terminator_set_flag) {
    kickstart_Timer = millis();
  }
  // kickstart the comms by sending the data
  if (millis() - kickstart_Timer > kickstart_Interval) {
    kickstart_Timer = millis();
    Xbee_send();
    Serial.println("kickstarter");
  }

  // Send to the serial port when send interval has expired
  if ((millis() - Xbee_Send_Timer2 > Xbee_Send_Interval2) && terminator_set_flag) {
    Xbee_Send_Timer2 = millis();
    terminator_set_flag = false;
    Xbee_send();
  }
  //-------------------------------------------------------------------------------------


  //-Check for the start < of the string -
  while (Serial1.available() && !startofXbee) {
    if (Serial1.read() == ('<'))  //Test to see if you are at the start of the payload
    {
      startofXbee = true;
      count = 0;
    }  //end if
  }    //end while

  //-Load data into the string array until end of string > is seen -
  while (Serial1.available() && startofXbee) {
    receive_string[count] = Serial1.read();

    if (receive_string[count] == '>')
      XBEEdataTerminator = true;

    count++;
  }  //end while

  //convert the string to variables
  if (XBEEdataTerminator) {
    sscanf(receive_string, "%u %u %u %u %u %f %f %f %f %f", &int_1, &int_2, &int_3, &int_4, &int_5, &float_1, &float_2, &float_3, &float_4, &float_5);

    startofXbee = false;
    XBEEdataTerminator = false;
    terminator_set_flag = true;


    //print the received variables
    Serial.print("  ");
    Serial.print(int_1);
    Serial.print("  ");
    Serial.print(int_2);
    Serial.print("  ");
    Serial.print(int_3);
    Serial.print("  ");
    Serial.print(int_4);
    Serial.print("  ");
    Serial.print(int_5);
    Serial.print("  ");
    Serial.print(float_1);
    Serial.print("  ");
    Serial.print(float_2);
    Serial.print("  ");
    Serial.print(float_3);
    Serial.print("  ");
    Serial.print(float_4);
    Serial.print("  ");
    Serial.println(float_5);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------



//----------------this is where the message is sent---------------------------------------------------------------------------------------------------------------
void Xbee_send() {

  char str2[500];

  //pack the variables into a string and send back to master
  snprintf(str2, sizeof(str2), "< %u %u %u %u %u %f %f %f %f %f >", int_1, int_2, int_3, int_4, int_5, float_1, float_2, float_3, float_4, float_5);
  for (uint16_t i = 0; i < strlen(str2); i++) {
    Serial1.write(str2[i]);
  }
}

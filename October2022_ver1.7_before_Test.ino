

                #include <EEPROM.h>
                #include "RTClib.h"
                #include <SPI.h>
                #include <Ethernet.h>
                #include <SD.h>
                #include "uptime.h"
                #include <Dns.h>
                #include <SafeString.h>

                RTC_DS3231 rtc;
                DNSClient dns;


                unsigned short int RTCMonth;
                unsigned short int CurrentMonth;
                bool FirstTime = 0;


                int val;
                unsigned char address = 0;   // address to store if the EEPROM was previously written

                unsigned char TOT_hr_address = 1;   // address to store the Total owner time
                unsigned char TOT_min_address = 2;

                unsigned char TRT_hr_address = 3;   // address to store the total renter time 
                unsigned char TRT_min_address = 4;

                unsigned char TMT_hr_address = 5;   // address to store the total Motor time
                unsigned char TMT_min_address = 6;

                unsigned char Ow_255_address = 7;
                unsigned char Re_255_address = 8;
                unsigned char TMT_255_address = 9;

                unsigned short TOT_EEPROM_h = 0;
                unsigned short TOT_EEPROM_m = 0;
                unsigned short TRT_EEPROM_h = 0;
                unsigned short TRT_EEPROM_m = 0;
                unsigned short TMT_EEPROM_h = 0;
                unsigned short TMT_EEPROM_m = 0;


                unsigned short Owner_255_indicator = 0;
                unsigned short Renter_255_indicator = 0;
                unsigned short TMT_255_indicator = 0;


                unsigned short DownloadRequest = 0  ;



                createSafeString(OneURL, 340, "");

                // createSafeString(OneURL, 1000, "");



                createSafeString(WriteSD, 180, "");


                createSafeString(hh, 10, "");
                createSafeString(mm, 10, "");

                createSafeString(TOT_h0, 10);
                createSafeString(TOT_m0, 10);
                createSafeString(TRT_h0, 10);
                createSafeString(TRT_m0, 10);
                createSafeString(TMT_h0, 10);
                createSafeString(TMT_m0, 10);


                // size of buffer used to capture HTTP requests
                // #define REQ_BUF_SZ   21

                // E2:0F:24:EF:FE:ED
                // MAC address from Ethernet shield sticker under board
                byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x89, 0x03 };
                

                // IPAddress ip(192, 168, 0, 10); // IP address, may need to change depending on network
                //IPAddress ip(10, 0, 0, 10);

                // use the numeric IP instead of the name for the server:
                // IPAddress IFTTT(34, 236, 248, 94);  // numeric IP for Google (no DNS)
                IPAddress IFTTT;
                // char IFTTT[] = "maker.ifttt.com";    // name address for Google (using DNS)
                IPAddress Google;


                EthernetServer server(80);  // create a server at port 80



                EthernetClient client2;
                EthernetClient client3;
                EthernetClient client4;
                EthernetClient client6;

                unsigned long StartTime, StartTimeOwner, StartTimeRenter, EndTime, EndTimeOwner, EndTimeRenter, timetaken, timetakenOwner, timetakenRenter;
                //  unsigned long TotalOwnerTime_Hours, TotalOwnerTime_Minutes, TotalRenterTime_Hours, TotalRenterTime_Minutes, TotalMotorTime_Hours, TotalMotorTime_Minutes;
                unsigned long  StartTimeLight, Light_Time;
                unsigned long TotalOwnerTime_Hours = 0;
                unsigned long TotalOwnerTime_Minutes = 0;
                unsigned long TotalRenterTime_Hours = 0;
                unsigned long TotalRenterTime_Minutes = 0;
                unsigned long TotalMotorTime_Hours = 0;
                unsigned long TotalMotorTime_Minutes = 0;

                unsigned long Res_StartTime_c2, Res_StartTime_c4, Res_StartTime_c3, Res_StartTime_c6;
                unsigned long Off_Motor_mail_startTime_Ow, Off_Motor_mail_startTime_Re;
                unsigned char Common_index = 0;
                unsigned char M_Off_common = 0;
                const unsigned short WaiteBeforeSendMail = 40000;
                bool c2 = 0;
                bool c4 = 0;
                bool c3 = 0;
                bool c6 = 0;

                bool trial;
                bool trialB;

                unsigned short R = 0;
                unsigned char Major_index = 0;

                bool setupFn = 0;
                unsigned long googleWaite = 0;
                const unsigned long GoogleInterval = 240000;
                bool Internet = 0;


                File webFile;               // the web page file on the SD card
                //char HTTP_req[REQ_BUF_SZ] = { 0 }; // buffered HTTP request stored as null terminated string
                //char req_index = 0;              // index into HTTP_req buffer
                //char res_index = 0;
                bool LED_state[2] = { 0 }; // stores the states of the LEDs
                unsigned short sensorReading = 0;
                bool LDR = 0;
                bool Interrupt = 0;
                bool Motor_state = 0;
                bool Renter_state = 0;
                bool Mega_State = 1;
                bool UNO_State = 0;   // state off for the arduino UNO

                // the following diffrentiators are not required internally as only i need to know who turned off the motor externally 
                // but in the webpage we are already differentiating who turned the motor on/off
                bool ext_Motor_interrupt_Owner = 0;    // this is used for differentiating between who turned off the motor
                bool ext_Motor_interrupt_Renter = 0;    // this is used for differentiating between who turned off the motor

                // Variable holding the timer value so far. One for each "Timer"
                unsigned long ContactorON;
                unsigned long ContactorOFF;
                // Time periods of blinks in milliseconds (1000 to a second).
                const unsigned long ContactorWaitIntervalON = 600000;          // 10 min to ensure it is really on
                const unsigned long ContactorWaitIntervalOFF = 300000;          // 5 min to ensure it is really off

                // contactor counter
                bool ONcounterContactor = 0;
                bool OFFcounterContactor = 0;


                // Motor Variable holding the timer value so far. One for each "Timer"
                unsigned long MotorContactorOn;
                unsigned long MotorContactorOff;
                // Time periods of blinks in milliseconds (1000 to a second).
                const unsigned long MotorContactorWaitInterval = 5000;          // 5 sec to ensure it is really on/off

                bool Signal = 0;
                unsigned char CommonSignal = 0;

                //  Mail requirements
                unsigned char Operator = 0;
                unsigned char LightOperator = 0;
                bool MotorMail = 0;
                bool LightMail = 0;
                bool lastState_Motor = 0;
                bool lastState_Light = 0;

                // in switch state 
                //  case 0   light off
                //  case 1   light ON 
                //  case 2   MotorOwnerON
                //  case 3   MotorOwnerOFF
                //  case 4   MotorRenterON 
                //  case 5   MotorRenterOFF

                // char buf1[] = "DDD, DD MMM YYYY hh:mm AP"; 
                // String Date_Time((char*)0);

                createSafeString(Date_Time, 60, "");

                File ExcelSD;

                char buf1[] = "DDD, DD MMM YYYY hh:mm AP";




                unsigned long h, m, s, ms;
                unsigned long TMT_m, TMT_h, TMT_over, TOT_m, TOT_h, TRT_m, TRT_h;
                unsigned long over;

                unsigned short day;
                unsigned short hour;
                unsigned short minute;

                createSafeString(days, 10);
                createSafeString(hours, 10);
                createSafeString(minutes, 10);

                bool MotorActualState;
                bool MotorInputReadingOwner;
                bool MotorInputReadingRenter;



                // FTP Variables 
                // change to your FTP server
                IPAddress FTPserver(192, 168, 1, 100);

                // comment out next line to write to SD from FTP server
                #define FTPWRITE

                EthernetClient FTP;
                EthernetClient dclient;

                char outBuf[128];
                unsigned char outCount;

                // change fileName to your file (8.3 format!)
                char fileName[13] = "file.csv";

                File fh;



                /* ============= Reset-Function ============== */
                void(*resetFunc) (void) = 0;   //declare reset function at address 0



                void setup()
                {    
                    // set up the LCD's number of columns and rows:
            //       lcd.begin(16, 2);
                    // Print a message to the LCD.
            //       lcd.print("Starting ....");
    
                    Serial.begin(9600);       // for debugging
                // disable Ethernet chip
                    Serial.println("Starting .....");

                    pinMode(10, OUTPUT);
                    digitalWrite(10, HIGH);
                    pinMode(53, OUTPUT);

                    Serial.println("calculateUptime .....");
                    uptime::calculateUptime();

        //           lcd.clear();
            //         lcd.print("Initialzing SD card ....");
        //            lcd.clear();

                    delay(2000);
                    SD.begin(4);  // initializing SD card for usage
                    delay(2000);

                    //  initialize SD card
                    Serial.println("Initializing SD card...");
                    Serial.println("Checking SD Card .....");
                    if (!SD.begin(4)) {
                            Serial.println("ERROR - SD card initialization failed!");
                        //      digitalWrite(x, LOW);
            //       lcd.clear();
            //       lcd.print("ERROR-SD failed!!");
            //       lcd.clear();
                        return;
                    }

                    
                        Serial.println("SUCCESS - SD card initialized.");
                    // check for index.htm file
                    //       if (!SD.exists("webpage.htm")) {
                        //  //  Serial.println("ERROR - Can't find index.htm file!");
                    //           return;  // can't find index file
                    //    }
                        //  //  Serial.println("SUCCESS - Found index.htm file.");

                    // Inputs from motor Owner or Renter state


                    pinMode(43, INPUT);          // INput for turn light OFF  ..connected to port 3
                    pinMode(42, INPUT);          // input for turn light ON ...  connected to port 2 

                    pinMode(22, OUTPUT);  // Owner to Operate the motor                         Relay
                    pinMode(23, OUTPUT);  // to turn off the motor ......                       Relay 
                    pinMode(24, OUTPUT);  // Renter to turn on the motor                        Relay 

                    pinMode(25, INPUT_PULLUP);  // for actual state of motor    motor is UP or down         220v Relay
                    pinMode(26, INPUT_PULLUP);  // for Owner as operator        if Owner key is closed      220v Relay 
                    pinMode(27, INPUT_PULLUP);  // for Renter as operator        if Renter key is closed    220v Relay   

                    pinMode(32, OUTPUT); //     for resetting the UNO


                    pinMode(2, OUTPUT);   // When light is off for whatever reason  pin 2 will be LOW 
                    pinMode(3, OUTPUT);   // when the light is ON for whatever reason pin 3 will be HIGH
                    

                    pinMode(A0, INPUT);
                    //    pinMode(A0, INPUT_PULLUP);


                    pinMode(5, OUTPUT);  // Normal LDR relay                                 control normal automatic operation  Relay 
                    pinMode(6, OUTPUT);  // to force the light off at night                                                      Relay 
                    pinMode(7, OUTPUT);  // used to force light on in the morning                                                Relay 


                    pinMode(30, OUTPUT); // LED Testing
                    pinMode(31, INPUT); // Arduino UNO Input  
                    digitalWrite(32, HIGH);  // for UNO Reset 

                    digitalWrite(2, HIGH);
                    digitalWrite(3, HIGH);

                    digitalWrite(5, HIGH);
                    digitalWrite(6, HIGH);
                    digitalWrite(7, HIGH);

                    digitalWrite(22, HIGH); // Owner to Operate the motor
                    digitalWrite(23, HIGH);  // to turn off the motor 
                    digitalWrite(24, HIGH);  // Renter to turn on the motor

                    // Ethernet.begin(mac, ip, subnet, gateway);  // initialize Ethernet device
                    //       delay(2000);

                     Serial.print("Please Waite 1 minute 20 sec ");
                    delay(60000);   // Putting delay so that the router can start and i can get IP address
                   Serial.print("Finished the Delay "); 

            //       unsigned char GetIP_Counter = 0;
        //         do {
                        // Serial.print("Trying to get IP address from DHCP ");
                          Ethernet.begin(mac);   // keep loop asking for DHCP
        //              delay(3000);
            //            GetIP_Counter++;
        //              // Serial.print("IP_Counter = ");
        //              // Serial.println(GetIP_Counter);
                    // Serial.print("Ethernet.begin(mac)=");
                    // Serial.println(Ethernet.begin(mac));
            //        } while (Ethernet.begin(mac) == 0 && GetIP_Counter < 1);

                    // print out the IP address
                        // Serial.print("IP = ");
                        // Serial.println(Ethernet.localIP());
                        // Serial.print("Gateway = ");
                    // Serial.println(Ethernet.gatewayIP());

                //      Serial.print("Get_IP Counter = ");
                //     Serial.println(GetIP_Counter);

                    if (Ethernet.begin(mac) == 0) {
                        // Serial.print("Assinging Static IP address");
                        IPAddress subnet(255, 255, 255, 0);
                        IPAddress gateway(192, 168, 1, 1);
                        IPAddress dns(192, 168, 1, 1);
                        IPAddress ip(192, 168, 1, 6);
                        Ethernet.begin(mac, ip, dns, gateway, subnet);

        //                Serial.print("Assinging Static IP address");
                       //     Serial.print("IP = ");
                       //     Serial.println(Ethernet.localIP());
                       //     Serial.print("Gateway = ");
                       //     Serial.println(Ethernet.gatewayIP());
                    }
                    // delay(3000);   // trying to put delay before enabling
                    //print out the IP address
                        //  Serial.print("IP = ");
                  //      Serial.println(Ethernet.localIP());



                    DNS_Function();
                    InternetFn();

                //        Serial.println("Sending Start Mail");
                    Operator = 6;
                    SendMail();

               //         Serial.println("Writing Start to SD-Card");

                    ExcelSD = SD.open("file.csv", FILE_WRITE);   // if the file opened okay, write to it:
                    if (ExcelSD) {
                        WriteSD = F("        , MegaStart, MegaStart, MegaStart, MegaStart, MegaStart, MegaStart, MegaStart, MegaStart");
                        ExcelSD.println(WriteSD);
                        // close the file:
                        ExcelSD.close();

                        WriteSD = F("");

                    }

                 //       Serial.println("Validating RTC Module");

                    if (!rtc.begin()) {
                        Serial.println("Couldn't find RTC");
                        Serial.flush();
                        abort();
                    }

           //         Serial.println("Successfull for RTC Module");

           //             Serial.println("Calling DateTimeFn...");
                    DateTimeFn();

                    //  //  Serial.println("Getting time");
                //  DateTime now = rtc.now();
                    //  //  Serial.println("We Got time");
                //  //  Serial.println("Clearing Strings..");
                    ClearStrings();

              //          Serial.println("SafeString enable debugging...");
                    SafeString::setOutput(Serial);

              //          Serial.println("Starting Server...");
                    server.begin();           // start to listen for clients

              //          Serial.println("Starting the loop Fn...");
                        delay(5000);   // trying to put delay before enabling

                }
                void loop()
                {
                    //     UNO_State = digitalRead(31);
                    MotorInputState();
                    LDR_Reading();
                    EthernetClient client = server.available();  // try to get client
                    if (client) {  // got client?
                        boolean currentLineIsBlank = true;

                        while (client.connected()) {
                            byte Request[21];
                            unsigned short Req_len = 21;
                            unsigned short len = 0;



                            if (client.available()) {


                                // client data available to read
                                char c = client.read(); // read 1 byte (character) from client
                        //        client.read(Request, R_len);
                                // limit the size of the stored received HTTP request
                                // buffer first part of HTTP request in HTTP_req array (string)
                                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                            //    client.read(Request, Req_len);

                                if (len < (Req_len - 1)) {
                                    //             HTTP_req[req_index] = c;          // save HTTP request character
                                    Request[R] = c;
                                    //              req_index++;
                                    R++;
                                }


                                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                                if (c == '\n' && currentLineIsBlank) {
                                    // send a standard http response header
                                    client.println("HTTP/1.1 200 OK");
                                    // remainder of header follows below, depending on if
                                    // web page or XML page is requested
                                    // Ajax request - send XML file

                        //            //  Serial.print(Request[5]);

                                    switch (Request[5])
                                    {
                                    case 97:
                                    {
                                        // send rest of HTTP header
                                        client.println("Content-Type: text/xml");
                                        client.println("Connection: close");
                                        client.println();
                                        if (Request[9] == 38) {
                                            switch (Request[10]) {
                                            case 102:
                                            {
                                                // Start file transfer 
                                    //          byte IPaddress_[4] = (client.remoteIP());
                                    //          FTP_server = FTP_server1.c_str()

                                    //            String IPIPIP = client.remoteIP();
                                    //            const char* apipch = IPIPIP;

                                                //  Serial.print(" Starting File Transfer");
                                                TransferFile();

                                            }
                                            break;
                                            case 76:                            // 76 stands for L --> LED=0/1
                                            {
                                                //  //  Serial.print("Matched LED Case");
                                                //  //  Serial.println();
                                                //  //  Serial.print("The value of LED is ");
                                                //  //  Serial.println(Request[15]);
                                                if (Request[15] == 49) {
                                                    //  //  Serial.print("ONE");
                                                    Major_index = 1;
                                                }
                                                else if (Request[15] == 48) {
                                                    //  //  Serial.print("ZERO");
                                                    Major_index = 2;
                                                }
                                            }
                                            break;
                                            case 79:                             // 79 stands for O --> Owner=0/1
                                            {
                                                //  //  Serial.print("Matched Owner Case");
                                                //  //  Serial.println();
                                                //  //  Serial.print("The value of Owner is ");
                                                //  //  Serial.println(Request[16]);
                                                if (Request[16] == 49) {
                                                    //  //  Serial.print("ONE");
                                                    Major_index = 3;
                                                }
                                                else if (Request[14] == 48) {
                                                    //  //  Serial.print("ZERO");
                                                    Major_index = 4;
                                                }
                                            }
                                            break;
                                            case 82:                             // 82 stands for R --> Renter=0/1
                                            {
                                                //  //  Serial.print("Matched Renter Case");
                                                //  //  Serial.println();
                                                //  //  Serial.print("The value of Renter is ");
                                                //  //  Serial.println(Request[17]);
                                                if (Request[17] == 49) {
                                                    //  //  Serial.print("ONE");
                                                    Major_index = 5;
                                                }
                                                else if (Request[17] == 48) {
                                                    //  //  Serial.print("ZERO");
                                                    Major_index = 6;
                                                }

                                            }
                                            break;
                                            //    case 85:                            // 85 stands for U --> Uno 
                                    //    {
                                    //
                                    //
                                    //    }
                                    //    break;
                                    //
                                    //    case 77:                           // 77 stands for M -> which is MegaReset fn
                                    //    {
                                    //
                                    //
                                    //    }
                                    //    break;

                                            }
                                        }
                                        SetLEDs();
                                        // send XML file containing input states
                                        XML_response(client);
                                    }
                                    break;
                                    case 117:                                            // if it is U then ascii is 85
                                    {
                                        webFile = SD.open("uno.jpg");
                                        cache_files(client);
                                        if (webFile) {
                                            while (webFile.available()) {
                                                client.write(webFile.read()); // send web page to client
                                            }
                                            webFile.close();
                                        }
                                    }
                                    break;
                                    case 109:
                                    {
                                        webFile = SD.open("motor.jpg");
                                        cache_files(client);
                                        if (webFile) {
                                            while (webFile.available()) {
                                                client.write(webFile.read()); // send web page to client
                                            }
                                            webFile.close();
                                        }
                                    }
                                    break;
                                    case 108:
                                    {
                                        webFile = SD.open("lamp.jpg");
                                        cache_files(client);
                                        if (webFile) {
                                            while (webFile.available()) {
                                                client.write(webFile.read()); // send web page to client
                                            }
                                            webFile.close();
                                        }
                                    }
                                    break;
                                    case 77:                     // 77 Stands for M
                                    {
                                        webFile = SD.open("mega.jpg");
                                        cache_files(client);
                                        if (webFile) {
                                            while (webFile.available()) {
                                                client.write(webFile.read()); // send web page to client
                                            }
                                            webFile.close();
                                        }

                                    }
                                    break;

                                    default:
                                    {
                                        // web page request
                                    // send rest of HTTP header
                                        client.println("Content-Type: text/html");
                                        client.println("Connection: close");
                                        client.println();
                                        // send web page
                                        webFile = SD.open("webpage4.htm");
                                        if (webFile) {
                                            while (webFile.available()) {
                                                client.write(webFile.read()); // send web page to client
                                            }
                                            webFile.close();
                                        }

                                    }

                                    }


                                    // display received HTTP request on serial port
                                            //  //  Serial.print(HTTP_req);
                                    // reset buffer index and all buffer elements to 0

                                        //  //  Serial.write(Request, R_len);


                                            //  //  Serial.println(HTTP_req);
                                            //  //  Serial.println();
                                    //  //  Serial.write(Request, R_len);
                    //             req_index = 0;
                                    R = 0;
                                    for (len = 0; len < Req_len; len++)
                                    {
                                        Request[len] = 0;
                                    }
                                    //              StrClear(HTTP_req, REQ_BUF_SZ);
                                    break;
                                }
                                // every line of text received from the client ends with \r\n
                                if (c == '\n') {
                                    // last character on line of received text
                                    // starting new line with next character read
                                    currentLineIsBlank = true;
                                }
                                else if (c != '\r') {
                                    // a text character was received from client
                                    currentLineIsBlank = false;
                                }
                            } // end if (client.available())
                        } // end while (client.connected())
                        client.flush();
                        delay(10);
                        client.stop(); // close the connection
                    } // end if (client)
                    // check the Input values
                    SendMailMotorOff();
                    ReadingResponse();
                    ReadingResponse_();
                    MotorActivateSignal();
                    BlynkLightControl();
                    LDR_State_Checker();
                    InternetFn();
                    Ethernet.maintain();
                }
                void LDR_Reading(void)
                {

                    sensorReading = analogRead(A0);
            //         Serial.print("Sensor Reading LDR = ");
            //       Serial.println(sensorReading);
            //         delay(1000);

                }

                void BlynkLightControl(void)
                {   bool BlynkLightON, BlynkLightOFF ;
                    BlynkLightON = digitalRead(42);
                    BlynkLightOFF = digitalRead(43);

                        if (BlynkLightON == 0 && BlynkLightOFF == 1 && LED_state[0] == 0 ) {
                                    StartTimeLight = millis();
                                    LED_state[0] = 1;
                                    LightMail = 1;        //  used to trigger mail about light status
                                    Interrupt = 1 ;
                                    digitalWrite(5, LOW);  // Makes the LED glow in Dark
                                    digitalWrite(3, LOW);
                                    digitalWrite(2, HIGH);

                                if (LightMail != lastState_Light)                // funciton to ensure mail is sent only one time and the start time as well
                                {
                                    //  //  Serial.println();
                        //  //  Serial.println(" triggered 1 Automatic Conrol  ");
                                    StartTimeLight = millis();
                                    LightOperator = 0;
                                    Light();
                                    lastState_Light = LightMail;
                                } 

                        } else if (BlynkLightON == 1 && BlynkLightOFF == 0 && LED_state[0] == 1 ) {
                                LightMail = 0;
                                LED_state[0] = 0;
                                Interrupt = 1 ;
                                digitalWrite(5, HIGH);   //Turns the LED OFF in Light.
                                digitalWrite(3, HIGH);
                                digitalWrite(2, LOW);

                                digitalWrite(3, HIGH);
                                digitalWrite(2, LOW);


                                if (LightMail != lastState_Light)
                                {
                                    //  //  Serial.println();
                                    //  //  Serial.println(" triggered 2 Automatic Conrol  ");
                                    LightOperator = 1;
                                    Light();
                                    lastState_Light = LightMail;
                                }

                        }

                }
                void LDR_State_Checker(void)
                {


                    if (Interrupt == 0) {
                        if (sensorReading <= 70) {
                                    StartTimeLight = millis();
                                    LED_state[0] = 1;
                                    LDR = 1;
                                    LightMail = 1;        //  used to trigger mail about light status
                                    digitalWrite(5, LOW);  // Makes the LED glow in Dark
                                    digitalWrite(3, LOW);
                                    digitalWrite(2, HIGH);
                                if (LightMail != lastState_Light)                // funciton to ensure mail is sent only one time and the start time as well
                                {
                                    //  //  Serial.println();
                        //  //  Serial.println(" triggered 1 Automatic Conrol  ");
                                    StartTimeLight = millis();
                                    LightOperator = 0;
                                    Light();
                                    lastState_Light = LightMail;
                                }                                                        
                        }
                        else if (80 < sensorReading  && sensorReading <= 160) {

                            if (ONcounterContactor == 0) {
                                ContactorON = millis();             // the start of 10 min since the threshold reached for the LDR 
                                ONcounterContactor = 1;
                                OFFcounterContactor = 0;
                            }
                         
                            if ((millis() - ContactorON) >= ContactorWaitIntervalON && (ONcounterContactor == 1)) {
                                LED_state[0] = 1;
                                LDR = 1;
                                LightMail = 1;        //  used to trigger mail about light status
                                digitalWrite(5, LOW);  // Makes the LED glow in Dark
                                digitalWrite(3, LOW);
                                digitalWrite(2, HIGH);   
                                if (LightMail != lastState_Light)                // funciton to ensure mail is sent only one time and the start time as well
                                {
                                    //  //  Serial.println();
                        //  //  Serial.println(" triggered 1 Automatic Conrol  ");
                                    StartTimeLight = millis();
                                    LightOperator = 0;
                                    Light();
                                    lastState_Light = LightMail;
                                }
                            }
                        }
                        else if (sensorReading >= 170) {
                            if (OFFcounterContactor == 0) {
                               ContactorOFF = millis();
                                ONcounterContactor = 0;
                                OFFcounterContactor = 1; 
                            }
                                digitalWrite(3, HIGH);
                                digitalWrite(2, LOW);
                            if ((millis() - ContactorOFF) >= ContactorWaitIntervalOFF && (OFFcounterContactor == 1)) {
                                LDR = 0;
                                LightMail = 0;
                                LED_state[0] = 0;

                                digitalWrite(5, HIGH);   //Turns the LED OFF in Light.

                                if (LightMail != lastState_Light)
                                {
                                    //  //  Serial.println();
                                    //  //  Serial.println(" triggered 2 Automatic Conrol  ");
                                    LightOperator = 1;
                                    Light();
                                    lastState_Light = LightMail;
                                }
                            }
                        }
                        
                    }
                }

                void MotorInputState(void)
                {
                    MotorActualState = digitalRead(25);
                    MotorInputReadingOwner = digitalRead(26);
                    MotorInputReadingRenter = digitalRead(27);
                    if (MotorInputReadingOwner == 0 && MotorActualState == 0)
                    {
                        MotorMail = 1;
                        LED_state[1] = 1;
                        Motor_state = 1;

                        if (MotorMail != lastState_Motor)
                        {
                            StartTimeOwner = millis();
                            ext_Motor_interrupt_Owner = 1;
                            Operator = 2;
                            SendMail();
                            lastState_Motor = MotorMail;

                        }
                    }
                    else if (MotorActualState == 1 && (ext_Motor_interrupt_Owner == 1 || Motor_state == 1))    // 1 in ext_motor_interrupt doesnot mean 5 volt from outside it is internal variable
                    {
                        ext_Motor_interrupt_Owner = 0;
                        MotorMail = 0;
                        LED_state[1] = 0;
                        Motor_state = 0;
                        if (MotorMail != lastState_Motor)
                        {
                            Operator = 3;
                            SendMail();
                            lastState_Motor = MotorMail;
                        }
                    }
                    else if (MotorInputReadingRenter == 0 && MotorActualState == 0)
                    {
                        ext_Motor_interrupt_Renter = 1;
                        MotorMail = 1;
                        Renter_state = 1;

                        if (MotorMail != lastState_Motor)
                        {
                            StartTimeRenter = millis();
                            Operator = 4;
                            SendMail();
                            lastState_Motor = MotorMail;

                        }
                    }
                    else if (MotorActualState == 1 && (ext_Motor_interrupt_Renter == 1 || Renter_state == 1))
                    {
                        ext_Motor_interrupt_Renter = 0;
                        MotorMail = 0;
                        Renter_state = 0;
                        if (MotorMail != lastState_Motor)
                        {
                            Operator = 5;
                            SendMail();
                            lastState_Motor = MotorMail;
                        }
                    }

                }
                // checks if received HTTP request is switching on/off LEDs
                // also saves the state of the LEDs
                void SetLEDs(void)
                {
                    //  //  Serial.println();
                    //  //  Serial.println("LDR:");
                //  //  Serial.println(LDR);  
                    // LED 1 (pin 6)
                    switch (Major_index) {
                    case 1:                     // the coming request is LED=1
                    {
                        if (LDR == 1) {
                            LED_state[0] = 1;  // save LED state
                            digitalWrite(6, HIGH); // will make LDR continue working at night
                            digitalWrite(3, LOW);
                            digitalWrite(2, HIGH);
                            Interrupt = 0;
                            LightMail = 1;

                            if (LightMail != lastState_Light)
                            {
                                //  //  Serial.println();
                                //  //  Serial.println(" triggered 1 case of LDR web control  ");
                                LightOperator = 0;
                                Light();
                                lastState_Light = LightMail;
                            }
                        }
                        else if (LDR == 0) {
                            LED_state[0] = 1;  // save LED state
                        // digitalWrite(8, LOW); // will force light to turn up at daylight by admin
                            digitalWrite(7, LOW); // will force light to turn up at daylight by admin
                            digitalWrite(3, LOW);
                            digitalWrite(2, HIGH);
                            Interrupt = 1;
                            LightMail = 1;

                            if (LightMail != lastState_Light)
                            {
                                //  //  Serial.println();
                                //  //  Serial.println(" triggered 2 case of LDR web control  ");
                                LightOperator = 0;
                                Light();
                                lastState_Light = LightMail;
                            }
                        }

                        Major_index = 0;
                    }
                    break;
                    case 2:                     // the coming request is LED=0
                    {
                        if (LDR == 1) {
                            LED_state[0] = 0;  // save LED state
                            digitalWrite(6, LOW); // will turn Off the light at night by admin
                            digitalWrite(3, HIGH);
                            digitalWrite(2, LOW);
                            Interrupt = 1;
                            LightMail = 0;
                            //  EndTime = millis();
                            if (LightMail != lastState_Light)
                            {
                                //  //  Serial.println();
                                //  //  Serial.println(" triggered 3 case of LDR web control  ");
                                LightOperator = 1;
                                Light();
                                lastState_Light = LightMail;
                            }
                        }
                        else if (LDR == 0) {
                            LED_state[0] = 0;  // save LED state
                    //     digitalWrite(8, HIGH);
                            digitalWrite(7, HIGH);
                            digitalWrite(3, HIGH);
                            digitalWrite(2, LOW);
                            Interrupt = 0;
                            LightMail = 0;
                            //    EndTime = millis();

                            if (LightMail != lastState_Light)
                            {
                                //  //  Serial.println();
                                //  //  Serial.println(" triggered 4 case of LDR web control  ");
                                LightOperator = 1;
                                Light();
                                lastState_Light = LightMail;
                            }
                        }
                        Major_index = 0;

                    }
                    break;
                    case 3:                        // coming request is Owner=1
                    {
                        // Motor Owner (pin 7)
                        if (Motor_state == 0) { // here to control web page to power on motor by contacting relay 3 sec then disconnect it
                            LED_state[1] = 1;  // save LED state for revealing this motor state on the webpage
                            Motor_state = 1;  // bec the next connection need the actual state of the motor in the program
                            MotorMail = 1;
                            ext_Motor_interrupt_Owner = 1;
                            StartTimeOwner = millis();
                            digitalWrite(22, LOW);

                            Signal = 1;                         // major Signal that action need still need to be taken 
                            CommonSignal = 1;                    // for differentiating  between the owner signals in switch case
                            MotorContactorOn = millis();        // for calculating the 5 sec without delaying the code

                    //    delay(3000);
                    //    digitalWrite(22, HIGH);



                            if (MotorMail != lastState_Motor)
                            {
                                Operator = 2;
                                SendMail();
                                lastState_Motor = MotorMail;
                            }

                        }
                        else if (Motor_state == 1) {
                            //      LED_state[1] = 1;  // save LED state
                            Motor_state = 1;
                        }
                        Major_index = 0;
                    }
                    break;
                    case 4:                       // coming request is Owner=0
                    {
                        if (Motor_state == 1) {
                            LED_state[1] = 0;  // save LED state for revealing this motor state on the webpage
                            Motor_state = 0; //  bec the next connection need the actual state of the motor in the program
                            MotorMail = 0;
                            ext_Motor_interrupt_Owner = 0;

                            digitalWrite(23, LOW);


                            Signal = 1;                         // major Signal that action need still need to be taken 
                            CommonSignal = 2;                    // for differentiating  between the owner signals in switch case
                            MotorContactorOff = millis();        // for calculating the 5 sec without delaying the code

                    //      delay(3000);
                    //      digitalWrite(23, HIGH);
                            if (MotorMail != lastState_Motor)
                            {
                                Operator = 3;
                                SendMail();
                                lastState_Motor = MotorMail;
                            }

                        }
                        else if (Motor_state == 0) {
                            //     LED_state[1] = 0;  // save LED state
                            Motor_state = 0;
                        }
                        Major_index = 0;
                    }
                    break;
                    case 5:                        // coming request is Renter=1
                    {
                        // Motor Renter (pin ----)
                        if (Renter_state == 0) { // here to control web page to power on motor by contacting relay 3 sec then disconnect it
                            Renter_state = 1;  // save LED state for revealing this motor state on the webpage
                            MotorMail = 1;
                            ext_Motor_interrupt_Renter = 1;
                            StartTimeRenter = millis();

                            digitalWrite(24, LOW);

                            Signal = 1;
                            CommonSignal = 3;
                            MotorContactorOn = millis();

                            //    delay(3000);
                            //    digitalWrite(24, HIGH);

                            if (MotorMail != lastState_Motor)
                            {
                                Operator = 4;
                                SendMail();
                                lastState_Motor = MotorMail;
                            }


                        }
                        else if (Renter_state == 1) {
                            Renter_state = 1;  // save LED state
                        }
                        Major_index = 0;
                    }
                    break;
                    case 6:                          // coming request is Renter=0
                    {
                        if (Renter_state == 1) {
                            Renter_state = 0;  // save LED state for revealing this motor state on the webpage
                            MotorMail = 0;
                            ext_Motor_interrupt_Renter = 0;

                            digitalWrite(23, LOW);

                            Signal = 1;
                            CommonSignal = 4;
                            MotorContactorOn = millis();


                            //     delay(3000);
                            //     digitalWrite(23, HIGH);


                            if (MotorMail != lastState_Motor)
                            {
                                Operator = 5;
                                SendMail();
                                lastState_Motor = MotorMail;
                            }

                        }
                        else if (Renter_state == 0) {
                            Renter_state = 0;  // save LED state
                        }
                        Major_index = 0;
                    }
                    break;
                    case 7:                           //  resetMega"
                    {
                        resetFunc(); // call reset 
                        Major_index = 0;
                    }
                    break;
                    case 8:                           //  restartUNO
                    {
                        digitalWrite(32, LOW);
                        delay(2000);
                        digitalWrite(32, HIGH);
                        Major_index = 0;
                    }
                    break;

                    }
                }

                // send the XML file with analog values, switch status
                //  and LED status
                void XML_response(EthernetClient cl)
                {
                    //  //  Serial.println("LED_state[1]");
                //  //  Serial.println(LED_state[1]);
                    cl.print("<?xml version=\"1.0\" ?>");
                    cl.print("<inputs>");
                    // checkbox LED states
                    // LED1
                    cl.print("<LED>");
                    if (LED_state[0] == 1) {
                        cl.print("checked");
                    }
                    else if (LED_state[0] == 0) {
                        cl.print("unchecked");
                    }
                    cl.println("</LED> ");
                    // button LED states
                    // LED3
                    cl.print("<Motor>");
                    if (LED_state[1] == 1) {
                        cl.print("on");
                    }
                    else if (LED_state[1] == 0) {
                        cl.print("off");
                    }
                    cl.println("</Motor>");
                    cl.print("<Renter>");
                    if (Renter_state == 1) {
                        cl.print("on");
                    }
                    else if (Renter_state == 0) {
                        cl.print("off");
                    }
                    cl.println("</Renter>");
                    cl.print("<UNO_State1>");
                    if (UNO_State == 1) {
                        cl.print("on");
                    }
                    else if (UNO_State == 0) {
                        cl.print("off");
                    }
                    cl.println("</UNO_State1>");

                    cl.print("<Mega_State1>");
                    if (Mega_State == 1) {
                        cl.print("on");
                    }
                    else if (Mega_State == 0) {
                        cl.print("off");
                    }
                    cl.println("</Mega_State1>");

                    cl.print("<Internet>");
                    if (Internet == 1) {
                        cl.print("on");
                    }
                    else if (Internet == 0) {
                        cl.print("off");
                    }
                    cl.println("</Internet>");

                    cl.print("<Download_Request>");
                    switch (DownloadRequest)
                    {
                    case 0:
                        cl.print("0");
                        break;
                    case 1:
                        cl.print("Success");
                        break;
                    case 2:
                        cl.print("Fail");
                        break;

                    }
                    cl.print("</Download_Request>");
                    cl.print("</inputs>");
                }
                void cache_files(EthernetClient cf)
                {
                    cf.println("Cache-Control: public,max-age=31536000,immutable");
                    cf.println("Content-Type: image/jpeg");
                    cf.println("Connection: close");
                    cf.println();
                }
                void SendMail()
                {
                    uptime::calculateUptime();

                    day = (uptime::getDays());
                    hour = (uptime::getHours());
                    minute = (uptime::getMinutes());

                    days = String(day).c_str();
                    hours = String(hour).c_str();
                    minutes = String(minute).c_str();

                    switch (Operator) {
                    case 2:
                    {
                        if (Internet == 1)
                        {
                            ClearStrings();
                            OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Owner%20:%20Motor%20ON%20&value2=%22%20Arduino_Up_Time%20:%20");
                            OneURL += days;
                            OneURL += F("%20days,%20");
                            OneURL += hours;
                            OneURL += F("%20hours,%20");
                            OneURL += minutes;
                            OneURL += F("%20minutes%20");
                            OneURL += F("%20%22%20 HTTP/1.1");


                            client3.stop();
                            if (client3.connect(IFTTT, 80)) {

                                client3.println(OneURL);
                                client3.println("Host: maker.ifttt.com");
                                client3.println("Connection: close");
                                client3.println();

                                Common_index = 2;
                                Res_StartTime_c3 = millis();

                                ClearStrings();
                            }


                            ClearStrings();
                            OneURL = F("GET /trigger/MotorTime/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Owner%20:%20Motor%20ON%20");
                            OneURL += F("%20%22%20 HTTP/1.1");

                            client6.stop();
                            if (client6.connect(IFTTT, 80)) {

                                client6.println(OneURL);
                                client6.println("Host: maker.ifttt.com");
                                client6.println("Connection: close");
                                client6.println();

                                Common_index = 2;
                                Res_StartTime_c6 = millis();

                                ClearStrings();
                            }
                        }

                        DateTimeFn();
                        ExcelSD = SD.open("file.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //  WriteSD = Date_Time + ", " + "Owner" + ", " + "Motor ON ";
                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Owner";
                            WriteSD += ", ";
                            WriteSD += "Motor ON ";

                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();

                            //      WriteSD.remove();
                            WriteSD = F("");
                            ClearStrings();
                        }


                        break;
                    }
                    case 3:
                    {

                        EndTimeOwner = millis();
                        timetakenOwner = EndTimeOwner - StartTimeOwner;


                        h = int(timetakenOwner / 3600000);
                        over = timetakenOwner % 3600000;
                        m = int(over / 60000);

                        TOT_h = h;
                        TOT_m = m;


                        val = EEPROM.read(address);
                        if (val == 0) {
                            val = 1;
                            EEPROM.update(address, val);
                            TotalOwnerTime_Hours = TOT_h;
                            TotalOwnerTime_Minutes = TOT_m;

                            TotalMotorTime_Hours = TOT_h;                // for calculating the total motor time by hours
                            TotalMotorTime_Minutes = TOT_m;          // for calculating the total motor time by mins

                            EEPROM.update(TOT_hr_address, TOT_h);
                            EEPROM.update(TOT_min_address, TOT_m);

                            EEPROM.update(TMT_hr_address, TOT_h);
                            EEPROM.update(TMT_min_address, TOT_m);

                        }
                        else if (val == 1) {


                            TOT_EEPROM_h = EEPROM.read(TOT_hr_address);
                            TOT_EEPROM_m = EEPROM.read(TOT_min_address);

                            Owner_255_indicator = EEPROM.read(Ow_255_address);
                            if (Owner_255_indicator > 0) {
                                Owner_255_indicator = (Owner_255_indicator * 255);
                                TotalOwnerTime_Hours = TOT_EEPROM_h + TOT_h + Owner_255_indicator;
                                TotalOwnerTime_Minutes = TOT_EEPROM_m + TOT_m;
                            }
                            else {
                                TotalOwnerTime_Hours = TOT_EEPROM_h + TOT_h;
                                TotalOwnerTime_Minutes = TOT_EEPROM_m + TOT_m;
                            }

                            while (TotalOwnerTime_Minutes >= 60)                                  // migirating min to hours 
                            {
                                TotalOwnerTime_Hours++;                                             // incrementing hrs vlaue by 1 
                                TotalOwnerTime_Minutes = TotalOwnerTime_Minutes - 60;
                            }


                            TMT_EEPROM_h = EEPROM.read(TMT_hr_address);                                // Reading values from EEPROM
                            TMT_EEPROM_m = EEPROM.read(TMT_min_address);                            // Reading values from EEPROM 

                            TMT_255_indicator = EEPROM.read(TMT_255_address);
                            if (TMT_255_indicator > 0)
                            {
                                TMT_255_indicator = (TMT_255_indicator * 255);
                                TotalMotorTime_Hours = TMT_EEPROM_h + TOT_h + TMT_255_indicator;
                                TotalMotorTime_Minutes = TMT_EEPROM_m + TOT_m;
                            }
                            else {

                                TotalMotorTime_Hours = TMT_EEPROM_h + TOT_h;                // for calculating the total motor time by hours
                                TotalMotorTime_Minutes = TMT_EEPROM_m + TOT_m;          // for calculating the total motor time by mins
                            }


                            while (TotalMotorTime_Minutes >= 60)                // migirating min to hours 
                            {
                                TotalMotorTime_Hours++;       // incrementing vlaue by 1 
                                TotalMotorTime_Minutes = TotalMotorTime_Minutes - 60;
                            }

                            while (TotalOwnerTime_Hours >= 255) {

                                TOT_EEPROM_h = TotalOwnerTime_Hours - 255;
                                Owner_255_indicator++;
                                // i need to store this values and checks against it 
                                EEPROM.update(Ow_255_address, Owner_255_indicator);

                            }

                            while (TotalMotorTime_Hours >= 255) {

                                TMT_EEPROM_h = TotalMotorTime_Hours - 255;
                                TMT_255_indicator++;
                                // i need to store this values and checks against it 
                                EEPROM.update(TMT_255_address, TMT_255_indicator);

                            }

                        }



                        hh = String(h).c_str();
                        mm = String(m).c_str();


                        TOT_h0 = String(TotalOwnerTime_Hours).c_str();
                        TOT_m0 = String(TotalOwnerTime_Minutes).c_str();

                        TMT_h0 = String(TotalMotorTime_Hours).c_str();
                        TMT_m0 = String(TotalMotorTime_Minutes).c_str();

                        M_Off_common = 1;
                        Off_Motor_mail_startTime_Ow = millis();




                        DateTimeFn();
                        ExcelSD = SD.open("file.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //        WriteSD = Date_Time + ", " + "Owner" + ", " + "Motor OFF" + ", " + hh + "," + mm + ", " + TOT_h0 + " " + TOT_m0 + ", " + TMT_h0 + "h" + " " + TMT_m0 + "m";

                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Owner";
                            WriteSD += ", ";
                            WriteSD += "Motor OFF";
                            WriteSD += ", ";
                            WriteSD += hh;
                            WriteSD += ",";
                            WriteSD += mm;
                            WriteSD += ", ";
                            WriteSD += TOT_h0;
                            WriteSD += " ";
                            WriteSD += TOT_m0;
                            WriteSD += ", ";
                            WriteSD += TMT_h0;
                            WriteSD += "h";
                            WriteSD += " ";
                            WriteSD += TMT_m0;
                            WriteSD += "m";

                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();

                            WriteSD = F("");
                            ClearStrings();
                        }

                        break;
                    }
                    case 4:
                    {
                        if (Internet == 1)
                        {
                            ClearStrings();
                            OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Renter%20:%20Motor%20ON%20&value2=%22%20Arduino_Up_Time%20:%20");
                            OneURL += days;
                            OneURL += F("%20days,%20");
                            OneURL += hours;
                            OneURL += F("%20hours,%20");
                            OneURL += minutes;
                            OneURL += F("%20minutes%20");
                            OneURL += TOT_h0;
                            OneURL += F("h");
                            OneURL += F("%20%22%20 HTTP/1.1");


                            client3.stop();
                            if (client3.connect(IFTTT, 80)) {
                                client3.println(OneURL);
                                client3.println("Host: maker.ifttt.com");
                                client3.println("Connection: close");
                                client3.println();

                                Common_index = 2;
                                Res_StartTime_c3 = millis();

                                ClearStrings();
                            }




                            ClearStrings();
                            OneURL += F("GET /trigger/MotorTime/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Renter%20:%20Motor%20ON%20");
                            OneURL += F("%20%22%20 HTTP/1.1");

                            if (client6.connect(IFTTT, 80)) {
                                client6.println(OneURL);
                                client6.println("Host: maker.ifttt.com");
                                client6.println("Connection: close");
                                client6.println();

                                Common_index = 2;
                                Res_StartTime_c6 = millis();

                                ClearStrings();
                            }
                        }

                        DateTimeFn();
                        ExcelSD = SD.open("file.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //  WriteSD = Date_Time + ", " + "Renter" + ", " + "Motor ON";
                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Renter";
                            WriteSD += ", ";
                            WriteSD += "Motor ON";


                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();

                            //            WriteSD.remove();
                            WriteSD = F("");
                            ClearStrings();

                        }


                        break;
                    }
                    case 5:
                    {


                        EndTimeRenter = millis();
                        timetakenRenter = EndTimeRenter - StartTimeRenter;

                        h = int(timetakenRenter / 3600000);
                        over = timetakenRenter % 3600000;
                        m = int(over / 60000);


                        TRT_h = h;
                        TRT_m = m;


                        val = EEPROM.read(address);
                        if (val == 0) {
                            val = 1;
                            EEPROM.update(address, val);
                            TotalRenterTime_Hours = TRT_h;
                            TotalRenterTime_Minutes = TRT_m;


                            TotalMotorTime_Hours = TRT_h;                // for calculating the total motor time by hours
                            TotalMotorTime_Minutes = TRT_m;          // for calculating the total motor time by mins

                            EEPROM.update(TRT_hr_address, TRT_h);
                            EEPROM.update(TRT_min_address, TRT_m);

                            EEPROM.update(TMT_hr_address, TRT_h);
                            EEPROM.update(TMT_min_address, TRT_m);
                        }
                        else if (val == 1) {



                            TRT_EEPROM_h = EEPROM.read(TRT_hr_address);
                            TRT_EEPROM_m = EEPROM.read(TRT_min_address);

                            Renter_255_indicator = EEPROM.read(Re_255_address);
                            if (Renter_255_indicator > 0)
                            {
                                Renter_255_indicator = (Renter_255_indicator * 255);
                                TotalRenterTime_Hours = TotalRenterTime_Hours + TRT_h + Renter_255_indicator;
                                TotalRenterTime_Minutes = TotalRenterTime_Minutes + TRT_m;

                            }
                            else {
                                TotalRenterTime_Hours = TotalRenterTime_Hours + TRT_h;
                                TotalRenterTime_Minutes = TotalRenterTime_Minutes + TRT_m;

                            }


                            while (TotalRenterTime_Minutes >= 60)                                  // migirating min to hours 
                            {
                                TotalRenterTime_Hours++;                                             // incrementing hrs vlaue by 1 
                                TotalRenterTime_Minutes = TotalRenterTime_Minutes - 60;
                            }


                            TMT_EEPROM_h = EEPROM.read(TMT_hr_address);                                // Reading values from EEPROM
                            TMT_EEPROM_m = EEPROM.read(TMT_min_address);                            // Reading values from EEPROM 

                            TMT_255_indicator = EEPROM.read(TMT_255_address);
                            if (TMT_255_indicator > 0)
                            {
                                TMT_255_indicator = (TMT_255_indicator * 255);
                                TotalMotorTime_Hours = TMT_EEPROM_h + TRT_h + TMT_255_indicator;
                                TotalMotorTime_Minutes = TMT_EEPROM_m + TRT_m;
                            }
                            else {

                                TotalMotorTime_Hours = TMT_EEPROM_h + TRT_h;                // for calculating the total motor time by hours
                                TotalMotorTime_Minutes = TMT_EEPROM_m + TRT_m;          // for calculating the total motor time by mins
                            }


                            while (TotalMotorTime_Minutes >= 60)                // migirating min to hours 
                            {
                                TotalMotorTime_Hours++;       // incrementing vlaue by 1 
                                TotalMotorTime_Minutes = TotalMotorTime_Minutes - 60;
                            }

                            while (TotalRenterTime_Hours >= 255) {

                                TRT_EEPROM_h = TotalRenterTime_Hours - 255;
                                Renter_255_indicator++;
                                // i need to store this values and checks against it 
                                EEPROM.update(Re_255_address, Renter_255_indicator);

                            }

                            while (TotalMotorTime_Hours >= 255) {

                                TMT_EEPROM_h = TotalMotorTime_Hours - 255;
                                TMT_255_indicator++;
                                // i need to store this values and checks against it 
                                EEPROM.update(TMT_255_address, TMT_255_indicator);

                            }

                        }





                        hh = String(h).c_str();
                        mm = String(m).c_str();

                        TRT_h0 = String(TotalRenterTime_Hours).c_str();
                        TRT_m0 = String(TotalRenterTime_Minutes).c_str();


                        TMT_h0 = String(TotalMotorTime_Hours).c_str();
                        TMT_m0 = String(TotalMotorTime_Minutes).c_str();

                        M_Off_common = 2;
                        Off_Motor_mail_startTime_Re = millis();


                        DateTimeFn();
                        ExcelSD = SD.open("file.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //     WriteSD = Date_Time + ", " + "Renter" + ", " + "Motor OFF" + ", " + hh + ", " + mm + ", " + TRT_h0 + " " + TRT_m0 + ", " + TMT_h0 + "h" + " " + TMT_m0 + "m";
                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Renter";
                            WriteSD += ", ";
                            WriteSD += "Motor OFF";
                            WriteSD += ", ";
                            WriteSD += hh;
                            WriteSD += ", ";
                            WriteSD += mm;
                            WriteSD += ", ";
                            WriteSD += TRT_h0;
                            WriteSD += " ";
                            WriteSD += TRT_m0;
                            WriteSD += ", ";
                            WriteSD += TMT_h0;
                            WriteSD += "h";
                            WriteSD += " ";
                            WriteSD += TMT_m0;
                            WriteSD += "m";


                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();

                            //          WriteSD.remove();
                            WriteSD = F("");
                            ClearStrings();
                        }

                        break;
                    }

                    case 6:
                    {
                        if (Internet == 1)
                        {
                            ClearStrings();
                            OneURL = F("GET /trigger/MotorTime/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Arduino_Mega%20:%20%22%20&value2=START");
                            OneURL += F("%20%22%20 HTTP/1.1");
                            if (client2.connect(IFTTT, 80)) {
                                client2.println(OneURL);
                                client2.println("Host: maker.ifttt.com");
                                client2.println("Connection: Close");
                                client2.println();
                                //         Common_index = 3;
                                //         Res_StartTime_c2 = millis();

                                ClearStrings();

                            }

                        }
                        break;
                    }
                    }
                }

                //  void SerialDebug1()
                // {
                //  //  Serial.println();
                //  //  Serial.print("connecting to ");
                //  //  Serial.print();
                //  //  Serial.println("...");
                //    }

                void Light()
                {
                    uptime::calculateUptime();

                    day = (uptime::getDays());
                    hour = (uptime::getHours());
                    minute = (uptime::getMinutes());

                    days = String(day).c_str();
                    hours = String(hour).c_str();
                    minutes = String(minute).c_str();


                    switch (LightOperator) {
                    case 0:
                    {
                        if (Internet == 1)
                        {
                            ClearStrings();
                            OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Light%20:%20ON%20&value2=%22%20Arduino_Up_Time%20:%20");
                            OneURL += days;
                            OneURL += F("%20days,%20");
                            OneURL += hours;
                            OneURL += F("%20hours,%20");
                            OneURL += minutes;
                            OneURL += F("%20minutes%20");
                            OneURL += F("%22%20 HTTP/1.1");

                            if (client2.connect(IFTTT, 80)) {

                                client2.println(OneURL);
                                client2.println("Host: maker.ifttt.com");
                                client2.println("Connection: close");
                                client2.println();

                                Common_index = 1;
                                Res_StartTime_c2 = millis();

                                ClearStrings();

                            }


                            ClearStrings();
                            OneURL = F("GET /trigger/light/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Light%20:%20ON%20&value3=%22%20Arduino_Up_Time%20:%20");
                            OneURL += days;
                            OneURL += F("%20days,%20");
                            OneURL += hours;
                            OneURL += F("%20hours,%20");
                            OneURL += minutes;
                            OneURL += F("%20minutes%20");
                            OneURL += F("%22%20 HTTP/1.1");

                            if (client4.connect(IFTTT, 80)) {

                                client4.println(OneURL);
                                client4.println("Host: maker.ifttt.com");
                                client4.println("Connection: close");
                                client4.println();

                                Common_index = 1;
                                Res_StartTime_c4 = millis();

                            }
                            ClearStrings();
                        }
                        DateTimeFn();
                        ExcelSD = SD.open("light.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //      WriteSD = Date_Time + ", " + "Light-ON";
                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Light-ON";

                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();

                            // WriteSD.remove();
                            WriteSD = F("");
                            ClearStrings();
                        }

                        break;
                    }

                    case 1:
                    {
                        Light_Time = millis() - StartTimeLight;

                        h = int(Light_Time / 3600000);
                        over = (Light_Time % 3600000);
                        m = int(over / 60000);

                        hh = String(h).c_str();
                        mm = String(m).c_str();



                        //  //  Serial.println();
                    //  //  Serial.println("Matched case 1 of light");
                    //  //  Serial.print("Light mail.... ");
                    //     SerialDebug1();
                        if (Internet == 1)
                        {
                            ClearStrings();
                            OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Light%20:%20OFF%20%22%20&value2=%22%20Time%20:%20");
                            OneURL += hh;
                            OneURL += F("h");
                            OneURL += F("%20");
                            OneURL += mm;
                            OneURL += F("m");
                            OneURL += F("%20%22%20 HTTP/1.1");
                            client2.stop();
                            if (client2.connect(IFTTT, 80)) {

                                client2.println(OneURL);
                                client2.println("Host: maker.ifttt.com");
                                client2.println("Connection: close");
                                client2.println();

                                Common_index = 1;
                                Res_StartTime_c2 = millis();

                                ClearStrings();
                            }



                            OneURL = F("GET /trigger/light/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Light%20:%20OFF%20%22%20&value2=%22%20Time%20:%20");
                            OneURL += hh;
                            OneURL += F("h");
                            OneURL += F("%20");
                            OneURL += mm;
                            OneURL += F("m");
                            OneURL += F("%20%22%20 HTTP/1.1");

                            if (client4.connect(IFTTT, 80)) {

                                client4.println(OneURL);
                                client4.println("Host: maker.ifttt.com");
                                client4.println("Connection: close");
                                client4.println();

                                Common_index = 1;
                                Res_StartTime_c4 = millis();

                                ClearStrings();

                            }
                        }


                        DateTimeFn();
                        ExcelSD = SD.open("light.csv", FILE_WRITE);   // if the file opened okay, write to it:
                        if (ExcelSD) {
                            //           WriteSD = Date_Time + ", " + "Light-OFF" + ", " + hh + ", " + mm;
                    //     WriteSD = Date_Time + ", " + "Light-OFF";
                            WriteSD = Date_Time;
                            WriteSD += ", ";
                            WriteSD += "Light-OFF";

                            ExcelSD.println(WriteSD);
                            // close the file:
                            ExcelSD.close();


                            WriteSD = F("");
                            ClearStrings();

                        }

                        break;
                    }
                    }
                }
                void DateTimeFn()
                {
                    DateTime now = rtc.now();
                    Date_Time = now.toString(buf1);



                    RTCMonth = (now.DateTime::month());

                    if (FirstTime == 0) {
                        CurrentMonth = RTCMonth;
                        FirstTime = 1;
                    }

                    //  //  Serial.print("the RTC month is ");
                    //  //  Serial.println(RTCMonth);

                    //  //  Serial.print("the Current month is ");
                    //  //  Serial.println(CurrentMonth);

                    //  //  Serial.println(buf1);

                    if (CurrentMonth != RTCMonth) {

                        TotalRenterTime_Hours = 0;
                        TotalRenterTime_Minutes = 0;

                        TotalOwnerTime_Hours = 0;
                        TotalOwnerTime_Minutes = 0;

                        TotalMotorTime_Hours = 0;
                        TotalMotorTime_Minutes = 0;

                        val = 0;
                        EEPROM.update(address, val);
                        EEPROM.update(TOT_hr_address, val);
                        EEPROM.update(TOT_min_address, val);
                        EEPROM.update(TRT_hr_address, val);
                        EEPROM.update(TRT_min_address, val);
                        EEPROM.update(TMT_hr_address, val);
                        EEPROM.update(TMT_min_address, val);
                        EEPROM.update(Ow_255_address, val);
                        EEPROM.update(Re_255_address, val);
                        EEPROM.update(TMT_255_address, val);

                        //  //  Serial.println("Updating all the EEPROM Variables with 0");

                        CurrentMonth = RTCMonth;


                    }
                    //  //  Serial.println("DateTimeFn Done");
                    //  //  Serial.println(Date_Time);
                }
                void MotorActivateSignal() {
                    if (CommonSignal != 0) {
                        switch (CommonSignal) {

                        case 1:          // Owner On
                        {
                            if (Signal == 1) {

                                if ((millis() - MotorContactorOn) >= MotorContactorWaitInterval)
                                {
                                    digitalWrite(22, HIGH);
                                    Signal = 0;
                                    CommonSignal = 0;
                                    MotorContactorOn = 0;
                                }

                            }
                        }
                        break;
                        case 2:        // owner Off
                        {
                            if (Signal == 1) {

                                if ((millis() - MotorContactorOff) >= MotorContactorWaitInterval)
                                {
                                    digitalWrite(23, HIGH);
                                    Signal = 0;
                                    CommonSignal = 0;
                                    MotorContactorOff = 0;
                                }

                            }
                        }
                        break;
                        case 3:         // Renter On
                        {
                            if (Signal == 1) {
                                if ((millis() - MotorContactorOn) >= MotorContactorWaitInterval)
                                {
                                    digitalWrite(24, HIGH);
                                    Signal = 0;
                                    CommonSignal = 0;
                                    MotorContactorOn = 0;
                                }
                            }
                        }
                        break;
                        case 4:          // Renter Off
                        {
                            if (Signal == 1) {

                                if ((millis() - MotorContactorOff) >= MotorContactorWaitInterval)
                                {
                                    digitalWrite(23, HIGH);
                                    Signal = 0;
                                    CommonSignal = 0;
                                    MotorContactorOff = 0;
                                }

                            }
                        }
                        break;


                        }
                    }
                }
                void ClearStrings()
                {
                    OneURL = F("");
                    Date_Time = F("");
                    //   day = F("");
                    //   hour = F("");
                    //   minute = F("");


                }
                void ReadingResponse() {
                    if (Internet == 1 && Common_index == 1) {
                        // switch (Common_index) {
                        //case 1:
                        {
                            bool result;
                            if (millis() - Res_StartTime_c2 > 2000 && c2 == 0) {

                                if (client2.available())
                                {
                                    //    unsigned short res_len;
                                    unsigned short len = 605;
                                    //     res_len = client2.available();
                                    byte buffer[605];

                                    //  //  Serial.println();
                                    //  //  Serial.println(" the First response length is : ");
                                    //  //  Serial.println(res_len);
                                    //  //  Serial.println();

                                    client2.read(buffer, len);


                                    int i = 0;
                                    result = 0;
                                    int k;
                                    for (i = 0; i < 605; i++)
                                    {  //  67 111 110 103 114 97
                                        k = i;
                                        if (buffer[k] == 67 && buffer[k + 1] == 111 && buffer[k + 2] == 110 && buffer[k + 3] == 103 && buffer[k + 4] == 114 && buffer[k + 5] == 97 && buffer[k + 6] == 116 && buffer[k + 7] == 117 && buffer[k + 8] == 108 && buffer[k + 9] == 97 && buffer[k + 10] == 116 && buffer[k + 11] == 105 && buffer[k + 12] == 111 && buffer[k + 13] == 110 && buffer[k + 14] == 115 && buffer[k + 15] == 33) {
                                            result = 1;
                                            //  //  Serial.print("found the word !!!!!!!!!!!!!!!");
                                        }
                                        buffer[i] = 0;
                                    }
                                    if (result == 1) {
                                        c2 = 1;
                                        client2.stop();
                                    }

                                }
                            }
                            else if (!client2.connected() && (millis() - Res_StartTime_c2 >= 6000) && c2 == 0) {
                                //  //  Serial.println();
                                //  //  Serial.println("disconnecting.");
                                client2.stop();
                            }
                            else if (!client2.connected()) { client2.stop(); }



                            if (millis() - Res_StartTime_c4 > 4500 && c4 == 0) {

                                if (client4.available())
                                {
                                    //        unsigned short res_len;
                                    unsigned short len = 605;
                                    //        res_len = client4.available();
                                    byte buffer[605];

                                    //  //  Serial.println();
                                    //  //  Serial.println(" the Second response length is : ");
                                    //  //  Serial.print(res_len);
                                    //  //  Serial.println();


                                    client4.read(buffer, len);
                                    unsigned short i = 0;
                                    result = 0;
                                    unsigned short k;
                                    for (i = 0; i < 605; i++)
                                    {  //  67 111 110 103 114 97
                                        k = i;
                                        if (buffer[k] == 67 && buffer[k + 1] == 111 && buffer[k + 2] == 110 && buffer[k + 3] == 103 && buffer[k + 4] == 114 && buffer[k + 5] == 97 && buffer[k + 6] == 116 && buffer[k + 7] == 117 && buffer[k + 8] == 108 && buffer[k + 9] == 97 && buffer[k + 10] == 116 && buffer[k + 11] == 105 && buffer[k + 12] == 111 && buffer[k + 13] == 110 && buffer[k + 14] == 115 && buffer[k + 15] == 33) {
                                            result = 1;
                                            //  //  Serial.print("found the word !!!!!!!!!!!!!!!");
                                        }
                                        buffer[i] = 0;
                                    }
                                    if (result == 1) {
                                        c4 = 1;
                                        client4.stop();
                                    }

                                }
                            }
                            else if (!client4.connected() && (millis() - Res_StartTime_c4 >= 6500) && c4 == 0) {
                                //  //  Serial.println();
                                //  //  Serial.println("disconnecting.");
                                client4.stop();
                                Common_index = 0;
                            }
                            else if (!client4.connected()) { client4.stop(); }

                            //  //  Serial.println("The C2/C4 value before && condidition");
                            //  //  Serial.println("C2= ");
                            //  //  Serial.println(c2);
                            //  //  Serial.println("C4= ");
                            //  //  Serial.println(c4);
                            if (c2 == 1 && c4 == 1) {
                                Common_index = 0;
                                c2 = 0;
                                c4 = 0;
                                trial = 0;
                                //  //  Serial.println();
                                //  //  Serial.println("C2= ");
                                //  //  Serial.println(c2);
                                //  //  Serial.println("C4= ");
                                //  //  Serial.println(c4);
                                //  //  Serial.println("Common_index= ");
                                //  //  Serial.println(Common_index);


                            }
                            else if (millis() - Res_StartTime_c4 >= 7500 && trial == 0) {
                                Light();
                                trial = 1;
                                //  //  Serial.print("Send Light Mail again");

                                //  //  Serial.println();
                                //  //  Serial.println("C2= ");
                                //  //  Serial.print(c2);
                                //  //  Serial.println("C4= ");
                                //  //  Serial.print(c4);
                            }


                        }
                        //     break;
                        // }
                    }
                }
                void ReadingResponse_() {
                    if (Internet == 1 && Common_index == 2) {
                        //  case 2:
                        // {
                        bool result1;
                        if (millis() - Res_StartTime_c3 > 500 && c3 == 0) {

                            if (client3.available())
                            {
                                //       unsigned short res_len11;
                                unsigned short len1 = 605;
                                //       res_len11 = client3.available();
                                byte buffer1[605];

                                //  //  Serial.println();
                                //  //  Serial.println(" the First response len1gth is : ");
                                //  //  Serial.println(res_len11);
                                //  //  Serial.println();

                                client3.read(buffer1, len1);


                                unsigned short i = 0;
                                result1 = 0;
                                unsigned short k_;
                                for (i = 0; i < 605; i++)
                                {  //  67 111 110 103 114 97
                                    k_ = i;
                                    if (buffer1[k_] == 67 && buffer1[k_ + 1] == 111 && buffer1[k_ + 2] == 110 && buffer1[k_ + 3] == 103 && buffer1[k_ + 4] == 114 && buffer1[k_ + 5] == 97 && buffer1[k_ + 6] == 116 && buffer1[k_ + 7] == 117 && buffer1[k_ + 8] == 108 && buffer1[k_ + 9] == 97 && buffer1[k_ + 10] == 116 && buffer1[k_ + 11] == 105 && buffer1[k_ + 12] == 111 && buffer1[k_ + 13] == 110 && buffer1[k_ + 14] == 115 && buffer1[k_ + 15] == 33) {
                                        result1 = 1;
                                        //  //  Serial.print("found the word !!!!!!!!!!!!!!!");
                                    }
                                    buffer1[i] = 0;
                                }
                                if (result1 == 1) {
                                    c3 = 1;
                                    client3.stop();
                                }
                            }
                        }
                        else if (!client3.connected() && (millis() - Res_StartTime_c3 >= 6000) && c3 == 0) {
                            //  //  Serial.println();
                            //  //  Serial.println("disconnecting.");
                            client3.stop();
                        }
                        else if (!client3.connected()) { client3.stop(); }

                        if (millis() - Res_StartTime_c6 > 700 && c6 == 0) {

                            if (client6.available())
                            {
                                //           unsigned short res_len11;
                                unsigned short len1 = 605;
                                //           res_len11 = client6.available();
                                byte buffer1[605];

                                //  //  Serial.println();
                                //  //  Serial.println(" the Second response len1gth is : ");
                                //  //  Serial.print(res_len11);
                                //  //  Serial.println();


                                client6.read(buffer1, len1);
                                unsigned short i = 0;
                                result1 = 0;
                                unsigned short k_;
                                for (i = 0; i < 605; i++)
                                {  //  67 111 110 103 114 97
                                    k_ = i;
                                    if (buffer1[k_] == 67 && buffer1[k_ + 1] == 111 && buffer1[k_ + 2] == 110 && buffer1[k_ + 3] == 103 && buffer1[k_ + 4] == 114 && buffer1[k_ + 5] == 97 && buffer1[k_ + 6] == 116 && buffer1[k_ + 7] == 117 && buffer1[k_ + 8] == 108 && buffer1[k_ + 9] == 97 && buffer1[k_ + 10] == 116 && buffer1[k_ + 11] == 105 && buffer1[k_ + 12] == 111 && buffer1[k_ + 13] == 110 && buffer1[k_ + 14] == 115 && buffer1[k_ + 15] == 33) {
                                        result1 = 1;
                                        //  //  Serial.print("found the word !!!!!!!!!!!!!!!");
                                    }
                                    buffer1[i] = 0;
                                }
                                if (result1 == 1) {
                                    c6 = 1;
                                }

                            }
                        }
                        else if (!client6.connected() && (millis() - Res_StartTime_c6 >= 6500) && c6 == 0) {
                            //  //  Serial.println();
                            //  //  Serial.println("disconnecting.");
                            client6.stop();
                            Common_index = 0;
                        }
                        else if (!client6.connected()) { client6.stop(); }

                        //  //  Serial.println("The c3/c6 value before && condidition");
                        //  //  Serial.println("c3= ");
                        //  //  Serial.println(c3);
                        //  //  Serial.println("c6= ");
                        //  //  Serial.println(c6);
                        if (c3 == 1 && c6 == 1) {
                            Common_index = 0;
                            c3 = 0;
                            c6 = 0;
                            trialB = 0;
                            //  //  Serial.println();
                            //  //  Serial.println("c3= ");
                            //  //  Serial.println(c3);
                            //  //  Serial.println("c6= ");
                            //  //  Serial.println(c6);
                            //  //  Serial.println("Common_index= ");
                            //  //  Serial.println(Common_index);


                        }
                        else if (millis() - Res_StartTime_c6 >= 7500 && trialB == 0) {
                            SendMail();
                            trialB = 1;

                            //  //  Serial.print("Send Motor Mail again");
                            //  //  Serial.println();
                            //  //  Serial.println("c3= ");
                            //  //  Serial.print(c3);
                            //  //  Serial.println("c6= ");
                            //  //  Serial.print(c6);
                        }
                        else if (millis() - Res_StartTime_c6 >= 8000 && trialB == 1) {
                            Common_index = 0;
                            c3 = 0;
                            c6 = 0;
                            trialB = 0;
                        }


                        // }
                        // break;

                    }
                }
                void SendMailMotorOff()
                {
                    if (Internet == 1 && M_Off_common != 0) {

                        switch (M_Off_common) {
                        case 1:
                        {
                            if ((millis() - Off_Motor_mail_startTime_Ow) >= WaiteBeforeSendMail) {
                                ClearStrings();
                                OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Owner%20:%20MoTor%20OFF%20%22%20||%20&value2=%22%20Owner-Motor-Time%20:%20");
                                OneURL += hh;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += mm;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("Total%20Owner%20hours%20:");
                                OneURL += TOT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TOT_m0;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20||%20Total%20Motor%20hours%20:");
                                OneURL += TMT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TMT_m0;
                                OneURL += F("m");
                                OneURL += F("%20||%20");
                                OneURL += F("%20Arduino_Up_Time%20:%20");
                                OneURL += days;
                                OneURL += F("%20days,%20");
                                OneURL += hours;
                                OneURL += F("%20hours,%20");
                                OneURL += minutes;
                                OneURL += F("%20minutes%20");
                                OneURL += F("%20%22%20 HTTP/1.1");


                                client3.stop();
                                if (client3.connect(IFTTT, 80)) {
                                    //  //  Serial.print("connected to ");
                                    //  //  Serial.println(client3.remoteIP());
                                    // Make a HTTP request:
                                    client3.println(OneURL);
                                    client3.println("Host: maker.ifttt.com");
                                    client3.println("Connection: close");
                                    client3.println();

                                    Common_index = 2;
                                    Res_StartTime_c3 = millis();

                                    ClearStrings();
                                }


                                ClearStrings();
                                OneURL = F("GET /trigger/MotorTime/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Owner_MoTor%20:%20OFF%20%22%20&value2=");
                                OneURL += hh;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += mm;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20&value3=");
                                OneURL += F("Total%20Owner%20hours%20:");
                                OneURL += TOT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TOT_m0;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20||%20Total%20Motor%20hours%20:");
                                OneURL += TMT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TMT_m0;
                                OneURL += F("m");
                                OneURL += F("%20||%20");
                                OneURL += F("%20Arduino_Up_Time%20:%20");
                                OneURL += days;
                                OneURL += F("%20days,%20");
                                OneURL += hours;
                                OneURL += F("%20hours,%20");
                                OneURL += minutes;
                                OneURL += F("%20minutes%20");
                                OneURL += F("%20%22%20 HTTP/1.1");

                                client6.stop();
                                if (client6.connect(IFTTT, 80)) {

                                    client6.println(OneURL);
                                    client6.println("Host: maker.ifttt.com");
                                    client6.println("Connection: close");
                                    client6.println();

                                    Common_index = 2;
                                    Res_StartTime_c6 = millis();

                                    ClearStrings();
                                }

                                M_Off_common = 0;

                            }
                        }
                        break;

                        case 2:
                        {
                            if ((millis() - Off_Motor_mail_startTime_Re) >= WaiteBeforeSendMail) {

                                ClearStrings();
                                OneURL = F("GET /trigger/Farm/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Renter%20:%20MoTor%20OFF%20%22%20||%20&value2=%22%20Renter-Motor-Time%20:%20");
                                OneURL += hh;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += mm;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("Total%20Renter%20hours%20:");
                                OneURL += TRT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TRT_m0;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20||%20Total%20Motor%20hours%20:");
                                OneURL += TMT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TMT_m0;
                                OneURL += F("m");
                                OneURL += F("%20||%20");
                                OneURL += F("%20Arduino_Up_Time%20:%20");
                                OneURL += days;
                                OneURL += F("%20days,%20");
                                OneURL += hours;
                                OneURL += F("%20hours,%20");
                                OneURL += minutes;
                                OneURL += F("%20minutes%20");
                                OneURL += F("%20%22%20 HTTP/1.1");


                                client3.stop();
                                if (client3.connect(IFTTT, 80)) {
                                    client3.println(OneURL);
                                    client3.println("Host: maker.ifttt.com");
                                    client3.println("Connection: close");
                                    client3.println();

                                    Common_index = 2;
                                    Res_StartTime_c3 = millis();

                                    ClearStrings();
                                }


                                ClearStrings();
                                OneURL = F("GET /trigger/MotorTime/with/key/kqotITniP1OKyJb3SHv44Ebj6XWr3_lILvz75wjMBIg?value1=%22%20Renter%20:%20MoTor%20OFF%20%22%20&value2=");
                                OneURL += hh;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += mm;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20&value3=");
                                OneURL += F("Total%20Renter%20hours%20:");
                                OneURL += TRT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TRT_m0;
                                OneURL += F("m");
                                OneURL += F("%20");
                                OneURL += F("%20||%20Total%20Motor%20hours%20:");
                                OneURL += TMT_h0;
                                OneURL += F("h");
                                OneURL += F("%20");
                                OneURL += TMT_m0;
                                OneURL += F("m");
                                OneURL += F("%20||%20");
                                OneURL += F("%20Arduino_Up_Time%20:%20");
                                OneURL += days;
                                OneURL += F("%20days,%20");
                                OneURL += hours;
                                OneURL += F("%20hours,%20");
                                OneURL += minutes;
                                OneURL += F("%20minutes%20");
                                OneURL += F("%20%22%20 HTTP/1.1");

                                if (client6.connect(IFTTT, 80)) {

                                    client6.println(OneURL);
                                    client6.println("Host: maker.ifttt.com");
                                    client6.println("Connection: close");
                                    client6.println();

                                    Common_index = 2;
                                    Res_StartTime_c6 = millis();

                                    ClearStrings();
                                }
                                M_Off_common = 0;
                            }
                        }
                        break;
                        }

                    }
                }
                void InternetFn() {

                    unsigned char L = 15;

                    if ((millis() - googleWaite) >= GoogleInterval || setupFn == 0) {
                        googleWaite = millis();
                        if (client2.connect(Google, 80)) {
                            //    googleWaite = millis();
                                //  //  Serial.print("connected to ");
                                //  //  Serial.println(client2.remoteIP());
                                //  //  Serial.println();
                                // Make a HTTP request:

                            client2.println("GET / HTTP/1.1");                                            //search?q=arduino
                            client2.println("Host: www.google.com");
                            client2.println("Connection: close");
                            client2.println();
                            //  //  Serial.println("Request Sent ... ");
                        }
                        delay(2500);
                        if ((millis() - googleWaite >= 1500) && client2.available())
                        {
                            //  //  Serial.println("Conditions matched 1 sec and client availability .. matching on response....");
                            byte googleReply[15];
                            client2.read(googleReply, L);

                            if (googleReply[9] == 50 && googleReply[10] == 48 && googleReply[11] == 48 && googleReply[12] == 32 && googleReply[13] == 79 && googleReply[14] == 75)
                            {
                                Internet = 1;
                    //           //  Serial.println("Internet is wokring...");

                    //            //  Serial.print("Internet value = ");
                    //            //  Serial.println(Internet);
                                DNS_Function();
                            }
                            else {
                                Internet = 0;
                    //          //  Serial.println("No Internet !!!");
                    //          //  Serial.print("Internet value = ");
                    //          //  Serial.println(Internet);
                            }
                        }
                        else {
                            Internet = 0;
                    //     //  Serial.println(" the server is not available ");
                    //     //  Serial.println("No Internet !!!");

                        }

                        if (!client2.connected()) {
                            client2.stop();
                        }
                        DownloadRequest = 0;
                        setupFn = 1;

                    }
                }

                void DNS_Function()
                {
                    //  //  Serial.println("Getting DNS IP for maker.ifttt.com");
                    dns.begin(Ethernet.dnsServerIP());
                    dns.getHostByName("maker.ifttt.com", IFTTT);
                    //  //  Serial.print("maker.ifttt.com =");
                    //  //  Serial.println(IFTTT);

                    dns.getHostByName("www.google.com", Google);
                    //  //  Serial.print("www.google.com = ");
                    //  //  Serial.println(Google);

                }
                void TransferFile() {
                    if (doFTP()) {
                //        //  Serial.println(F("FTP OK"));
                        DownloadRequest = 1;
                    }
                    else {
            //           //  Serial.println(F("FTP FAIL"));
                        DownloadRequest = 2;
                    }
                }
                byte doFTP()
                {
                #ifdef FTPWRITE
                    fh = SD.open(fileName, FILE_READ);
                #else
                    SD.remove(fileName);
                    fh = SD.open(fileName, FILE_WRITE);
                #endif

                    if (!fh)
                    {
            //          //  Serial.println(F("SD open fail"));
                        return 0;
                    }

                #ifndef FTPWRITE  
                    if (!fh.seek(0))
                    {
            //          //  Serial.println(F("Rewind fail"));
                        fh.close();
                        return 0;
                    }
                #endif

        //         //  Serial.println(F("SD opened"));

                    if (FTP.connect(FTPserver, 21)) {
            //           //  Serial.println(F("Command connected"));
                    }
                    else {
                        fh.close();
            //            //  Serial.println(F("Command connection failed"));
                        return 0;
                    }

                    if (!eRcv()) return 0;

                    FTP.println(F("USER gomaa"));

                    if (!eRcv()) return 0;

                    FTP.println(F("PASS 123456"));

                    if (!eRcv()) return 0;

                    FTP.println(F("SYST"));

                    if (!eRcv()) return 0;

                    FTP.println(F("Type I"));

                    if (!eRcv()) return 0;

                    FTP.println(F("PASV"));

                    if (!eRcv()) return 0;

                    char* tStr = strtok(outBuf, "(,");
                    int array_pasv[6];
                    for (int i = 0; i < 6; i++) {
                        tStr = strtok(NULL, "(,");
                        array_pasv[i] = atoi(tStr);
                        if (tStr == NULL)
                        {
            //                //  Serial.println(F("Bad PASV Answer"));

                        }
                    }

                    unsigned int hiPort, loPort;

                    hiPort = array_pasv[4] << 8;
                    loPort = array_pasv[5] & 255;

        //           //  Serial.print(F("Data port: "));
                    hiPort = hiPort | loPort;
        //         //  Serial.println(hiPort);

                    if (dclient.connect(FTPserver, hiPort)) {
        //            //  Serial.println(F("Data connected"));
                    }
                    else {
            //            //  Serial.println(F("Data connection failed"));
                        FTP.stop();
                        fh.close();
                        return 0;
                    }

                #ifdef FTPWRITE
                    FTP.print(F("STOR "));
                    FTP.println(fileName);
                #else
                    FTP.print(F("RETR "));
                    FTP.println(fileName);
                #endif

                    if (!eRcv())
                    {
                        dclient.stop();
                        return 0;
                    }

                #ifdef FTPWRITE
            //        //  Serial.println(F("Writing"));

                    byte clientBuf[64];
                    int clientCount = 0;

                    while (fh.available())
                    {
                        clientBuf[clientCount] = fh.read();
                        clientCount++;

                        if (clientCount > 63)
                        {
                            dclient.write(clientBuf, 64);
                            clientCount = 0;
                        }
                    }

                    if (clientCount > 0) dclient.write(clientBuf, clientCount);

                #else
                    while (dclient.connected())
                    {
                        while (dclient.available())
                        {
                            char c = dclient.read();
                            fh.write(c);
            //              //  Serial.write(c);
                        }
                    }
                #endif

                    dclient.stop();
            //        //  Serial.println(F("Data disconnected"));

                    if (!eRcv()) return 0;

                    FTP.println(F("QUIT"));

                    if (!eRcv()) return 0;

                    FTP.stop();
        //         //  Serial.println(F("Command disconnected"));


                    fh.close();
        //         //  Serial.println(F("SD closed"));
                    return 1;
                }
                byte eRcv()
                {
                    byte respCode;
                    byte thisByte;

                    while (!FTP.available()) delay(1);

                    respCode = FTP.peek();

                    outCount = 0;

                    while (FTP.available())
                    {
                        thisByte = FTP.read();
        //             //  Serial.write(thisByte);

                        if (outCount < 127)
                        {
                            outBuf[outCount] = thisByte;
                            outCount++;
                            outBuf[outCount] = 0;
                        }
                    }

                    if (respCode >= '4')
                    {
                        efail();
                        return 0;
                    }

                    return 1;
                }
                void efail()
                {
        //         byte thisByte = 0;

                    FTP.println(F("QUIT"));

                    while (!FTP.available()) delay(1);

        //          while (FTP.available())
        //         {
        //                 thisByte = FTP.read();
        //              //  Serial.write(thisByte);
        //         }

                    FTP.stop();
            //        //  Serial.println(F("Command disconnected"));
                    fh.close();
        //          //  Serial.println(F("SD closed"));
                }

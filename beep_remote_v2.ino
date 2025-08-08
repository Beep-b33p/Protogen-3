#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_DotStar.h>

//buttons and pin number
 const int happyFace = 13;
 const int sadFace = 12;
 const int angryFace = 11;
 const int shockedFace = 10;
 const int loading = 9;
 uint16_t currentState;

BLEService        resv = BLEService("0d420329-43b4-464a-aceb-c5dd186cd9fc"); //reSv = remoteService
BLECharacteristic cust = BLECharacteristic("0cf7d534-be5f-402f-b520-2ad170a99b4a"); // cuSt = currentState
//BLECharacteristic baSt = BLECharacteristic("0cf7d534-be5f-402f-b520-2ad170a99b4a"); // baSt = batteryState
 // ^ could use the BLE Battery Service instead of creating a custom one? 
 //https://www.bluetooth.com/specifications/bas-1-1/
 BLEDis bledis;    // DIS (Device Information Service) helper class instance
Adafruit_DotStar strip(1, 8, 6, DOTSTAR_BRG);
void setup()
{
  Serial.begin(9600);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Beep Remote! - Remote Service :3");
  Serial.println("-----------------------\n");

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.begin();

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

     //Configure and Start the Device Information Service
  Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("ItsyBitsynRF52480");
  bledis.begin(); 

  Serial.println("Configuring the Button Remote Service");
  setupREM();
  
  // Setup the advertising packet(s)
  Serial.println("Setting up the advertising payload(s)");
  startAdv();

  pinMode(happyFace, INPUT);
  pinMode(sadFace, INPUT);
  pinMode(angryFace, INPUT);
  pinMode(shockedFace, INPUT);
  pinMode(loading, INPUT);

  strip.begin(); // Initialize pins for output
  strip.setBrightness(10);
  strip.show();  // Turn all LEDs off ASAP
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include buttonService UUID
  Bluefruit.Advertising.addService(resv);

  // Include Name
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(120);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */

void setupREM(void)
{
  
  resv.begin();
  cust.setProperties(CHR_PROPS_NOTIFY);
  //cust.setProperties(CHR_PROPS_READ);
  cust.setPermission(SECMODE_OPEN,  SECMODE_OPEN);
  cust.setFixedLen(6);
  cust.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  cust.begin();
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value) //notify
{
    // Display the raw request packet
    Serial.print("CCCD Updated: ");
    //Serial.printBuffer(request->data, request->len);
    Serial.print(cccd_value);
    Serial.println("");

    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    if (chr->uuid == resv.uuid) {
        if (chr->notifyEnabled(conn_hdl)) {
            Serial.println("Button Service 'Notify' enabled");
        } else {
            Serial.println("Button Service 'Notify' disabled");
        }
    }
}

void loop() 
{
  /*int reading1 = digitalRead(happyFace); //happy expression
  int reading2 = digitalRead(sadFace); //sad expression
  int reading3 = digitalRead(angryFace); //angry expression
  int reading4 = digitalRead(shockedFace); //suprised expression
  int reading5 = digitalRead(loading); //loading expression

  if (Bluefruit.connected()) 
  {
    strip.setPixelColor(0, 120 , 0, 0);
    strip.show();*/

      if (digitalRead(happyFace) == HIGH) 
      {
        currentState = 1;
        Serial.println("The thumb button is on");
        Serial.print("Current State of the Face is ");
        cust.write16(currentState);
        cust.notify16(currentState);
        Serial.println(currentState);
        strip.setPixelColor(0, 0, 0, 255);
        strip.show();
        delay(100);
      }
      else if (digitalRead(sadFace) == HIGH) 
      {
      currentState = 2;
        Serial.println("The index button is on");
        Serial.print("Current State of the Face is ");
        cust.write16(currentState);
        cust.notify16(currentState);
        Serial.println(currentState);
        strip.setPixelColor(0, 0, 0, 255);
        strip.show();
        delay(100);
      }
      else if (digitalRead(angryFace) == HIGH) 
      {
      currentState = 3;
        Serial.println("The middle button is on");
        Serial.print("Current State of the Face is ");
        cust.write16(currentState);
        cust.notify16(currentState);
        Serial.println(currentState);
        strip.setPixelColor(0, 0, 0, 255);
        strip.show();
        delay(100);
      }

      else if (digitalRead(shockedFace) == HIGH) 
      {
      currentState = 4;
        Serial.println("The ring button is on");
        Serial.print("Current State of the Face is ");
        cust.write16(currentState);
        cust.notify16(currentState);
        Serial.println(currentState);
        strip.setPixelColor(0, 0, 0, 255);
        strip.show();
        delay(100);
      }
      else if (digitalRead(loading) == HIGH) 
      {
      currentState = 5;
        Serial.println("The pinky button is on");
        Serial.print("Current State of the Face is ");
        cust.write16(currentState);
        cust.notify16(currentState);
        Serial.println(currentState);
        strip.setPixelColor(0, 0, 0, 255);
        strip.show();
        delay(100);
      }
    else
    {
    Serial.println("The button is off");
    currentState = 0;
    cust.notify16(currentState);
    delay(100);
    }
    delay(100);
  }
//}
   /*else if(digitalRead(happyFace) == LOW)
  {
    currentState = 2;
    Serial.println("The button is on");
    Serial.print("Current State of the Face is ");
    cust.write16(currentState);
    cust.notify16(currentState);
    Serial.println(currentState);
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
    delay(250);
  }*/
  /*else if(sensorVal3 = 0)
  {
    currentState = 3;
    delay(3000);
  }
    else if(sensorVal4 = 0)
  {
    currentState = 4;
    delay(3000);
  }
    else if(sensorVal5 = 0)
  {
    currentState = 5;
    delay(3000);
  } 
  else
  {
   digitalWrite(LED,LOW);
   Serial.println("The button is off");
   currentState = 0;
   cust.notify16(currentState);
   delay(250);
  }
  

  // Only send updates every three seconds
  delay(1000);
  }*/
//}

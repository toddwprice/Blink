 /*
  Web client
 
 This sketch connects to the Staywell Customs internal site
 using an Arduino Wiznet Ethernet shield, gets data on workload 
 percentage, and prints it to a tft.
 
 This sketch is based upon the web client example sketch included 
 in the arduino sdk, created 18 Dec 2009 by David A. Mellis.
 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 */

//define the pins for the tft, since we're not using the spi pins
//as those would conflict with the ethernet shield

#define sclk 9
#define mosi 8
#define cs   7
#define dc   6
#define rst  5

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h> //spi library for ethernet
#include <Ethernet.h>
#include <aJSON.h>

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0D, 0x19, 0x58 };
IPAddress server(10,249,201,20); //ev-web-01

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
String json = "";
char ajson[600] = "";

int print = 0; //variable to tell whether or not to store the web data

int screen = 1;

int month = 1;
int year = 2012;
int prevMonth = 1;
int prevYear = 2012;

int loadPage = 0;

int report = 0;




void setup() {
  
  tft.initR(INITR_REDTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE); 
  tft.setTextSize(1);
  tft.setCursor(0, 10);
  
  tft.println(" Up/Down selects");
  tft.println(" the month");
  tft.println(" Left/Right selects");
  tft.println(" the year");
  tft.println(" Current month ");
  tft.print(" selection is ");
  tft.println(month);
  tft.println(" Current year");
  tft.print(" selection is ");
  tft.println(year);

  Serial.begin(9600);
  Serial.println("Starting!");
  

  
  

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(300);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 8181)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /api/blink/2 HTTP/1.0");
    client.println();
    //Serial.println("Waiting for network connection.");
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }


}



void loop()
{
    //prepare the LCD to be printed to

  
  while( loadPage == 0 ) {

    uint8_t b = readButton();
    if (b == BUTTON_DOWN) {
      month++;

    }
    if (b == BUTTON_LEFT) {
      year--;

    }
    if (b == BUTTON_UP) {
      month--;

    }
    if (b == BUTTON_RIGHT) {
      year++;

    }
    if (b == BUTTON_SELECT) {
      loadPage = 1;
      Serial.println("going to load webpage now...");
    }  

    if (prevMonth != month || prevYear != year) {
      tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(0, 10);
      tft.println(" Up/Down selects");
      tft.println(" the month");
      tft.println(" Left/Right selects");
      tft.println(" the year");
      tft.println(" Current month ");
      tft.print(" selection is ");
      tft.println(month);
      tft.println(" Current year");
      tft.print(" selection is ");
      tft.println(year);

    }

    prevMonth = month;
    prevYear = year;
    
    if ( loadPage == 1 ) {
      tft.print(" aaaa");
    }
  }
   

    //Serial.print("Got out of loop!");
    // if there are incoming bytes available 
    // from the server, read them and print them:
    if (client.available()) {
      char c = client.read();
      //Serial.print(c);
      if ( c == '{' ) { //wait until the json starts with a bracket
        print = 1;
      }
      if (print == 1) { //save only the json
        json += c;
      }
      
      //delay(3);
      //tft.print(c);

    }


    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      //Serial.println("Data loaded from webpage.");
      
      Serial.println(json); //this will not appear on tft, it's only for debugging
      Serial.println();

      //turn the string into a char array so it can be parsed
      json.toCharArray(ajson, 700);

      //find the data within the json, and create a variable with that data
      aJsonObject* jsonObject = aJson.parse(ajson);
      aJsonObject* ed = aJson.getObjectItem(jsonObject, "AmountApproved");

      //again, debugging purposes
      Serial.println("Amount Approved:");
      Serial.println(ed->valuefloat);
      Serial.println("disconnecting.");  

      client.stop();


      //loop through the screens forever

      while(true) {

        if(screen == 1) { 
          tft.fillScreen(ST7735_BLACK);
          tft.setCursor(0, 0);
          tft.setTextSize(2);
          //print the data from the json
          //currently only prints placeholder values 
          tft.println();
          tft.println(" Workload");
          tft.println(" ========");

          tft.print(" ed: 100%");
          tft.setTextSize(1);
          tft.println();
          tft.println();

          tft.setTextSize(2);
          tft.print(" de: 100%");
          tft.setTextSize(1);
          tft.println();
          tft.println();

          tft.setTextSize(2);
          tft.println(" am: 100%");
          tft.print(" ");
          tft.println(ed->valuefloat);
         /* tft.setTextSize(1);
          tft.println();
          tft.println();

          tft.println(" Current month ");
          tft.print(" selection is ");
          tft.println(month);
          tft.println(" Current year");
          tft.print(" selection is ");
          tft.println(year);
        */
          screen = 2;
          delay(3000);
        }
        if(screen == 2) {
          tft.fillScreen(ST7735_BLACK);
          tft.setTextSize(2);
          tft.setCursor(0, 0);

          tft.println();
          tft.println(" Placeholder Page 2");
         /* tft.setTextSize(1);
          tft.println();
          tft.println();

          tft.println(" Current month ");
          tft.print(" selection is ");
          tft.println(month);
          tft.println(" Current year");
          tft.print(" selection is ");
          tft.println(year);
        */
          screen = 3;
          delay(3000);
        }  
        if(screen == 3) {
          tft.fillScreen(ST7735_BLACK);
          tft.setTextSize(2);
          tft.setCursor(0, 0);
          tft.println();
          tft.println(" Placeholder Page 3");
          tft.println(" This is cooler than Placeholder Page 2");
          tft.setTextSize(1);
         /* tft.setTextSize(1);
          tft.println();
          tft.println();

          tft.println(" Current month ");
          tft.print(" selection is ");
          tft.println(month);
          tft.println(" Current year");
          tft.print(" selection is ");
          tft.println(year);
        */
          screen = 1;
          delay(3000);
        }
      }
      for(;;)
      ;

    }
  }
  
  uint8_t readButton(void) {
  float a = analogRead(3);

  a *= 5.0;
  a /= 1024.0;

  //Serial.print("Button read analog = ");
  //Serial.println(a);
  if (a < 0.2) return BUTTON_DOWN;
  if (a < 1.0) return BUTTON_RIGHT;
  if (a < 1.5) return BUTTON_SELECT;
  if (a < 2.0) return BUTTON_UP;
  if (a < 3.2) return BUTTON_LEFT;
  else return BUTTON_NONE;
}


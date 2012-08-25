/*
  Web client
 
 this sketch requires an arduino mega, other boards don't have enough ram
 
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
String json;
char ajson[600];

int print = 0; //variable to tell whether or not to store the web data

int screen = 1;

int k;

char* months[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
  "Aug", "Sep", "Oct", "Nov", "Dec"};


void setup() {

  tft.initR(INITR_REDTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE); 
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  attachInterrupt(0, buttonPress, CHANGE);

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

    //Serial.println(json);
    //this will not appear on tft,
    //it's only for debugging
    //Serial.println();

    //turn the string into a char array so it can be parsed
    json.toCharArray(ajson, 500);

    //find the data within the json, and create a variable with that data
    aJsonObject* jsonObject = aJson.parse(ajson);

    aJsonObject* month = aJson.getObjectItem(jsonObject, "CurrentMonth");
    aJsonObject* ed1 = aJson.getObjectItem(jsonObject, "EdWorkload1");
    aJsonObject* de1 = aJson.getObjectItem(jsonObject, "DeWorkload1");
    aJsonObject* am1 = aJson.getObjectItem(jsonObject, "AMWorkload1"); 
    aJsonObject* irfcount1 = aJson.getObjectItem(jsonObject, "IrfCount1");
    aJsonObject* irfcost1 = aJson.getObjectItem(jsonObject, "IrfCost1");
    aJsonObject* pages1 = aJson.getObjectItem(jsonObject, "Pages1");
    aJsonObject* products1 = aJson.getObjectItem(jsonObject, "Products1");

    aJsonObject* ed2 = aJson.getObjectItem(jsonObject, "EdWorkload2");
    aJsonObject* de2 = aJson.getObjectItem(jsonObject, "DeWorkload2");
    aJsonObject* am2 = aJson.getObjectItem(jsonObject, "AMWorkload2");
    aJsonObject* irfcount2 = aJson.getObjectItem(jsonObject, "IrfCount2");
    aJsonObject* irfcost2 = aJson.getObjectItem(jsonObject, "IrfCost2");
    aJsonObject* pages2 = aJson.getObjectItem(jsonObject, "Pages2");
    aJsonObject* products2 = aJson.getObjectItem(jsonObject, "Products2");

    aJsonObject* ed3 = aJson.getObjectItem(jsonObject, "EdWorkload3");
    aJsonObject* de3 = aJson.getObjectItem(jsonObject, "DeWorkload3");
    aJsonObject* am3 = aJson.getObjectItem(jsonObject, "AMWorkload3");
    aJsonObject* irfcount3 = aJson.getObjectItem(jsonObject, "IrfCount3");
    aJsonObject* irfcost3 = aJson.getObjectItem(jsonObject, "IrfCost3");
    aJsonObject* pages3 = aJson.getObjectItem(jsonObject, "Pages3");
    aJsonObject* products3 = aJson.getObjectItem(jsonObject, "Products3");


    int curMonth = month->valueint - 1;
    Serial.println(curMonth);
    int lastMonth = curMonth - 1;
    int lst2Month = lastMonth - 1;

    if(lastMonth < 0) {
      lastMonth += 12;
    }
    if(lst2Month < 0) {
      lst2Month += 12;     
    }  

    Serial.println(lastMonth);
    Serial.println(lst2Month);

    //again, debugging purposes
    //Serial.println(ed1->valuefloat);
    Serial.println("disconnecting.");  

    client.stop();


    //loop through the screens forever

    while(true) {

      if(screen == 1) { 
        
        tft.setTextSize(1);
        //print the data from the json
        //currently only prints placeholder values 
        tft.println();
        tft.println(" Workload");
        tft.println(" =================");

        tft.print("     ");
        tft.print(months[curMonth]);
        tft.print("  ");
        tft.print(months[lastMonth]);
        tft.print("  ");
        tft.println(months[lst2Month]);

        tft.print(" ed: ");
        tft.print(ed1->valuefloat, 0);
        tft.print("% ");
        tft.print(ed2->valuefloat, 0);
        tft.print("% ");
        tft.print(ed3->valuefloat, 0);
        tft.println("%");

        tft.print(" de: ");
        tft.print(de1->valuefloat, 0);
        tft.print("%  ");
        tft.print(de2->valuefloat, 0);
        tft.print("%  ");
        tft.print(de3->valuefloat, 0);
        tft.println("%");

        tft.print(" am: ");
        tft.print(am1->valuefloat, 0);
        tft.print("%  ");
        tft.print(am2->valuefloat, 0);
        tft.print("%  ");
        tft.print(am3->valuefloat, 0);
        tft.println("%");

        tft.println();
        tft.println();
        tft.println(" Pages");
        tft.println(" =================");

        tft.print(" ");
        tft.print(months[curMonth]);
        tft.print(": ");
        tft.println(pages1->valueint);
        tft.print(" ");
        tft.print(months[lastMonth]);
        tft.print(": ");
        tft.println(pages2->valueint);
        tft.print(" ");
        tft.print(months[lst2Month]);
        tft.print(": ");
        tft.println(pages3->valueint);
        
        screen = 3;

      }
      if(screen == 2) {

        tft.println();
        tft.println(" IRF Count");
        tft.println(" =================");
        tft.print(" ");
        tft.print(months[curMonth]);
        tft.print("  ");
        tft.print(months[lastMonth]);
        tft.print("  ");
        tft.println(months[lst2Month]);

        tft.print(" ");
        tft.print(irfcount1->valueint);
        tft.print("    ");
        tft.print(irfcount2->valueint);
        tft.print("     ");
        tft.println(irfcount3->valueint);

        tft.println();
        tft.println();
        tft.println(" IRF Cost");
        tft.println(" =================");

        tft.print(" ");
        tft.print(months[curMonth]);
        tft.print(": ");
        tft.println(irfcost1->valuefloat);

        tft.print(" ");
        tft.print(months[lastMonth]);
        tft.print(": ");
        tft.println(irfcost2->valuefloat);

        tft.print(" ");
        tft.print(months[lst2Month]);
        tft.print(": ");
        tft.println(irfcost3->valuefloat);

        tft.println();
        tft.println();
        tft.println(" Products");
        tft.println(" =================");

        tft.print(" ");
        tft.print(months[curMonth]);
        tft.print(": ");
        tft.println(products1->valueint);
        tft.print(" ");
        tft.print(months[lastMonth]);
        tft.print(": ");
        tft.println(products2->valueint);
        tft.print(" ");
        tft.print(months[lst2Month]);
        tft.print(": ");
        tft.println(products3->valueint);
      
        screen = 4;
        
      }
    }
    for(;;)
      ;

  }
}

void buttonPress() {
  if(screen == 3) {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
    screen = 2;
  }
  if(screen == 4) {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
    screen = 1;
  }
}  






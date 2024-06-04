
//last edit 18/07/23 03:40PM
#include<Arduino.h>
#include<Hash.h>
#define BLYNK_PRINT Serial
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include<FS.h>
#include <SPI.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
#define SENSOR  2




int GetData,rm; // signal from blynk
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor =0.21;// calibration factor 0.21;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned long totalMilliLitres;
double flowLitres;
double totalLitres;
double totalLitres1,TOTALLITRES1;





//--------------------------------------------------------------------------------------------------
char auth[] = "rp8Bz89aLXJ_a76nZBtH_7JZnNiDDRGe"; //mera : rp8Bz89aLXJ_a76nZBtH_7JZnNiDDRGe
char ssid[] = "Anti Virus";
char pass[] = "Encryption2312";  // Set password to "" for open networks.
String GAS_ID = "AKfycbzxiIRStmK1USzOldHHcyd8lWfcPwUg0oKsieELJC4GVsmi7eNxWT6f"; //AKfycbzxiIRStmK1USzOldHHcyd8lWfcPwUg0oKsieELJC4GVsmi7eNxWT6f
//https://script.google.com/macros/s/AKfycbzxiIRStmK1USzOldHHcyd8lWfcPwUg0oKsieELJC4GVsmi7eNxWT6f/exec
String string_vol,tag="Flow_meter1";

//-----------------------------------------Host & httpsPort----------------------------------------------
const char* host = "script.google.com";
const char* http_username="sohail";
const char* http_password="123456";
const int httpsPort = 443;
//-------------------------------------------------------------------------------------------------------


WiFiClientSecure client; //--> Create a WiFiClientSecure object.
WidgetLCD lcd(V0);
WidgetTerminal terminal(V3);
WidgetLED led(V4);
AsyncWebServer server(80);


inline void IRAM_ATTR pulseCounter()
{ pulseCount++; }

BLYNK_CONNECTED()
{ Blynk.syncVirtual(V10);}

BLYNK_WRITE(V10)
{ totalLitres= param.asInt();}

BLYNK_WRITE(V2)
{rm=param.asInt();}

BLYNK_WRITE(V1)
{GetData=param.asInt();}

BLYNK_WRITE(V3)
{totalLitres=param.asInt();
terminal.println("Refresh");}

String readVolume()
{unsigned long rV =totalLitres1;
    return String(rV);
 }
 

String readFlowRate()
{ unsigned long rFR =flowRate; // put here flowRate
  return String(rFR);
}


String readVolumeLive()
{ unsigned long rVL =totalLitres1; // put here flowRate
  return String(rVL);
}

String readFlowRateLive()
{ unsigned long rFRL =flowRate; // put here flowRate
  return String(rFRL);
  }
String readlastlive()
{  String rLTL=string_vol; // put here flowRate
  return (rLTL);
  }








void setup() 
{
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres ;
  totalMilliLitres;
  previousMillis;
  Serial.begin(115200);
  delay(500);
  if(!SPIFFS.begin())
   { Serial.println("An Error has occurred while mounting SPIFFS");
    return;
   }
  
  //WiFi.begin(ssid, pass); //--> Connect to your WiFi router
  Blynk.begin(auth, ssid, pass);
  delay(50);

    
  
  //----------------------------------------Wait for connection
  Serial.println("Connecting");
   while (WiFi.status() != WL_CONNECTED) {
  Serial.println(".");
  
  
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);}

  Serial.println("");
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  pinMode(SENSOR, INPUT);
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board


    
  
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  
  Serial.println("");
  terminal.print("\n");
  
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  terminal.println("Successfully connected to");
  terminal.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  terminal.print("IP address:  ");
  terminal.print(WiFi.localIP());
 terminal.flush();
  //----------------------------------------
 
  client.setInsecure();

lcd.print(0,0,"final year");
lcd.print(0,1," !!Project!! ");// use: (position X: 0-15, position Y: 0-1, "Message you want to print")
delay(2000);
lcd.clear();
attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, RISING);



server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  if(!request->authenticate(http_username,http_password))
      return request->requestAuthentication();
   request->send(SPIFFS,"/index.html");
  });


server.on("/volume", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send_P(200,"text/plain", readVolume().c_str());
  });

server.on("/FlowRate", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send_P(200,"text/plain", readFlowRate().c_str());
  });

server.on("/readVolumeLive", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send_P(200,"text/plain", readVolumeLive().c_str());
  });

server.on("/readFlowRateLive", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send_P(200,"text/plain", readFlowRateLive().c_str());
  });

server.on("/lastreading", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send_P(200,"text/plain", readlastlive().c_str());

});

server.begin();
}










void loop()
{ Blynk.run();
led.on();
//Blynk.setProperty(V4,"color","#23C48E");
  {currentMillis = millis();
   if(currentMillis - previousMillis > interval) 
  { 
    pulse1Sec = pulseCount;
    pulseCount = 0;
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
 

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
 
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;
    totalLitres1=totalLitres;


    lcd.print(0,0,totalLitres1);
    lcd.print(0,1,flowRate); 
    Blynk.virtualWrite(V10,totalLitres1);

    // Print the flow rate for this second in litres / minute
   /* Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space
*/
    }
  }led.off(); 
//Blynk.setProperty(V4, "color","#ED9D00");

unsigned long FLOWRATE= flowRate;
TOTALLITRES1=totalLitres1;
String Rate= "flowRate: " + String(FLOWRATE) + " °C";
String volume= "volume: " + String(TOTALLITRES1) + " %";
Serial.print(Rate);
Serial.print("\t");
Serial.print(volume);
Serial.print("\n");



  
   
   
   
if(GetData!=0)
{ sendData(tag,TOTALLITRES1); //--> Calls the sendData Subroutine
  Blynk.email("sohailshaikh125@gmail.com", "notification", "Reading sent successfully :  "+String(TOTALLITRES1));
  GetData=0; }

if(rm!=0)
{terminal.clear();
lcd.clear();
 Blynk.virtualWrite(V2,LOW);
 Blynk.setProperty(V2,"offLabel","Refresh");
 rm=0;}



}
















void sendData(String TAG, unsigned long TOTALLITRES11)
{
  Blynk.setProperty(V1, "color","#D3435C");
  if(!client.connect(host, httpsPort))
    {
   Serial.println("connection failed");
   terminal.print("connection failed");
   return;}
   
//String string_flow =  String(FLOWRATE1);  // String string_flow =  String(tem, DEC); 
string_vol =  String(TOTALLITRES11); 
String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + TAG + "&humidity=" + string_vol;
Serial.print("requesting URL: ");   
Serial.println(url);
terminal.print("requesting URL:");



client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
Serial.println("request sent");
terminal.print("request sent");
Blynk.setProperty(V1,"offLabel","Success");
terminal.flush();

//----------------------------------------
//----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      Blynk.setProperty(V1, "color","#23C48E");
      break;
    }
  }

String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\""))
     {
    Serial.println("esp8266/Arduino CI successfull!");
    digitalWrite(ON_Board_LED, LOW);
    delay(500);
    digitalWrite(ON_Board_LED, HIGH);
    delay(500); }
     
  else{    Serial.println("esp8266/Arduino CI has failed");
    terminal.print("Data sent:  ");
    terminal.print(string_vol);
    terminal.println("  Successfully...");
    terminal.flush();
      }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  Blynk.virtualWrite(V1,LOW);
  Blynk.setProperty(V1,"offLabel","GET DATA");
  Blynk.setProperty(V1, "color","#5F7CD8");
 
}

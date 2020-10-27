#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

byte mac[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 }; //random arduino-prefixed mac can be obtained from https://ssl.crox.net/arduinomac/
//IPAddress ip(192, 168, 0, 199); //for static - some random ip on my subnet
IPAddress gateway(192, 168, 0, 1); //for local subnet
IPAddress subnet(255, 255, 255, 0); //for local subnet
char serverName[] = "192.168.0.42"; //address of the server getting the post requests as a string (ip also as string)
int serverPort = 3000;
int sensorPin = A0; 
String probe = "A0"; //can be skipped but analog input A0 has pin #14 which looks a bit silly.
int moisture;

EthernetClient client;
HttpClient httpc = HttpClient(client, serverName, serverPort);

void setup() {
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);  // disable sd card SPI on the ethernet shield
  Serial.print(F("Starting ethernet..."));
//  Ethernet.begin(mac, ip, gateway, subnet); //for static
  if(!Ethernet.begin(mac)) Serial.println(F("failed")); //for dhcp
   else Serial.println(F("ok")); //for dhcp
  Serial.println(Ethernet.localIP());
  delay(2000);
  Serial.println(F("Ready"));
}

void loop()
{
  moisture = analogRead(sensorPin);
  Serial.print(moisture);
  moisture = map(moisture,1023,0,0,100); //mapping the 0...1023 values from the ADC in reverse to 0...100%
  Ethernet.maintain();
  String contentType = "application/x-www-form-urlencoded";
  String moistureS = String(moisture);
  String postData = String("probe=" + probe + "&moisture=" + moistureS);
  httpc.post("/readings", contentType, postData); //posting to server's /readings page
  
  int statusCode = httpc.responseStatusCode();
  String response = httpc.responseBody();
  Serial.print("Response code/status: ");
  Serial.print(statusCode);
  Serial.print("/");
  Serial.println(response);
  delay(5000); //measuring interval
  }

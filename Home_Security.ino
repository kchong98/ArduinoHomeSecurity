#include <SPI.h>
#include <WiFi101.h>
#include <Servo.h>

char ssid[] = "BELL876";        // your network SSID (name)
char pass[] = "Icnrmp2001bellfibe";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;
int doorBell = 0;

Servo myservo;

String header;
String lock = "l";

int pos = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);      // initialize serial communication
  pinMode(9, OUTPUT);      // set the pin mode
  pinMode(0, INPUT);
  myservo.attach(9);
  // for (pos = 180; pos >= 40; pos -= 1) { // goes from 180 degrees to 40 degrees
  // in steps of 1 degree
  myservo.write(40);              // tell servo to go to position 40
  delay(15);                       // waits 15ms for the servo to reach the position
  //}
  lock = "l";
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWiFiStatus();                        // you're connected now, so print out the status
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  int waterLevel = analogRead(A0);
  if (digitalRead(0) == HIGH && lock == "l") {
    doorBell = 1;
  }

  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          //client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>SMaRT.IO House</title>");
          client.println("<style>");
          client.println("a{auto; font-size: 32px; padding-bottom: 20px;}");
          client.println("p{text-align: center; font-size: 32px; padding-bottom: 20px; width: 33%; padding-top: 20px; padding-bottom: 20px;}");
          client.println(".knock{font-weight:bold; background-color:lightgrey; color: black;}");
          client.println(".flood{font-weight:bold; background-color:lightblue; color: blue;}");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Security-Maintained and Real-Time Internet Operated House</h1>");

          if (header.indexOf("GET /u") >= 0 && lock == "l") {
            myservo.write(180);              // tell servo to go to position 180
            delay(15);                       // waits 15ms for the servo to reach the position
            lock = "u";
            doorBell = 0;
          }
          else if (header.indexOf("GET /l") >= 0 && lock == "u") {
            myservo.write(40);              // tell servo to go to position 40
            delay(15);                       // waits 15ms for the servo to reach the position
            lock = "l";
          }

          if (lock == "u") {
            client.println("<a href=\"/l\"><p>LOCK</p></a>");
          }
          //client.println("<br />");
          else if (lock == "l") {
            client.println("<a href=\"/u\"><p>UNLOCK</p></a>");
          }
          if (waterLevel > 100) {
            client.println("<p class=\"flood\">FLOOD ALERT</p>");
          }
          else {
            client.println("<p>No Flood</p>");
          }
          if (doorBell == 1 && lock == "l") {
            client.println("<p class=\"knock\">Knock! Knock!</p>");
          }
          else {
            client.println("<p>Nobody at the Door</p>");
          }

          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    header = "";
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

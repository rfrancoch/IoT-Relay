#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "FRANCO ARBUROLA";
const char *password = "05022010";
const byte gpio = 0;

ESP8266WebServer server ( 80 );

void getPage(boolean turnON) {
  char temp[500];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  byte gpioChange = 0;
  byte gpioState = 0;

  if (turnON) {
    gpioState = 1;
  } else {
    gpioState = 0;
    gpioChange = 1;
  }
  digitalWrite(gpio, gpioState);

  snprintf ( temp, 500,
             "<html>\
  <head>\
    <title>ESP8266 Server</title>\
    <style>\
      body { background-color: #ebebeb; font-family: Arial, Helvetica, Sans-Serif; Color: #3b3b3b; }\
    </style>\
  </head>\
  <body>\
    <h1>IOT Relay</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <center><a href='/gpio/%d'><img src=\"/icon%d.svg\" /></a></center><br\>\
  </body>\
</html>",
             hr, min % 60, sec % 60, gpioChange, gpioState
           );
  server.send ( 200, "text/html", temp );
}

void handleRootOFF() {
  getPage(false);
}

void handleRootON() {
  getPage(true);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

void drawIcon(boolean isOn) {
  String svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"128\" height=\"128\" viewBox=\"-4 -4 36 36\" xml:space=\"preserve\"><path ";
  if (isOn) {
    svg += " fill=\"yellow\" stroke=\"gray\" ";
  } else {
    svg += " fill=\"gray\" stroke=\"black\" ";
  }
  svg += "stroke-width=\"1\" d=\"M14 12h-4v-12h4v12zm4.213-10.246l-1.213 1.599c2.984 1.732 5 4.955 5 8.647 0 5.514-4.486 10-10 10s-10-4.486-10-10c0-3.692 2.016-6.915 5-8.647l-1.213-1.599c-3.465 2.103-5.787 5.897-5.787 10.246 0 6.627 5.373 12 12 12s12-5.373 12-12c0-4.349-2.322-8.143-5.787-10.246z\"></path></svg>\n";
  server.send ( 200, "image/svg+xml", svg);
}

void drawIconON() {
  drawIcon(true);
}

void drawIconOFF() {
  drawIcon(false);
}

void setup ( void ) {
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, 0);

  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }
  server.on ( "/", handleRootOFF );
  server.on ( "/gpio/1", handleRootON );
  server.on ( "/gpio/0", handleRootOFF );
  server.on ( "/icon1.svg", drawIconON );
  server.on ( "/icon0.svg", drawIconOFF );
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  server.handleClient();
}


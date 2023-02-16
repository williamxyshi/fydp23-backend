#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <utility/wifi_drv.h>

char ssid[] = "deadass";
char pass[] = "ilovemywife";

char brew_host[] = "fydp.eba-twsqhru6.us-east-1.elasticbeanstalk.com";
int port = 80;      // port

int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

void setup() {
  Serial.begin(9600);
  while (!Serial);

  WiFiDrv::pinMode(25, OUTPUT);  //GREEN
  WiFiDrv::pinMode(26, OUTPUT);  //RED
  WiFiDrv::pinMode(27, OUTPUT);  //BLUE
  
  enable_WiFi();
  connect_WiFi();

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.println("Connecting to brew daddy");
  if (client.connect(brew_host, port)) {
    Serial.print("connected to brew daddy: ");
    Serial.println(brew_host);

    client.println("GET / HTTP/1.1");
    client.println("Host: fydp.eba-twsqhru6.us-east-1.elasticbeanstalk.com");
    client.println("Connection: keep-alive");
    client.println("Accept: */*");

    client.println();
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  String response = "";
  while (client.available()) {
    response += char(client.read());
  }

  if (response != "") {
    Serial.println(response);

    String json_str = skipResponseHeaders(response);

    // JSON doc used to deserialize JSON strings
    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, json_str);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    } else {
      Serial.println("Deserialized doc[data] (json is valid)");
      WiFiDrv::digitalWrite(27, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(26, HIGH);
      delay(500);
      WiFiDrv::digitalWrite(25, HIGH);
      delay(10000);
      WiFiDrv::digitalWrite(25, LOW);
      WiFiDrv::digitalWrite(26, LOW);
      WiFiDrv::digitalWrite(27, LOW);      
    }
  }  
}

String skipResponseHeaders(String response) {
  int i = response.indexOf("\r\n\r\n");
  if (i != -1) {
    String json_str = response.substring(i + 4);
    Serial.print("Skipping headers, we get: ");
    Serial.println(json_str);
    return json_str;   
  }
  return "";
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

/* Fuentes:

https://www.youtube.com/watch?v=QreTYtv3lsc
https://github.com/riccibitti/CloudMyIp
----------------------------------------------------------------------------------------------------------------------------------------------------------
Descripcion:

CloudMyIp is an inexpensive ($3) IOT device that reads the public IP address assigned to your router by your ISP (Internet Service Provider), and publishes it on a web page anyone can read with a link. Great for accessing your home network or IP cameras from abroad, saving the money for a subscription to a dynamic dns service (or the time spent for similar "free" services requiring you to connect and read their ads once a month for continued service).

How it works:

an ESP8266 module (e.g. NodeMCU) reads the home network public IP address using the ipify.org API
the module echoes back the public IP address to the cloud, publishing a "dweet" using the dweet.io APIs.
a web application running on freeboard.io reads the dweet, and presents the public IP address nicely using any web browser.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
Pagina a visualizar la ip:    http://dweet.io/get/latest/dweet/for/lincoyan

 CODIGO PARA EL ESP8266:
 */

 #include "ESP8266WiFi.h"
// #include <WiFi.h>

// WiFi parameters
char ssid [] = "movistar_CC4B58";
char key[] = "casabicho";       // your network key
int keyIndex = 0;              // your network key Index numbe
int status = WL_IDLE_STATUS;   // the Wifi radio's status
String dweetThingName("salas525");
long x = 0;

void setup() {
  
  // Start Serial
  Serial.begin(9600);
  delay(10);
  
  // We start by connecting to a WiFi network
   WiFi.mode(WIFI_STA);
WiFi.begin(ssid, key);
Serial.print("Conectando a la red wifi, SSID: ");
  x= millis();
  while (WiFi.status() != WL_CONNECTED) {
    
    Serial.println(ssid);
    // status = WiFi.begin(ssid, key);
    //  Serial.print(status);
  
    delay(500);
    
    Serial.print(".");
        if((millis() - x) > 5000){
          Serial.print("RESETEANDO POR FALLA CONEXION WIFI");
      ESP.restart();
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



String getIp()
{
  WiFiClient client;
  if (client.connect("api.ipify.org", 80)) 
  {
      Serial.println("connected");
      client.println("GET / HTTP/1.0");
      client.println("Host: api.ipify.org");
      client.println();
  } else {
      Serial.println("Connection to ipify.org failed");
      delay(500);
      ESP.restart();
      return String();
  }
  delay(5000);
  String line;
  while(client.available())
  {
    line = client.readStringUntil('\n');
    Serial.println(line);
  }
  return line;
}


bool dweet( String ip)
{

  WiFiClient client;
  if (!client.connect("dweet.io", 80)) 
  {
    Serial.println("dweet failed, RESETEANDO...");
    ESP.restart();
    return false;
  }  
  else 
  {
    Serial.println("dweeting ip address as " + dweetThingName +" [" + ip + "]" );
    
    client.println( String("GET /dweet/for/" + dweetThingName + "?IPaddress=") + ip + " HTTP/1.1");
    client.println( "Host: dweet.io" );
    client.println( "Connection: close");
    client.println("");
    client.println("");
    
    delay(1000);
  
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    };
    return true;
  }; 
}


void loop() 
{
  String ip = getIp();
  if ( ip != String() )
    dweet( ip ) ;
  // repeat after one minute
  delay(60000);
}

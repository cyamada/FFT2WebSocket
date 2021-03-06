#include <fix_fft.h>  
#include <Ethernet.h>
#include <SPI.h>
#include <sha1.h>
#include <WebSocketClient.h>

static void hang()
{
  while(true) {}
}

EthernetClient client;

class MyWebSocket : public websocket::WebSocket
{
public:
    MyWebSocket(Client& client)
        : websocket::WebSocket(client)
    { }
    
protected:
  
    virtual void onClose()
    {
      Serial.println("WebSocket closed");
      hang();
    }  
    
    virtual void onError(websocket::Result error)
    {
      Serial.print("WebSocket error:");Serial.println(error);
    }
    
    virtual void onTextFrame(char const* msg, uint16_t size, bool isLast)
    {  
      Serial.print("Got text frame: '");Serial.print(msg);Serial.println("'");
    }
};

MyWebSocket webSocketClient(client);

byte ip[] = { 10, 15, 0, 85 };
byte gateway[] = { 10, 15, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
//char server[] = "169.254.85.190";  //websocket server
char server[] = "10.15.0.219";
int sample_size = 2;
int sample_const = 32;
int sample = sample_size * sample_const;
char im[64], data[64];
int i = 0, val;
//int peak = 0;

void setup() {                             
  Serial.begin(115200);
  
  uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x73, 0xBF }; //mac address of the ethernet board
  Ethernet.begin(mac, ip, gateway, subnet);
  
  if (client.connect(server, 8080)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    hang();// Hang on failure
  }
  Serial.println("end setup");
  
  if (websocket::clientHandshake(client, server, "/") == websocket::Success_Ok) {  
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");  
    hang();// Hang on failure
  }
  client.flush();
  
}

void loop() {
  String dataStr = "{\"channel\":\"in\",\"data\":\"";
  //String dataStr;
  if (client.connected()) {
    
    for (i=0; i < sample; i++) {                                     
      val = analogRead(0);        //electret microphone attached to A0                            
      data[i] = val/4 -sample  ;                                       
      im[i] = 0;                                                      
    }
  
    fix_fft(data,im,4 + sample_size,0);
  
    for (i=1; i< (sample / 2);i++) {       //values from 0-500Hz are trash      
      data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);  
      /*
      if (data[i] > peak) {
        peak = data[i] + 1;
      }
      */
      
        String temp = String((data[i] + 0)); 
        dataStr += temp + " ";
     
    }
    dataStr += "\"}";
    
    Serial.println(dataStr);  
      
    char arr[126];                   //126 char max
    dataStr.toCharArray(arr, 126);
    webSocketClient.sendData(arr);
    //String peakStr = String(peak);
    //Serial.println(peak);
    //Serial.println(dataStr);
    //itoa(peak, dat, 6);
    //peak = 0;
    client.flush();
    delay(1);
  } else {
    Serial.println("The connection has been interrupted");
    client.flush();
    delay(10000);
    reconnect();
  }
}  
 
void reconnect() {
  uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x73, 0xBF }; //mac address of the ethernet board
  Ethernet.begin(mac, ip, gateway, subnet);
  
  if (client.connect(server, 8080)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    hang();// Hang on failure
  }
  Serial.println("end setup");
  
  if (websocket::clientHandshake(client, server, "/") == websocket::Success_Ok) {  
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");  
    hang();// Hang on failure
  }
  client.flush();
}
  

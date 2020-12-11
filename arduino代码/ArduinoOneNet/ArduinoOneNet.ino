#include "ESP8266.h"
#include "dht11.h"
#include "SoftwareSerial.h"
#include "stdio.h"
#include "stdlib.h"
#include "U8glib.h"

static const unsigned char wendu[3][32] PROGMEM={
{0x00,0x00,0xC4,0x1F,0x48,0x10,0x48,0x10,0xC1,0x1F,0x42,0x10,0x42,0x10,0xC8,0x1F,
0x08,0x00,0xE4,0x3F,0x27,0x25,0x24,0x25,0x24,0x25,0x24,0x25,0xF4,0x7F,0x00,0x00},/*"温",0*/
/* (16 X 16 , 宋体 )*/

{0x80,0x00,0x00,0x01,0xFC,0x7F,0x44,0x04,0x44,0x04,0xFC,0x3F,0x44,0x04,0x44,0x04,
0xC4,0x07,0x04,0x00,0xF4,0x0F,0x24,0x08,0x42,0x04,0x82,0x03,0x61,0x0C,0x1C,0x70},/*"度",1*/
/* (16 X 16 , 宋体 )*/

{0x00,0x00,0xE4,0x1F,0x28,0x10,0x28,0x10,0xE1,0x1F,0x22,0x10,0x22,0x10,0xE8,0x1F,
0x88,0x04,0x84,0x04,0x97,0x24,0xA4,0x14,0xC4,0x0C,0x84,0x04,0xF4,0x7F,0x00,0x00},/*"湿",2*/
/*(16 X 16 , 宋体 )*/
};

//配置ESP8266WIFI设置
#define SSID "HUAWEI Mate 30 5G"
#define PASSWORD "77777777"  
#define HOST_NAME "api.heclouds.com"  
#define DEVICE_ID "642856759"       
#define HOST_PORT (80)              
String APIKey = "j5M=Gu7Te=MPxvrEtaAfuOSMS1s=";

#define INTERVAL_SENSOR 5000 

//创建dht11示例

dht11 DHT11;

#define DHT11PIN 4

SoftwareSerial mySerial(3, 2);
ESP8266 wifi(mySerial);

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); 

/*void draw(void) {
 u8g.drawXBMP( 0, 16,16, 16, wendu[0]);
 u8g.drawXBMP( 16, 16,16, 16, wendu[1]);
 u8g.drawXBMP( 0, 32,16, 16, wendu[2]);
 u8g.drawXBMP( 16, 32,16, 16, wendu[1]);
}*/

void setup()
{
  pinMode(10,OUTPUT);
  mySerial.begin(115200); 
  Serial.begin(9600);     
  Serial.print("setup begin\r\n");

  //以下为ESP8266初始化的代码
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  Serial.println("");
  Serial.print("DHT11 LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);

  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
}

unsigned long net_time1 = millis(); 
void loop(){
     
  if (net_time1 > millis())
    net_time1 = millis();

  if (millis() - net_time1 > INTERVAL_SENSOR) 
  {

    int chk = DHT11.read(DHT11PIN);

    Serial.print("Read sensor: ");
    switch (chk) {
      case DHTLIB_OK:
        Serial.println("OK");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }

    float sensor_hum = (float)DHT11.humidity;
    float sensor_tem = (float)DHT11.temperature;
    Serial.print("Humidity (%): ");
    Serial.println(sensor_hum, 2);

    Serial.print("Temperature (oC): ");
    Serial.println(sensor_tem, 2);
    Serial.println("");

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("create tcp ok\r\n");
      char buf[10];     
      String jsonToSend = "{\"Temperature\":";
      dtostrf(sensor_tem, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += ",\"Humidity\":";
      dtostrf(sensor_hum, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += "}";
      String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "Content-Length:";
      postString += jsonToSend.length();
      postString += "\r\n";
      postString += "\r\n";
      postString += jsonToSend;
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str();

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray));
      Serial.println("send success");
      if (wifi.releaseTCP()) { 
        Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
      }
      postArray = NULL;
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");

    net_time1 = millis();
  }
  
    float n =(float)DHT11.temperature;
    if(n>=20.00)
    {
     digitalWrite(10,HIGH);
      }
      
    u8g.firstPage();  
    do {
      u8g.setFont(u8g_font_7x13);
      u8g.drawXBMP( 4, 0,16, 16, wendu[0]);
      u8g.drawXBMP( 18, 0,16, 16, wendu[1]);
      u8g.setPrintPos(48,16);
      u8g.print((float)DHT11.temperature);
      u8g.drawXBMP( 4, 18,16, 16, wendu[2]);
      u8g.drawXBMP( 18, 18,16, 16, wendu[1]);
      u8g.setPrintPos(48,32);
      u8g.print((float)DHT11.humidity);
    } while( u8g.nextPage() );

}

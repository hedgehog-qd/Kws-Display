#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <avr/pgmspace.h>
#include <ArduinoJson.h>//Json库
#include <HttpClient.h>//HTTP库
#include <TJpg_Decoder.h>

extern const uint16_t SecondScreen[0xE100] PROGMEM;//第二屏（老毅定制主屏）
extern const uint16_t FirstScreen[0x3840] PROGMEM;//第一屏（吃小孩塞牙）

const char *ssid     = "WIFI_SSID";//输入Wifi账号
const char *password = "WIFI_KEY";//输入WiFi密码
const char* host="api.bilibili.com";//B站API
String UID="34848891";// B站用户UID
int following=0;// 关注数
int follower=0;// 粉丝数
int LCD_BL_PIN = 5;//TFT屏背光控制脚！！不懂别动！！
int LCD_BL_PWM = 80;//屏幕亮度设定（小于80将无法亮屏）
int WAIT_SECOND = 300;//B站API轮询延迟时间（秒）

void setup() {
  //gpio.write(5, gpio.HIGH);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM*10));
  // put your setup code here, to run once:
  Serial.begin(9600);//串口通讯波特率
  /*---------------------开机初始化版本显示-----------------------*/
  Serial.println("KWS FANS SCREEN FRIMWORK VER. 1.0 FINAL");
  Serial.println("TTL DEBUG MODE");
  Serial.println("COMPILE AT: 2022/02/23");
  Serial.println("BY hedgehog_qd && Fuller12");
  Serial.println("OPENSOURCE:");
  Serial.println("VISIT: http://main.hedgehog-qd.xyz");
  Serial.println("&& https://github.com/hedgehog-qd");
  Serial.println("BOOTING NOW...");
  tft.setTextSize(2);
  tft.println("KWSFC FRIMEWORK");
  tft.println("VER. 1.0");
  tft.println("By hedgehog_qd & Fuller12");
  tft.println("BOOTING NOW...");
  delay(500);
  /*-----------------------------------------------------------*/
  WiFi.begin(ssid, password);//连接WiFi

  tft.setTextSize(2);
  tft.print("CONNECTING TO: ");
  tft.println(ssid);
  int pp = 0;
  while ( WiFi.status() != WL_CONNECTED ){
    delay ( 500 );
    Serial.print ( "." );
    pp++;
    tft.print("CONNECTING WiFi: ");
    tft.println(pp);
  }
  
  Serial.println("WiFi connected");//连接成功
  Serial.print("IP address: ");//打印IP地址
  Serial.println(WiFi.localIP());
  
  
  //tft.fillScreen();
  tft.fillScreen(TFT_BLACK);
  
  tft.setSwapBytes(true);// RGB->BGR 加上这句才能正确显示颜色。
  tft.pushImage(120,120,120,120,FirstScreen);
  tft.setCursor(10,10,1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("WIFI CONNECTED:");
  tft.setCursor(10,60,1);
  
  tft.setTextColor(TFT_BLUE);
  tft.println(WiFi.localIP());
  delay(5000);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // put your main code here, to run repeatedly:


  //创建TCP连接
  WiFiClient client;
  const int httpPort=80;
  if(!client.connect(host,httpPort)){
    tft.fillScreen(TFT_BLACK);
    Serial.println("connection failed");//网络请求无响应打印连接失败
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.print("CONNECTION FAILED!");
    return;
  }//URL请求地址
  String url="/x/relation/stat?vmid="+UID+"&jsonp=jsonp";// B站粉丝数链接，注意修改UID
  //发送网络请求 
  client.print(String("GET ")+url+" HTTP/1.1\r\n"+"Host: "+host+"\r\n"+"Connection: close\r\n\r\n");
  delay(5000);
  //定义answer变量用来存放请求网络服务器后返回的数据
  String answer;
  while(client.available()){
    String line=client.readStringUntil('\r');
    answer+=line;
    }
  //断开服务器连接
  client.stop();
  Serial.println();
  Serial.println("closing connection");
  //获得json格式的数据
  String jsonAnswer;
  int jsonIndex;
  //找到有用的返回数据位置i 返回头不要
  for(int i=0;i<answer.length();i++){
    if(answer[i]=='{'){
      jsonIndex=i;
      break;
    }
  }
  jsonAnswer=answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  

    

  StaticJsonDocument<256> doc;
  
  deserializeJson(doc, jsonAnswer);

  int code = doc["code"]; // 0
  const char* message = doc["message"]; // "0"
  int ttl = doc["ttl"]; // 1
  JsonObject data = doc["data"];
  long data_mid = data["mid"];//34848891
  Serial.println(data_mid);
  String data_following = data["following"];// 26
  int data_whisper = data["whisper"];// 0
  int data_black = data["black"];// 0
  String data_follower = data["follower"];//196
  following =data["following"];//59
  follower=data["follower"];//411
  Serial.println("Following: ");
  Serial.println(data_following);
  Serial.println("follower: ");
  Serial.println(data_follower);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);// RGB->BGR 加上这句才能正确显示颜色。
  tft.pushImage(0,0,240,240,SecondScreen);
  tft.setCursor(1,1,1);
  tft.setTextSize(2);
  tft.println(" ");
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW);
  //tft.println("   -Kws-");
  //tft.println("FOLLOWING:");
  //tft.println(" ");
  tft.print("  ");
  tft.println(data_following);
  tft.setTextSize(2);
  //tft.println("FOLLOWER:");
  tft.println(" ");
  tft.println(" ");
  tft.setTextSize(1);
  tft.println(" ");
  //tft.print(" ");
  tft.setTextSize(4);
  tft.println(data_follower);
  delay(1000*WAIT_SECOND);
}

#include <Wire.h>  // Arduino IDE 內建
// LCD I2C Library，從這裡可以下載：
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
#include <LiquidCrystal_I2C.h>
const int ledPin = 13;
const int sensorPin = A0;
const double alpha = 0.75;              // smoothing參數 可自行調整0~1之間的值
const double beta = 0.5;                // find peak參數 可自行調整0~1之間的值
const int period = 20;                  // sample脈搏的delay period
int old_beat = 0;
int count_down = 10;
int LED_control=2;
int old_LED_control=2;

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

void setup()
{
  for(int i=2;i<=11;i++)
    pinMode(i, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);               // Set the baud rate of the Serial Monitor
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光

  // 閃爍三次
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // 開啟背光
    delay(250);
    lcd.noBacklight(); // 關閉背光
    delay(250);
  }
  lcd.backlight();

  // 輸出初始化文字
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Hello, WongWong!");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Heart Beat v2.0");
  delay(1000);

//  // 告知使用者可以開始手動輸入訊息
//  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print("Use Serial Mon");
//  lcd.setCursor(0, 1);
//  lcd.print("Type to display"); 

}

void loop()
{
  // 當使用者手動輸入訊息
  if (Serial.available()) {
    // 等待一小段時間，確認資料都接收下來了
    delay(100);
    // 清除舊訊息
    lcd.clear();
    // 讀取新訊息
  }
  senseHeartRate();                   // 測量心跳
    
}
void lcd_print()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  if (old_beat < 10)
    lcd.print("( -_- ) zzz");
  else if (old_beat < 60)
    lcd.print("( >_< ) !!!");
  else 
    lcd.print("( ^_^ )");
  lcd.print(" ");
  lcd.print(count_down);
  lcd.setCursor(0, 1);
  lcd.print("Heart Beat: ");
  lcd.print(old_beat);
}

void senseHeartRate()
{
    int count = 0;                              // 記錄心跳次數
    double oldValue = 0;                        // 記錄上一次sense到的值
    double oldChange = 0;                       // 記錄上一次值的改變
    count_down = 10;
    unsigned long startTime = millis();         // 記錄開始測量時間
   
    while(millis() - startTime < 10000) {       // sense 10 seconds
        int rawValue = analogRead(sensorPin);   // 讀取心跳sensor的值

        if(((10000-(millis() - startTime))/1000) < count_down)
        {
          lcd_print();
          count_down--;
        }
        //Serial.print(millis() - startTime);
        //Serial.print(" ");
//        lcd.clear();
//        lcd.setCursor(0, 0);
//        lcd.print("( ^_^ ) ");
//        lcd.print((10000-(millis() - startTime))/100);
//        lcd.setCursor(0, 1);
//        lcd.print("heart beat: ");
//        lcd.print(old_beat);
//        if((millis() - startTime < 10000)%10==0) 
//        {
//          Serial.print(rawValue);
//          Serial.print(" ");
//        }
        double value = alpha*oldValue + (1-alpha)*rawValue;     //smoothing value
   
        //find peak
        double change = value-oldValue;         // 計算跟上一次值的改變量
        if (change>beta && oldChange<-beta) {   // heart beat
            count = count + 1;
        }
        LED_control = ((millis() - startTime)/100)%10+2;
        if(LED_control != old_LED_control)
        {
          digitalWrite(old_LED_control, LOW);    // turn the LED off by making the voltage LOW
          digitalWrite(LED_control, HIGH);   // turn the LED on (HIGH is the voltage level)
          old_LED_control = LED_control;
        }
         
        oldValue = value;
        oldChange = change;
        delay(period);
    }
    old_beat = count;
    lcd_print();
    
    Serial.print("\nheart beat: ");
    Serial.println(count);
}

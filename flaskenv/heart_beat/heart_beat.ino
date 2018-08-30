const int ledPin = 13;
const int sensorPin = A0;
const double alpha = 0.75;              // smoothing參數 可自行調整0~1之間的值
const double beta = 0.5;                // find peak參數 可自行調整0~1之間的值
const int period = 20;                  // sample脈搏的delay period

void setup()
{
    pinMode(sensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);               // Set the baud rate of the Serial Monitor
}

void loop()
{
    senseHeartRate();                   // 測量心跳
    
}
void senseHeartRate()
{
    int count = 0;                              // 記錄心跳次數
    double oldValue = 0;                        // 記錄上一次sense到的值
    double oldChange = 0;                       // 記錄上一次值的改變
       
    unsigned long startTime = millis();         // 記錄開始測量時間
   
    while(millis() - startTime < 10000) {       // sense 10 seconds
        int rawValue = analogRead(sensorPin);   // 讀取心跳sensor的值
        //Serial.print(millis() - startTime);
        //Serial.print(" ");
        if((millis() - startTime < 10000)%10==0) 
        {
          Serial.print(rawValue);
          Serial.print(" ");
        }
        double value = alpha*oldValue + (1-alpha)*rawValue;     //smoothing value
   
        //find peak
        double change = value-oldValue;         // 計算跟上一次值的改變量
        if (change>beta && oldChange<-beta) {   // heart beat
            count = count + 1;
        }
         
        oldValue = value;
        oldChange = change;
        delay(period);
    }
    Serial.print("\nheart beat: ");
    Serial.println(count);
}

#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Wire.h>
#include <dht11.h>
#define DHT11PIN 2
dht11 DHT11;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 
  int map_low = 420;
  int map_high = 1015;
  int in3=7;
  int in4=8;
  int in1=9;
  int in2=10;
  int a1 = 0;
  int b1 = 0;
  int c1 = 0;
  int d = 0;
QueueHandle_t queue_1;
struct Garden {
  int x1;//nhiet do   Pin2
  int x2;//do am      
  int x3;//anh sang 3
  int x4;//do am dat
  int x5;// cam bien mua
  int current;// chuyen analog sang phan tram
};
void TaskCambien(void *pvParameter);
void TaskDieuKhien(void *pvParameter);
void TaskHienthi(void *pvParameter);
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(53, OUTPUT);
  queue_1 = xQueueCreate(5, sizeof(struct Garden));
 if (queue_1 != NULL)
  {
    xTaskCreate(TaskCambien, "cambien", 128, NULL, 1, NULL);
    xTaskCreate(TaskDieukhien, "Dieukhien", 128, NULL, 2, NULL);
    xTaskCreate(TaskHienthi, "Hienthi", 128, NULL, 2, NULL);
  }
}
void loop() {
}
void TaskCambien(void * pvParameters) {
 struct  Garden a;
  int xStatus;
  int dht;
  for ( ; ; ) {
    dht = DHT11.read(DHT11PIN);
    a.x1 = (int)DHT11.temperature,2;
    a.x2 = (int)DHT11.humidity,2;
    a.x3 = analogRead(A0);
    a.x4 = analogRead(A1);
    a.current = map(a.x4, map_high, map_low, 0, 100);
    a.x5 = digitalRead(3);
     if(a.current > 100)
      {
        a.current = 100;
        }
     if(a.current < 0)
        a.current = 0;
  xStatus = xQueueSend(queue_1, &a, portMAX_DELAY);
    if (xStatus != pdPASS)
    {
      Serial.print("dont send");
    }
  }
}
void TaskDieukhien(void * pvParameters) {
  struct Garden b;
  int xStatus;
  for (;;)
  {
    if ( uxQueueMessagesWaiting( queue_1 ) != 5 )
    {
      Serial.println( "Queue should have been empty!\r\n" );
    }
    xStatus = xQueueReceive(queue_1, &b, portMAX_DELAY);
    if (xStatus == pdPASS)
    {
 //********//cam bien nhiet do DHT11   
     if (b.x1 <35  && a1 == 0) { 
    delay(1000);
    analogWrite(in3,80);
    analogWrite(in4,LOW);
    delay(1200);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
          a1 =1;
  }  else if (b.x1 >= 35 && a1 == 1){
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
          a1 =0;
  }
  if (b.x1 >= 35 && b1 == 0) { 
   delay(1000);
   analogWrite(in3,LOW);
   analogWrite(in4,80);
   delay(1200);
   digitalWrite(in3,LOW);
   digitalWrite(in4,LOW);
            b1 =1;
  } 
  else if (b.x1 < 35 && b1 == 1)
  {
     digitalWrite(in3,LOW);
     digitalWrite(in4,LOW);
           b1 =0;
  }
 //********//cam bien do am dat    
     if(b.current > 40)
     {
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    lcd.setCursor(13, 0);
    lcd.print("OFF");
    digitalWrite(13, HIGH);
      }
      else
      {
    analogWrite(in1,80);
    digitalWrite(in2,LOW);
    lcd.setCursor(14, 0);
    lcd.print("ON");
    digitalWrite(13, LOW);
      }
    // ********//cam bien anh sang
  if(b.x3 > 500)
  {
    digitalWrite(14, HIGH);
    digitalWrite(15, HIGH);
    }
    else 
   digitalWrite(14, LOW);
   digitalWrite(15,LOW);
    // ********//cam bien mua
     if (b.x5 == 1  && c1 == 0) { 
   delay(1000);
   analogWrite(in3,80);
   analogWrite(in4,LOW);
   delay(1200);
   digitalWrite(in3,LOW);
   digitalWrite(in4,LOW);
            c1 = 1;
  }  else if (b.x5 == 0 && c1 == 1)
  {
   digitalWrite(in3,LOW);
   digitalWrite(in4,LOW);
            c1 =0;
  }
  if (b.x5 == 0 && d == 0)
  { 
    delay(1000);
    analogWrite(in3,LOW);
    analogWrite(in4,80);
    delay(1200);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
         d =1;
  }  
  else if ( b.x5 == 1 && d == 1)
  {
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
        d =0;
  }
    }  
      delay(500);
  }
  }
void TaskHienthi(void * pvParameters) 
{
 struct Garden b;
  int xStatus;
  for (;;)
  {
    if ( uxQueueMessagesWaiting( queue_1 ) != 5 )
    {
      Serial.println( "Queue should have been empty!\r\n" );
    }
    xStatus = xQueueReceive(queue_1, &b, portMAX_DELAY);
    if (xStatus == pdPASS)
    {
 //********** Hien thi len monitor
      Serial.print("nhiet do = ");
      Serial.println(b.x1);
      Serial.print("do am kk = ");
      Serial.println(b.x2);
      Serial.print("anh sang = ");
      Serial.println(b.x3);
      Serial.print("do am dat = ");
      Serial.println(b.current);
      Serial.print("mua = ");
      Serial.println(b.x5);
  //********** Hien thi len lcd
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(b.x1);
    lcd.print((char)0xDF);
    lcd.setCursor(4,1);
    lcd.print(b.x2);
    lcd.print("%");
    lcd.setCursor(0, 0);
    lcd.print("Moisture:");
    lcd.setCursor(10, 0);
    lcd.print(b.current);
    lcd.print("%");
    // *********code hien thi Hot
         if (b.x1 >= 35){
         lcd.setCursor(9,1);
         lcd.print("-Hot-");
         Serial.print("-Hot-");}
    // *********code hien thi Rain
         if(b.x5 == 0){    
         lcd.setCursor(9,1);
         lcd.print("-Rain-");
         Serial.print("-Rain-");}
}  
delay(500);
}}

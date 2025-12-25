#define BLYNK_TEMPLATE_ID "TMPL6lZN2iMD0"
#define BLYNK_TEMPLATE_NAME "canthongminh"
#define BLYNK_AUTH_TOKEN "UKcg6qW4e5lorfN9rcwziawSkKp1Lk2w"

#include "HX711.h"
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "Adafruit_Thermal.h"
#include "HardwareSerial.h"

#define DOUT  19
#define CLK   18
HX711 scale(DOUT, CLK);

char auth[] = "UKcg6qW4e5lorfN9rcwziawSkKp1Lk2w";
char ssid[] = "Minh Ne";
char pass[] = "88888888";

LiquidCrystal lcd(32, 33, 25, 26, 27, 14);

HardwareSerial mySerial(2); 
Adafruit_Thermal printer(&mySerial);

// Calibration & pricing
float calibration_factor = 207146.47;
float weight_gram = 0;
float weight_kg = 0;
float last_weight_gram = -1;
long price = 0;
long last_price = -1;
int unit_price = 0; 
bool showPrice = false;
unsigned long invoice_number = 1;

void setup() {
  Serial.begin(115200);
  Serial.println("Khoi dong...");

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dang khoi dong...");
  delay(2000);

  WiFi.begin(ssid, pass);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi OK");
  } else {
    Serial.println("WiFi FAIL");
  }

  Blynk.config(auth);
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.connect(10000);
    if (Blynk.connected()) {
      Serial.println("Blynk OK");
    } else {
      Serial.println("Blynk FAIL");
    }
  }

  scale.set_scale();
  scale.tare();
  delay(500);
  Serial.println("Can san sang!");

  mySerial.begin(115200, SERIAL_8N1, 16, 17);
  delay(1000);
  printer.begin();
  printer.setDefault();
  Serial.println("May in san sang!");

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("San sang can");
  delay(1000); 
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Khoi luong");
  lcd.setCursor(7, 1); 
  lcd.print("0g");
}

// V0: Tare (only reset weight)
BLYNK_WRITE(V0) {
  int btn = param.asInt();
  if (btn == 1) {
    scale.set_scale();
    scale.tare();
    weight_gram = 0;
    weight_kg = 0;
    last_weight_gram = -1; 
    delay(500);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Moi Dat Lai!");
    delay(1000);
    lcd.clear();
    if (showPrice) {
      lcd.setCursor(4, 0);
      lcd.print("Gia tien");
      lcd.setCursor(5, 1);
      lcd.print("0 VND");
    } else {
      lcd.setCursor(3, 0);
      lcd.print("Khoi luong");
      lcd.setCursor(7, 1);
      lcd.print("0g");
    }
  }
}

// V1: Chuyển đổi hiển thị
BLYNK_WRITE(V1) {
  int state = param.asInt();
  showPrice = (state == 1);
  lcd.clear();
  if (showPrice) {
    lcd.setCursor(4, 0);
    lcd.print("Gia tien");
    char displayStr[16];
    sprintf(displayStr, "%ld VND", price);
    int cursorPos = (16 - strlen(displayStr)) / 2;
    lcd.setCursor(cursorPos, 1);
    lcd.print(displayStr);
  } else {
    lcd.setCursor(3, 0);
    lcd.print("Khoi luong");
    char displayStr[16];
    if (weight_gram > 1000) {
      sprintf(displayStr, "%.2fkg", weight_kg);
    } else {
      sprintf(displayStr, "%dg", (int)round(weight_gram));
    }
    int cursorPos = (16 - strlen(displayStr)) / 2;
    lcd.setCursor(cursorPos, 1);
    lcd.print(displayStr);
  }
}

// V2: In hóa đơn
BLYNK_WRITE(V2) {
  int btn = param.asInt();
  if (btn == 1) {
    if (unit_price == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nhap don gia!");
      delay(1500);
      lcd.clear();
      if (showPrice) {
        lcd.setCursor(4, 0);
        lcd.print("Gia tien");
        char displayStr[16];
        sprintf(displayStr, "%ld VND", price);
        int cursorPos = (16 - strlen(displayStr)) / 2;
        lcd.setCursor(cursorPos, 1);
        lcd.print(displayStr);
      } else {
        lcd.setCursor(3, 0);
        lcd.print("Khoi luong");
        char displayStr[16];
        if (weight_gram > 1000) {
          sprintf(displayStr, "%.2fkg", weight_kg);
        } else {
          sprintf(displayStr, "%dg", (int)round(weight_gram));
        }
        int cursorPos = (16 - strlen(displayStr)) / 2;
        lcd.setCursor(cursorPos, 1);
        lcd.print(displayStr);
      }
      return;
    }

    printer.wake();
    printer.setDefault();
    printer.justify('C');

    printer.println(F("CUA HANG TAP HOA"));
    printer.println(F("KHO ABC"));
    printer.println(F("----------------"));

    printer.print(F("SO HD: "));
    printer.println(invoice_number);
    printer.println(F("----------------"));
    printer.println(F("** HOA DON BAN LE **"));
    printer.println(F("----------------"));

    printer.println(F("KHOI LUONG:"));
    if (weight_gram > 1000) {
      printer.print(round(weight_kg * 100.0) / 100.0, 2);
      printer.println(F("kg"));
    } else {
      printer.print(round(weight_gram));
      printer.println(F("g"));
    }
    printer.print(F("DON GIA: "));
    printer.print(unit_price);
    printer.println(F(" VND/kg"));
    printer.print(F("TONG TIEN: "));
    printer.print(price);
    printer.println(F(" VND"));
    printer.println(F("----------------"));

    printer.println(F("CAM ON BAN DA MUA HANG!"));
    printer.feed(4);
    printer.sleep();

    invoice_number++;

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Da in hoa don!");
    delay(1000);
    lcd.clear();
    if (showPrice) {
      lcd.setCursor(4, 0);
      lcd.print("Gia tien");
      char displayStr[16];
      sprintf(displayStr, "%ld VND", price);
      int cursorPos = (16 - strlen(displayStr)) / 2;
      lcd.setCursor(cursorPos, 1);
      lcd.print(displayStr);
    } else {
      lcd.setCursor(3, 0);
      lcd.print("Khoi luong");
      char displayStr[16];
      if (weight_gram > 1000) {
        sprintf(displayStr, "%.2fkg", weight_kg);
      } else {
        sprintf(displayStr, "%dg", (int)round(weight_gram));
      }
      int cursorPos = (16 - strlen(displayStr)) / 2;
      lcd.setCursor(cursorPos, 1);
      lcd.print(displayStr);
    }
  }
}

// V3: Nhập đơn giá từ Text Input
BLYNK_WRITE(V3) {
  String input = param.asStr();
  int new_price = input.toInt();
  if (new_price >= 0 && new_price != unit_price) {
    unit_price = new_price;
    float rounded_weight_kg = round(weight_kg * 100.0) / 100.0;
    price = round(rounded_weight_kg * unit_price);
    last_price = -1; 
  }
}

// V4: Reset đơn giá về 0
BLYNK_WRITE(V4) {
  int btn = param.asInt();
  if (btn == 1) {
    unit_price = 0;
    price = 0;
    last_price = -1; 
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Da reset gia");
    delay(1000);
    lcd.clear();
    if (showPrice) {
      lcd.setCursor(4, 0);
      lcd.print("Gia tien");
      lcd.setCursor(5, 1);
      lcd.print("0 VND");
    } else {
      lcd.setCursor(3, 0);
      lcd.print("Khoi luong");
      char displayStr[16];
      if (weight_gram > 1000) {
        sprintf(displayStr, "%.2fkg", weight_kg);
      } else {
        sprintf(displayStr, "%dg", (int)round(weight_gram));
      }
      int cursorPos = (16 - strlen(displayStr)) / 2;
      lcd.setCursor(cursorPos, 1);
      lcd.print(displayStr);
    }
  }
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }

  scale.set_scale(calibration_factor);
  weight_kg = scale.get_units(5); 
  weight_gram = weight_kg * 1000.0; 

  if (weight_gram < 2.0) { 
    weight_kg = 0;
    price = 0;
  } else {
    float rounded_weight_kg = round(weight_kg * 100.0) / 100.0;
    price = round(rounded_weight_kg * unit_price);
  }

  if (weight_gram != last_weight_gram || price != last_price) {
    char displayStr[16];
    int cursorPos;
    lcd.setCursor(0, 1);
    lcd.print("                "); 
    if (showPrice) {
      sprintf(displayStr, "%ld VND", price);
      cursorPos = (16 - strlen(displayStr)) / 2; 
      lcd.setCursor(cursorPos, 1);
      lcd.print(displayStr);
    } else {
      if (weight_gram >= 2.0 && weight_gram > 1000) {
        sprintf(displayStr, "%.2fkg", weight_kg);
      } else {
        sprintf(displayStr, "%dg", (int)round(weight_gram));
      }
      cursorPos = (16 - strlen(displayStr)) / 2; 
      lcd.setCursor(cursorPos, 1);
      lcd.print(displayStr);
    }
    last_weight_gram = weight_gram;
    last_price = price;
  }
  delay(300);
}
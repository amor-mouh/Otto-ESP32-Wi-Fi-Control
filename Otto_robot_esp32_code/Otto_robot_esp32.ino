#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// إعدادات الـ Wi-Fi (Access Point)
const char* ssid     = "Robot_AP";
const char* password = "123456789";

WebServer server(80);

// تعريف دبابيس السيرفوات
const int PIN_LEG_L  = 18;
const int PIN_LEG_R  = 19;
const int PIN_FOOT_L = 22;
const int PIN_FOOT_R = 23;

// تعريف دبوس البازر وقناة LEDC
const int PIN_BUZZER  = 25;
const int BUZZER_CHAN = 4;

// تعريف دبابيس حساس الألتراسونيك
const int PIN_TRIG = 4;
const int PIN_ECHO = 16;

// كائنات السيرفو
Servo servoLegL;
Servo servoLegR;
Servo servoFootL;
Servo servoFootR;

// الاحتفاظ بآخر زاوية لكل سيرفو للتنعيم
int posLegL  = 90;
int posLegR  = 90;
int posFootL = 90;
int posFootR = 90;

// متغيرات التفاعل بالحساس
unsigned long lastSensorCheck = 0;
unsigned long lastHandTriggerTime = 0;
int currentAutoMode = 0; // 0: عادي/انتظار, 1: مشي, 2: رقص, 3: ابتسامة

// =================== نغمات وموسيقى البازر ===================
void playTone(int frequency, int durationMs) {
  if (frequency <= 0) {
    delay(durationMs);
    return;
  }
  ledcWriteTone(BUZZER_CHAN, frequency);
  delay(durationMs);
  ledcWriteTone(BUZZER_CHAN, 0);
}

void playStartMelody() {
  int melody[] = {262, 330, 392, 523, 659, 784};
  int duration[] = {100, 100, 100, 150, 150, 250};
  for (int i = 0; i < 6; i++) {
    playTone(melody[i], duration[i]);
    delay(20);
  }
}

void playSwitchSound() {
  playTone(587, 80); delay(30);
  playTone(880, 120); delay(30);
  playTone(1175, 150);
}

void playPartyMelody() {
  int notes[] = {523, 587, 659, 698, 784, 880, 988, 1047};
  for (int i = 0; i < 8; i++) {
    playTone(notes[i], 70);
    delay(10);
  }
}

// =================== تحريك السيرفوات بشكل ناعم ===================
void moveServosSmooth(int targetLegL, int targetLegR, int targetFootL, int targetFootR, int stepDelay) {
  int diffLegL  = abs(targetLegL - posLegL);
  int diffLegR  = abs(targetLegR - posLegR);
  int diffFootL = abs(targetFootL - posFootL);
  int diffFootR = abs(targetFootR - posFootR);

  int maxSteps = max(diffLegL, max(diffLegR, max(diffFootL, diffFootR)));

  for (int i = 0; i <= maxSteps; i++) {
    if (posLegL != targetLegL) {
      posLegL += (targetLegL > posLegL) ? 1 : -1;
      servoLegL.write(posLegL);
    }
    if (posLegR != targetLegR) {
      posLegR += (targetLegR > posLegR) ? 1 : -1;
      servoLegR.write(posLegR);
    }
    if (posFootL != targetFootL) {
      posFootL += (targetFootL > posFootL) ? 1 : -1;
      servoFootL.write(posFootL);
    }
    if (posFootR != targetFootR) {
      posFootR += (targetFootR > posFootR) ? 1 : -1;
      servoFootR.write(posFootR);
    }
    delay(stepDelay);
  }
}

void homePosition() {
  moveServosSmooth(90, 90, 90, 90, 4);
  delay(100);
}

// =================== حركات الروبوت الذكية ===================
void walkForward(int steps, int stepDelay) {
  for (int i = 0; i < steps; i++) {
    moveServosSmooth(posLegL, posLegR, 120, 120, stepDelay);
    moveServosSmooth(60, 60, posFootL, posFootR, stepDelay);
    moveServosSmooth(posLegL, posLegR, 60, 60, stepDelay);
    moveServosSmooth(120, 120, posFootL, posFootR, stepDelay);
  }
  homePosition();
}

void walkBackward(int steps, int stepDelay) {
  for (int i = 0; i < steps; i++) {
    moveServosSmooth(posLegL, posLegR, 60, 60, stepDelay);
    moveServosSmooth(60, 60, posFootL, posFootR, stepDelay);
    moveServosSmooth(posLegL, posLegR, 120, 120, stepDelay);
    moveServosSmooth(120, 120, posFootL, posFootR, stepDelay);
  }
  homePosition();
}

void turnRight(int steps, int stepDelay) {
  for (int i = 0; i < steps; i++) {
    moveServosSmooth(posLegL, posLegR, 125, 125, stepDelay);
    moveServosSmooth(130, 130, posFootL, posFootR, stepDelay);
    moveServosSmooth(posLegL, posLegR, 55, 55, stepDelay);
    moveServosSmooth(50, 50, posFootL, posFootR, stepDelay);
  }
  homePosition();
}

void turnLeft(int steps, int stepDelay) {
  for (int i = 0; i < steps; i++) {
    moveServosSmooth(posLegL, posLegR, 55, 55, stepDelay);
    moveServosSmooth(130, 130, posFootL, posFootR, stepDelay);
    moveServosSmooth(posLegL, posLegR, 125, 125, stepDelay);
    moveServosSmooth(50, 50, posFootL, posFootR, stepDelay);
  }
  homePosition();
}

void danceTilt(int repetitions, int stepDelay) {
  for (int i = 0; i < repetitions; i++) {
    moveServosSmooth(posLegL, posLegR, 120, 120, stepDelay);
    playTone(880, 80);
    moveServosSmooth(posLegL, posLegR, 60, 60, stepDelay);
    playTone(440, 80);
  }
  homePosition();
}

void danceSlide(int repetitions, int stepDelay) {
  for (int i = 0; i < repetitions; i++) {
    moveServosSmooth(60, 120, 120, 90, stepDelay);
    playTone(1047, 50);
    delay(40);
    moveServosSmooth(120, 60, 90, 60, stepDelay);
    playTone(523, 50);
    delay(40);
  }
  homePosition();
}

void shakeOneLeg(int repetitions, int stepDelay) {
  moveServosSmooth(90, 90, 130, 130, stepDelay);
  delay(100);
  for (int i = 0; i < repetitions; i++) {
    moveServosSmooth(60, 120, posFootL, posFootR, stepDelay);
    playTone(700, 60);
    moveServosSmooth(120, 60, posFootL, posFootR, stepDelay);
    playTone(900, 60);
  }
  homePosition();
}

void happySmile() {
  playPartyMelody();
  for (int i = 0; i < 3; i++) {
    moveServosSmooth(70, 110, 110, 70, 3);
    moveServosSmooth(110, 70, 70, 110, 3);
  }
  homePosition();
}

// =================== قراءة المسافة بالألتراسونيك ===================
float getDistanceCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // حساب طول النبضة
  long duration = pulseIn(PIN_ECHO, HIGH, 38000); 
  if (duration == 0) return 999.0; // خارج النطاق أو لا توجد استجابة
  return (duration * 0.0343) / 2.0;
}

// فحص المسافة وطباعتها في السيريال للتشخيص
void checkUltrasonicHand() {
  if (millis() - lastSensorCheck > 300) { // قراءة الحساس كل 300ms
    lastSensorCheck = millis();
    float dist = getDistanceCM();

    // طباعة القراءة في الـ Serial Monitor للتحقق
    Serial.print("[Ultrasonic Sensor] Distance: ");
    if (dist >= 999.0) {
      Serial.println("No echo / Out of range (999 cm)");
    } else {
      Serial.print(dist);
      Serial.println(" cm");
    }

    // التفاعل عند وضع اليد (أقل من 15 سم) بشرط مرور ثانيتين بين كل تفاعل
    if (dist > 1.5 && dist < 15.0 && (millis() - lastHandTriggerTime > 2000)) {
      lastHandTriggerTime = millis();
      currentAutoMode = (currentAutoMode + 1) % 4; // التنقل بين الأوضاع (0 -> 1 -> 2 -> 3 -> 0)
      
      Serial.print(">>> HAND DETECTED! Switching to Mode: ");
      Serial.println(currentAutoMode);

      playSwitchSound();

      if (currentAutoMode == 1) {
        Serial.println("Action: Walk Forward");
        walkForward(2, 4);
      } else if (currentAutoMode == 2) {
        Serial.println("Action: Dance Slide");
        danceSlide(3, 3);
      } else if (currentAutoMode == 3) {
        Serial.println("Action: Happy Smile");
        happySmile();
      } else {
        Serial.println("Action: Standby / Home");
        homePosition();
      }
    }
  }
}

// =================== واجهة الموقع HTML ===================
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>التحكم بالروبوت الذكي</title>
  <style>
    body { font-family: Arial, sans-serif; background-color: #1a1a2e; color: #ffffff; text-align: center; margin: 0; padding: 20px; }
    h1 { color: #00fff5; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; max-width: 420px; margin: auto; }
    .btn { background: #16213e; color: #00fff5; border: 2px solid #00fff5; padding: 14px 4px; font-size: 15px; border-radius: 12px; cursor: pointer; transition: 0.2s; font-weight: bold; }
    .btn:active { background: #00fff5; color: #16213e; transform: scale(0.95); }
    .btn-stop { border-color: #ff4757; color: #ff4757; grid-column: span 3; margin-top: 10px; font-size: 18px; }
    .btn-stop:active { background: #ff4757; color: #fff; }
    .empty { visibility: hidden; }
  </style>
</head>
<body>
  <h1>🤖 لوحة التحكم بالروبوت</h1>
  
  <div class="grid">
    <div class="empty"></div>
    <button class="btn" onclick="sendCmd('/forward')">⬆️ للأمام</button>
    <div class="empty"></div>

    <button class="btn" onclick="sendCmd('/left')">↩️ دوران يسار</button>
    <button class="btn" onclick="sendCmd('/backward')">⬇️ للخلف</button>
    <button class="btn" onclick="sendCmd('/right')">↪️ دوران يمين</button>

    <button class="btn" onclick="sendCmd('/dance')">💃 رقصة جانبية</button>
    <button class="btn" onclick="sendCmd('/slide')">🕺 رقصة السلايد</button>
    <button class="btn" onclick="sendCmd('/shake')">🦩 رجل واحدة</button>
    <button class="btn" onclick="sendCmd('/smile')">😊 ابتسامة</button>

    <button class="btn btn-stop" onclick="sendCmd('/home')">🛑 وضع مستمر / ثبات</button>
  </div>

  <script>
    function sendCmd(path) {
      fetch(path).catch(err => console.log(err));
    }
  </script>
</body>
</html>
)rawliteral";

// =================== مسارات السيرفر ===================
void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

void setupServerRoutes() {
  server.on("/", handleRoot);
  
  server.on("/forward", []() {
    server.send(200, "text/plain", "Forward");
    walkForward(3, 4);
  });

  server.on("/backward", []() {
    server.send(200, "text/plain", "Backward");
    walkBackward(3, 4);
  });

  server.on("/right", []() {
    server.send(200, "text/plain", "Turn Right");
    turnRight(3, 4);
  });

  server.on("/left", []() {
    server.send(200, "text/plain", "Turn Left");
    turnLeft(3, 4);
  });

  server.on("/dance", []() {
    server.send(200, "text/plain", "Dance Tilt");
    danceTilt(4, 3);
  });

  server.on("/slide", []() {
    server.send(200, "text/plain", "Dance Slide");
    danceSlide(4, 3);
  });

  server.on("/shake", []() {
    server.send(200, "text/plain", "Shake");
    shakeOneLeg(4, 3);
  });

  server.on("/smile", []() {
    server.send(200, "text/plain", "Smile");
    happySmile();
  });

  server.on("/home", []() {
    server.send(200, "text/plain", "Home");
    homePosition();
  });

  server.begin();
}

// =================== SETUP & LOOP ===================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== OTTO ROBOT INITIALIZING ===");

  // إعداد دبابيس الألتراسونيك
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  ledcSetup(BUZZER_CHAN, 2000, 8);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHAN);

  servoLegL.setPeriodHertz(50);
  servoLegR.setPeriodHertz(50);
  servoFootL.setPeriodHertz(50);
  servoFootR.setPeriodHertz(50);

  servoLegL.attach(PIN_LEG_L, 500, 2400);
  servoLegR.attach(PIN_LEG_R, 500, 2400);
  servoFootL.attach(PIN_FOOT_L, 500, 2400);
  servoFootR.attach(PIN_FOOT_R, 500, 2400);

  homePosition();

  WiFi.softAP(ssid, password);
  Serial.print("Access Point Started. IP: ");
  Serial.println(WiFi.softAPIP());

  setupServerRoutes();
  playStartMelody();
}

void loop() {
  server.handleClient();
  checkUltrasonicHand(); // فحص الحساس باستمرار وطباعة القيم
}
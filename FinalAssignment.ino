//西蒙游戏
#include <U8glib.h>
#define INTERVAL_LCD 20       //定义OLED刷新时间间隔  
unsigned long lcd_time = millis();  //OLED刷新时间计时器
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  //设置OLED型号  
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_SS u8g.setFont(u8g_font_fub25n)

//无法下载pitches库，直接从GitHub复制了代码过来
/*************************************************
   Public Constants
 *************************************************/
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

const byte ledPins[] = {9, 10, 11, 12};
const byte buttonPins[] = {2, 3, 4, 5};
#define SPEAKER_PIN 8//扬声器端口

#define MAX_GAME_LENGTH 100//长度

const int gameTones[] = { NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5};//音调

/*全局变量-存储游戏状态*/
byte gameSequence[MAX_GAME_LENGTH] = {0};
byte gameIndex = 0;//游戏索引

void setup() {
  Serial.begin(9600);
  for (byte i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(SPEAKER_PIN, OUTPUT);
    //启动随机数生成器并赋值。
//假设引脚A0是浮动的(断开连接):
  randomSeed(analogRead(A0));
}


//点亮给定的LED，并播放合适的音调
void lightLedAndPlayTone(byte ledIndex) {
  digitalWrite(ledPins[ledIndex], HIGH);
  tone(SPEAKER_PIN, gameTones[ledIndex]);//参数为扬声器和音调
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
  noTone(SPEAKER_PIN);
}


// 播放用户必须重复的当前音符序列
void playSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

/**
    直到用户按下其中一个按钮，
	并返回那个按钮的索引
*/
byte readButtons() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        return i;
      }
    }
    delay(1);
  }
}

//按顺序玩游戏，并报告游戏分数
void gameOver() {
  Serial.print("Game over!");
  Serial.print(" your score:");  
  Serial.println(gameIndex - 1);
  //OLED屏幕上显示
   u8g.firstPage();  
    do {    
        setFont_L;    
        u8g.setPrintPos(0,10);
        u8g.print("Game over!");    
        u8g.setPrintPos(0,20);
        u8g.print("your score:");
        u8g.setPrintPos(0,35);
        u8g.print(gameIndex - 1); 
    }while (u8g.nextPage());

  gameIndex = 0;
  delay(200);

  // 鼓舞的声音
  tone(SPEAKER_PIN, NOTE_DS5);
  delay(300);
  tone(SPEAKER_PIN, NOTE_D5);
  delay(300);
  tone(SPEAKER_PIN, NOTE_CS5);
  delay(300);
  for (byte i = 0; i < 10; i++) {
    for (int pitch = -10; pitch <= 10; pitch++) {
      tone(SPEAKER_PIN, NOTE_C5 + pitch);
      delay(5);
    }
  }
  noTone(SPEAKER_PIN);
  delay(500);
}

/**
   获取用户的输入，并将其与预期序列进行比较。
*/
bool checkUserSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
  }

  return true;
}

//玩家完成一个关卡时播放欢呼的声音

void playLevelUpSound() {
  tone(SPEAKER_PIN, NOTE_E4);
  delay(150);
  tone(SPEAKER_PIN, NOTE_G4);
  delay(150);
  tone(SPEAKER_PIN, NOTE_E5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_C5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_D5);
  delay(150);
  tone(SPEAKER_PIN, NOTE_G5);
  delay(150);
  noTone(SPEAKER_PIN);
}

//主函数
void loop() {
  //在序列的末尾添加一个随机的颜色  
  gameSequence[gameIndex] = random(0, 4);
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH) {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  playSequence();
  if (!checkUserSequence()) {
    gameOver();
  }

  delay(300);

  if (gameIndex > 0) {
    playLevelUpSound();
    delay(300);
  }
}


#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp> 

// Wio Terminalの設定を自動で読み込む
static LGFX lcd;
static LGFX_Sprite sprite(&lcd); 

// タイマー変数の設定
long setMinutes = 5;           // 初期設定時間（分）
long remainingSeconds = 0;     // 残り時間（秒）
unsigned long lastMillis = 0;  // ミリ秒カウント用
bool isRunning = false;        // タイマー動作フラグ
bool isFinished = false;       // 終了フラグ

// 色の定義
#define COLOR_TIMER 0xE8E4 // 赤
#define COLOR_BG    0xFFFF // 白

void setup() {
  // ハードウェア初期化
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);

  // ピン設定
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP); 
  
  // ★追加：ブザーのピン設定
  pinMode(WIO_BUZZER, OUTPUT);

  // スプライト作成
  sprite.createSprite(lcd.width(), lcd.height());
  
  // 初期時間のセット
  remainingSeconds = setMinutes * 60;
}

// ★追加：音を鳴らす関数
void playTimeUpSound() {
  // "ピ・ピ・ピ" と3回鳴らす
  for (int i = 0; i < 3; i++) {
    analogWrite(WIO_BUZZER, 128); // 音ON (PWM 50%)
    delay(100);
    analogWrite(WIO_BUZZER, 0);   // 音OFF
    delay(100);
  }
}

void loop() {
  // --- 入力処理 ---
  if (digitalRead(WIO_KEY_C) == LOW) {
    delay(200); 
    if (isFinished) {
      isFinished = false;
      remainingSeconds = setMinutes * 60;
      drawTimer(); // リセット直後に描画更新
    } else {
      isRunning = !isRunning;
    }
  }

  // 時間設定
  if (!isRunning && !isFinished) {
    bool changed = false;
    if (digitalRead(WIO_5S_UP) == LOW) {
      setMinutes++;
      if (setMinutes > 60) setMinutes = 60;
      changed = true;
      delay(150);
    }
    if (digitalRead(WIO_5S_DOWN) == LOW) {
      setMinutes--;
      if (setMinutes < 1) setMinutes = 1;
      changed = true;
      delay(150);
    }
    if (changed) {
        remainingSeconds = setMinutes * 60;
        drawTimer(); // 設定変更時にすぐ描画更新
    }
  }

  // --- タイマー計算処理 ---
  if (isRunning && remainingSeconds > 0) {
    if (millis() - lastMillis >= 1000) {
      remainingSeconds--;
      lastMillis = millis();
      
      // タイムアップ判定
      if (remainingSeconds <= 0) {
        remainingSeconds = 0;
        isRunning = false;
        
        // ★修正：まだ終了フラグが立っていない（今終わった瞬間）なら音を鳴らす
        if (!isFinished) {
            isFinished = true;
            drawTimer();     // 画面をTIME UPにする
            playTimeUpSound(); // ★音を鳴らす
        }
      } else {
        // 通常カウントダウン時は描画更新
        drawTimer();
      }
    }
  } else {
    lastMillis = millis();
    // 停止中も描画更新（PAUSE表示などをちらつかせないため、頻度を落とすなどの工夫も可）
    if(!isRunning) drawTimer();
  }
}

void drawTimer() {
  sprite.fillScreen(COLOR_BG);

  int cx = lcd.width() / 2;
  int cy = lcd.height() / 2;
  int radius = 100;

  // 外枠
  sprite.fillCircle(cx, cy, radius, 0xDDDD); 
  sprite.fillCircle(cx, cy, radius - 5, COLOR_BG);

  // 扇形の計算
  long totalSeconds = setMinutes * 60;
  if (totalSeconds == 0) totalSeconds = 1;
  float ratio = (float)remainingSeconds / (float)totalSeconds;
  
  if (remainingSeconds > 0) {
    float angle = 360.0 * ratio;
    sprite.fillArc(cx, cy, radius - 5, 0, -90, -90 + angle, COLOR_TIMER);
  }

  // 文字表示
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(middle_center);
  sprite.setTextSize(3); 
  
  int min = remainingSeconds / 60;
  int sec = remainingSeconds % 60;
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", min, sec);
  sprite.drawString(timeStr, cx, cy);

  sprite.setTextSize(2);
  if (isFinished) {
    sprite.setTextColor(TFT_RED);
    sprite.drawString("TIME UP!", cx, cy + 50);
  } else if (!isRunning) {
    sprite.setTextColor(TFT_BLUE);
    sprite.drawString("PAUSE", cx, cy + 50);
  }

  sprite.pushSprite(0, 0);
}
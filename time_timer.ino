// --- ここから修正済みコード ---
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp> // ★重要：これを追加すると Wio Terminal 用に自動設定されます

// Wio Terminalの設定を自動で読み込む
static LGFX lcd;
static LGFX_Sprite sprite(&lcd); // 画面チラつき防止のためのスプライト

// タイマー変数の設定
long setMinutes = 5;           // 初期設定時間（分）
long remainingSeconds = 0;     // 残り時間（秒）
unsigned long lastMillis = 0;  // ミリ秒カウント用
bool isRunning = false;        // タイマー動作フラグ
bool isFinished = false;       // 終了フラグ

// 色の定義 (Time Timer風の赤)
#define COLOR_TIMER 0xE8E4 // 赤っぽい色 (RGB565)
#define COLOR_BG    0xFFFF // 白

void setup() {
  // ハードウェア初期化
  lcd.init();
  lcd.setRotation(1); // 横向き
  lcd.setBrightness(128);

  // 5方向スイッチとボタンのピン設定
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP); // 右側のボタン（スタート/ストップ）

  // スプライト（描画バッファ）の作成
  sprite.createSprite(lcd.width(), lcd.height());
  
  // 初期時間のセット
  remainingSeconds = setMinutes * 60;
}

void loop() {
  // --- 入力処理 ---
  
  // スタート・ストップボタン (Cボタン)
  if (digitalRead(WIO_KEY_C) == LOW) {
    delay(200); // チャタリング防止
    if (isFinished) {
      // 終了していたらリセット
      isFinished = false;
      remainingSeconds = setMinutes * 60;
    } else {
      // 動作切替
      isRunning = !isRunning;
    }
  }

  // 時間設定 (動作中でない時のみ変更可能)
  if (!isRunning && !isFinished) {
    if (digitalRead(WIO_5S_UP) == LOW) {
      setMinutes++;
      if (setMinutes > 60) setMinutes = 60; // 最大60分
      remainingSeconds = setMinutes * 60;
      delay(150);
    }
    if (digitalRead(WIO_5S_DOWN) == LOW) {
      setMinutes--;
      if (setMinutes < 1) setMinutes = 1;   // 最小1分
      remainingSeconds = setMinutes * 60;
      delay(150);
    }
  }

  // --- タイマー計算処理 ---
  if (isRunning && remainingSeconds > 0) {
    // 1秒ごとの処理
    if (millis() - lastMillis >= 1000) {
      remainingSeconds--;
      lastMillis = millis();
      
      // タイムアップ判定
      if (remainingSeconds <= 0) {
        remainingSeconds = 0;
        isRunning = false;
        isFinished = true;
        // ここでブザーを鳴らすなどの処理を追加可能
      }
    }
  } else {
    // 動作していないときは基準時間を更新し続ける
    lastMillis = millis();
  }

  // --- 描画処理 ---
  drawTimer();
}

void drawTimer() {
  // 背景クリア
  sprite.fillScreen(COLOR_BG);

  // 中心座標と半径
  int cx = lcd.width() / 2;
  int cy = lcd.height() / 2;
  int radius = 100;

  // --- 円形ゲージの描画 ---
  // 全体（薄いグレーの枠）
  sprite.fillCircle(cx, cy, radius, 0xDDDD); 
  sprite.fillCircle(cx, cy, radius - 5, COLOR_BG);

  // 残り時間の角度計算 (360度 * 残り / 全体)
  
  long totalSeconds = setMinutes * 60;
  // ゼロ除算防止
  if (totalSeconds == 0) totalSeconds = 1;
  
  float ratio = (float)remainingSeconds / (float)totalSeconds;
  
  // Time Timer風：残り時間の分だけ扇形を描く
  // start_angle, end_angle
  // -90度が12時位置。
  if (remainingSeconds > 0) {
    // 赤い扇形を描画 (中心x, 中心y, 半径r1, 半径r2, 開始角度, 終了角度, 色)
    // 時計回りに減らしていく表現
    float angle = 360.0 * ratio;
    sprite.fillArc(cx, cy, radius - 5, 0, -90, -90 + angle, COLOR_TIMER);
  }

  // --- テキスト表示 ---
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(middle_center);
  
  // 中央に残り時間をデジタル表示
  // フォントサイズは適宜調整してください。とりあえずデフォルトフォントを拡大します
  // sprite.setFont(&fonts::FreeSansBold24pt7b); // フォントがない場合エラーになることがあるので一旦コメントアウト
  sprite.setTextSize(3); // 標準フォントを3倍拡大
  
  int min = remainingSeconds / 60;
  int sec = remainingSeconds % 60;
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", min, sec);
  sprite.drawString(timeStr, cx, cy);

  // 状態表示
  sprite.setTextSize(2);
  if (isFinished) {
    sprite.setTextColor(TFT_RED);
    sprite.drawString("TIME UP!", cx, cy + 50);
  } else if (!isRunning) {
    sprite.setTextColor(TFT_BLUE);
    sprite.drawString("PAUSE", cx, cy + 50);
  }

  // 画面に転送
  sprite.pushSprite(0, 0);
}
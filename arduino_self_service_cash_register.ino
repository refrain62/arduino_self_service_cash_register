
/*
 * Arduino での 雑誌 幼稚園の付録のセルフサービス レジスターの真似事
 * 
 * Arduiono IDE で ツール -> ライブラリ管理にてMsTimer2のライブラリを追加すること！
 */

#include <MsTimer2.h>

// ※10kΩでの設定値
const int C_P_SEN_STANDARD_VALUE = 650;     // 光センサーの基準値

const int C_PIN_SPEAKER = 8;                // D8ピンに圧電スピーカー
const int C_PIN_PHOTORESISTOR = A0;         // A0ピンに光センター

const int C_BEEP_SOUND_HZ = 659;            // 659Hz ミの音
const int C_BEEP_SOUND_SEC = 100;           // ビープ音の長さ(ms)

const int C_DELAY_READ_INTERVAL = 500;      // 光センサー読み込みのインターバル(ms)
const int C_DELAY_READ_END = 1000;          // レジ読み込み後の遅延(ms)

int iLoopCnt = 0;                           // ループカウンタ
int pSenseorVal = 0;                        // 光センサーの値
boolean bRegistorReadingFlg = false;        // レジ読み込み中フラグ 
int iDisplayNumber = 123;                   // ディスプレイ表示用の数値

// ------------------------------
// LED表示関連
// ------------------------------
const int C_LED_DELAY = 1;          // LEDの表示サイクル(ms)
const int C_LED_OFF_IDX = 11;       // LED OFF のインデックス

int srclkPin = 10;   // (10)  SH_CP [SRCLK] on 74HC595
int rclkPin = 11;   // (11) ST_CP [RCLK] on 74HC595
int dsPin = 12;     // (12) DS [SER] on 74HC595

int dig1 = 2;       // DIG1を1番ピンに割り当て
int dig2 = 3;       // DIG2を2番ピンに割り当て
int dig3 = 4;       // DIG3を3番ピンに割り当て
int dig4 = 5;       // DIG4を4番ピンに割り当て

// seven_ledsをbyte型として定義
// 配列にDisplay表示用のデータ0~9と全消灯を設定
byte seven_leds[12] =       { B11111100,  // 0
                              B01100000,  // 1
                              B11011010,  // 2
                              B11110010,  // 3
                              B01100110,  // 4
                              B10110110,  // 5
                              B10111110,  // 6
                              B11100000,  // 7
                              B11111110,  // 8
                              B11100110,  // 9
                              B00000001,  // D.P 
                              B00000000,  // OFF
                             };

//シフトレジスタ部分を関数化
void funcShiftReg( int argIdx )
{
  //送信中のRCLKをLowにする
  digitalWrite(rclkPin, LOW);
  
  //シフト演算を使って点灯するLEDを選択 
  shiftOut(dsPin, srclkPin, LSBFIRST, seven_leds[ argIdx ]); 

  //送信終了後RCLKをHighにする
  digitalWrite(rclkPin, HIGH);
}

// ------------------------------
// レジスター読み込み動作
// ------------------------------
void registorReading( int argVal )
{
  int amountLen = 12;                                  // 金額の配列数
  int amounts[] = { 198, 298, 330, 110, 220, 98, 50, 1980, 2980, 3300, 1100, 2200 };    // 金額の配列

  // LED金額を変更（ランダム）
  iDisplayNumber = amounts[ random(1, 12) ];
  
  // レジ読み込み動作中に変更
  bRegistorReadingFlg = true;

  // 商品（バーコード）読み込み音を鳴らす
  tone( C_PIN_SPEAKER, C_BEEP_SOUND_HZ );
  delay( C_BEEP_SOUND_SEC );
  noTone( C_PIN_SPEAKER );
  
  Serial.println( (iDisplayNumber / 1000) % 10 );
  Serial.println( (iDisplayNumber / 100) % 10 );
  Serial.println( (iDisplayNumber / 10) % 10 );
  Serial.println( (iDisplayNumber / 1) % 10 );

  // レジ動作後のディレイ
  delay( C_DELAY_READ_END ); 

  // レジ読み込み動作停止に変更
  bRegistorReadingFlg = false;
}

// ------------------------------
// LED表示をOFF
// ------------------------------
void clearLedAmount()
{
  digitalWrite(dig1, HIGH);    //1番ピンをHIGH DI1 OFF
  digitalWrite(dig2, HIGH);    //2番ピンをHIGH DI2 OFF
  digitalWrite(dig3, HIGH);    //3番ピンをHIGH DI3 OFF
  digitalWrite(dig4, HIGH);    //4番ピンをHIGH DI4 OFF

  // LED OFF 消灯
  funcShiftReg( C_LED_OFF_IDX );
}

// ------------------------------
// 金額をLEDに表示
// ------------------------------
void displayLedAmount()
{
  int dispNum1 = (iDisplayNumber / 1000) % 10;
  int dispNum2 = (iDisplayNumber / 100) % 10;
  int dispNum3 = (iDisplayNumber / 10) % 10;
  int dispNum4 = (iDisplayNumber / 1) % 10;
  
  // LED表示をOFF
  clearLedAmount();

  // バーコード読込処理時のみ数値を表示
  if(   bRegistorReadingFlg == true
      )
  {
    // ----------
    // LED1表示
    // ----------
    digitalWrite(dig1, LOW); 
    digitalWrite(dig2, HIGH);
    digitalWrite(dig3, HIGH);
    digitalWrite(dig4, HIGH);

    if( dispNum1 != 0 )
    {
      funcShiftReg( dispNum1 );
    }
    
    delay( 1 );
    funcShiftReg( C_LED_OFF_IDX );
    
    // ----------
    // LED2表示
    // ----------
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2, LOW);
    digitalWrite(dig3, HIGH);
    digitalWrite(dig4, HIGH);
  
    if( dispNum2 != 0 )
    {
      funcShiftReg( dispNum2 );
    }
    
    delay( 1 );
    funcShiftReg( C_LED_OFF_IDX );
    
    // ----------
    // LED3表示
    // ----------
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2, HIGH);
    digitalWrite(dig3, LOW);
    digitalWrite(dig4, HIGH);
    
    funcShiftReg( dispNum3 );
    delay( 1 );
    funcShiftReg( C_LED_OFF_IDX );
    
    // ----------
    // LED4表示
    // ----------
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2, HIGH);
    digitalWrite(dig3, HIGH);
    digitalWrite(dig4, LOW);
    
    funcShiftReg( dispNum4 );
    delay( 1 );
    funcShiftReg( C_LED_OFF_IDX );
  }
}

// ------------------------------
// 起動時の処理（起動時に1度だけ実行されます）
// ------------------------------
void setup()
{
  // ----------
  // 通信開始
  // ----------
  Serial.begin( 9600 );

  // ----------
  // LEDの初期化
  // ----------
  pinMode(rclkPin, OUTPUT);   // 11番ピンをOUTPUTとして定義
  pinMode(dsPin, OUTPUT);     // 12番ピンをOUTPUTとして定義
  pinMode(srclkPin, OUTPUT);  // 9番ピンをOUTPUTとして定義
  
  pinMode(dig1, OUTPUT);      // 1番ピンをOUTPUTとして定義
  pinMode(dig2, OUTPUT);      // 2番ピンをOUTPUTとして定義
  pinMode(dig3, OUTPUT);      // 3番ピンをOUTPUTとして定義
  pinMode(dig4, OUTPUT);      // 4番ピンをOUTPUTとして定義

  // LED表示をOFF
  clearLedAmount();

  // ----------
  // LED表示用の割り込み処理
  // ----------
  MsTimer2::set(5, displayLedAmount); // displayLedAmount()を1ミリ秒ごとに呼び出す
  MsTimer2::start(); // タイマを開始する
}

// ------------------------------
// ループ処理（起動後にループ）
// ------------------------------
void loop()
{
//  // 読み込みのインターバル時のみ処理
//  if(   (iLoopCnt % C_DELAY_READ_INTERVAL) == 0
//    )
//  {
    // 光センサーの値を読み込む
    pSenseorVal = analogRead( C_PIN_PHOTORESISTOR );
    
    // 読み込んだ値をログに出力
    Serial.println( pSenseorVal );
    
    // 読み込み開始か判断
    if(   bRegistorReadingFlg == false
      &&  C_P_SEN_STANDARD_VALUE <= pSenseorVal
      )
    {
      // レジ読み込み処理実行
      registorReading( pSenseorVal );
    }
//  }
  
//  // ループカウンタのインクリメント
//  iLoopCnt = iLoopCnt + 1;

  // 読込処理のインターバル遅延
  delay( C_DELAY_READ_INTERVAL );
}


/*
 * Arduino での 雑誌 幼稚園の付録のセルフサービス レジスターの真似事
 */

// ※10kΩでの設定値
const int C_P_SEN_STANDARD_VALUE = 600;     // 光センサーの基準値

const int C_PIN_SPEAKER = 8;                // D8ピンに圧電スピーカー
const int C_PIN_PHOTORESISTOR = A0;         // A0ピンに光センター

const int C_BEEP_SOUND_HZ = 659;            // 659Hz ミの音
const int C_BEEP_SOUND_SEC = 100;           // ビープ音の長さ(ms)

const int C_DELAY_READ_INTERVAL = 500;      // 光センサー読み込みのインターバル(ms)
const int C_DELAY_READ_END = 1000;          // レジ読み込み後の遅延(ms)

int pSenseorVal = 0;                        // 光センサーの値
boolean bRegistorReadingFlg = false;        // レジ読み込み中フラグ 

// ------------------------------
// 起動時の処理（起動時に1度だけ実行されます）
// ------------------------------
void setup()
{
  // 通信開始
  Serial.begin( 9600 );
}

// ------------------------------
// ループ処理（起動後にループ）
// ------------------------------
void loop()
{
  // 光センサーの値を読み込む
  pSenseorVal = analogRead( C_PIN_PHOTORESISTOR );
  
  // 読み込んだ値をログに出力
  Serial.println( pSenseorVal );
  
  // 読み込み開始か判断
  if(   bRegistorReadingFlg == false
    &&  C_P_SEN_STANDARD_VALUE <= pSenseorVal
    )
  {
    // レジ読み込み動作中に変更
    bRegistorReadingFlg = true;

    // レジ読み込み処理実行
    registorReading( pSenseorVal );
  } 
  
  // 読み込みインターバルのディレイ
  delay( C_DELAY_READ_INTERVAL ); 
}

// ------------------------------
// レジスター読み込み動作
// ------------------------------
void registorReading( int argVal )
{
    // 商品（バーコード）読み込み音を鳴らす
    tone( C_PIN_SPEAKER, C_BEEP_SOUND_HZ );
    delay( C_BEEP_SOUND_SEC );
    noTone( C_PIN_SPEAKER );
  
    // レジ動作後のディレイ
    delay( C_DELAY_READ_END ); 
    
    // レジ読み込み動作停止に変更
    bRegistorReadingFlg = false;
}

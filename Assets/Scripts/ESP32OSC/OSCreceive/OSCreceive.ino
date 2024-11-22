#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi接続のためのSSIDとパスワード
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 書き換え必須！！！！！
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char* ssid = "SSID";
const char* password = "PASSWORD";

const int localPort = 8000;  // 受信するポート番号

const int ledPin = 2; // LEDのピン番号

WiFiUDP udp;

union OscMessage {
  int intValue;
  float floatValue;
  char stringValue[256];
};

void setup() {
  Serial.begin(115200);

  // WiFi接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // UDPの初期化
  Serial.println(WiFi.localIP());
  udp.begin(localPort);
  Serial.printf("Listening on UDP port %d\n", localPort);

  // LEDを出力モードに
  pinMode(ledPin, OUTPUT);
  
  Serial.println("setup finished");
}

void loop() {
  // 受信バッファを設定
  char incomingPacket[255];
  
  // UDPでOSCメッセージを受信
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';  // パケットの末尾をNULLで終了
    }
    
    // OSCメッセージを解析
    OscMessage message = parseOscMessage(incomingPacket, len);
    
    // 受信したらLEDを点灯
    ledOn();
  }
}

// LEDを点灯させる
void ledOn() {
  digitalWrite(ledPin, HIGH);
  delay(250);
  digitalWrite(ledPin, LOW);
  delay(250);
} 

// 4バイト境界に揃えるためのパディング計算
int padSize(int size) {
  return (size + 3) & ~3;  // 4バイト境界に揃える
}

// 受信メッセージを解析してメッセージを返す
OscMessage parseOscMessage(char* packet, int packetSize) {
  // アドレス部分の抽出
  String address = String(packet);
  Serial.println("Received OSC address: " + address);

  // アドレス部分の終わりを見つける（4バイト境界に揃える）
  int addressEnd = padSize(address.length() + 1);

  // 型タグの位置を見つける
  int typeTagPos = addressEnd;

  OscMessage message;
  
  // 型タグが存在し、"i"（整数）であることを確認
  if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 'i') {
    // 値の位置を取得して整数として読み取る（型タグの4バイト後が値の開始位置）
    int valuePos = padSize(typeTagPos + 2);
    memcpy(&message.intValue, packet + valuePos, sizeof(message.intValue));
    message.intValue = ntohl(message.intValue);  // ネットワークバイトオーダーをホストバイトオーダーに変換
    Serial.println("Received int value: " + String(message.intValue));
  } 
  // 型タグが存在し、"f"（浮動小数点）であることを確認
  else if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 'f') {
      // 値の位置を取得して浮動小数点数として読み取る（型タグの4バイト後が値の開始位置）
      int valuePos = padSize(typeTagPos + 2);

      // ネットワークバイトオーダーからホストバイトオーダーに変換
      uint32_t networkOrderValue;
      memcpy(&networkOrderValue, packet + valuePos, sizeof(networkOrderValue));
      networkOrderValue = ntohl(networkOrderValue);  // ネットワークオーダーをホストオーダーに変換

      // 変換した値を浮動小数点型にキャスト
      float hostOrderFloatValue;
      memcpy(&hostOrderFloatValue, &networkOrderValue, sizeof(hostOrderFloatValue));

      message.floatValue = hostOrderFloatValue;
      Serial.println("Received float value: " + String(message.floatValue));
  }
  // 型タグが存在し、"s"（文字列）であることを確認
  else if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 's') {
    // 値の位置を取得して文字列として読み取る（型タグの4バイト後が値の開始位置）
    int valuePos = padSize(typeTagPos + 2);
    strcpy(message.stringValue, (char*)(packet + valuePos));
    Serial.println("Received string value: " + String(message.stringValue));
  } 
  else {
    Serial.println("Unexpected or missing type tag");
  }

  return message;
}
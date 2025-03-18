#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// 传感器引脚配置
#define IR_PIN 5         // 红外传感器连接的GPIO引脚

// WiFi credentials
const char *ssid = "你的2.4Gwifi";             
const char *password = "mima";   

// MQTT Broker settings
const int mqtt_port = 8883;
const char *mqtt_broker = "hangzhou.emqxsl.cn";
const char *gas_topic = "GasNum";          // 新增气体检测主题
const char *mqtt_topic = "emqx/esp8266";     // 保留原有主题
const char *mqtt_username = "espny";  
const char *mqtt_password = "123456";
// 新增全局变量
volatile int triggerCount = 0; // 红外触发计数器
String countTopic = "homeassistant/sensor/ir_counter/state"; // 计数专用主题  

// NTP Server settings
//const char *ntp_server = "pool.ntp.org"; 
const char* ntp_server = "cn.pool.ntp.org"; // Recommended NTP server for users in China    
const long gmt_offset_sec = 0;            
const int daylight_offset_sec = 0;        

// 全局变量
volatile bool gasDetected = false;        // 中断标志位
volatile unsigned long lastTriggerTime = 0; // 记录最后一次有效触发时间
volatile unsigned long lastDebounceTime = 0; // 防抖时间记录

BearSSL::WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

// 证书配置保持不变...
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

// Function declarations
void connectToWiFi();

void connectToMQTT();

void syncTime();

void mqttCallback(char *topic, byte *payload, unsigned int length);


// 中断服务函数
volatile bool isProcessing = false; // 新增状态锁变量
ICACHE_RAM_ATTR void sensorISR() {
  unsigned long currentMillis = millis();
    // 200ms防抖逻辑
  if (currentMillis - lastDebounceTime < 200) {
    return;
  }
  lastDebounceTime = currentMillis;

  // 5秒内不重复触发
  if (!isProcessing && (currentMillis - lastTriggerTime >= 5000)) {
    Serial.printf("[MQTT] Sent count=%d at %lu\n", triggerCount, currentMillis);
    isProcessing = true;    // 锁定状态
    triggerCount++;         // 计数器递增
    gasDetected = true;
    lastTriggerTime = currentMillis; // 更新最后触发时间
  }
  
}

void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT_PULLUP);          // 启用内部上拉电阻
  attachInterrupt(digitalPinToInterrupt(IR_PIN), sensorISR, FALLING); // 配置下降沿中断
  
  connectToWiFi();
  syncTime();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  connectToMQTT();
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTT();
  }
  mqtt_client.loop();

  if (gasDetected) {
    String message = "{\"count\":" + String(triggerCount) + 
                    ", \"timestamp\":" + String(time(nullptr)) + "}";
    mqtt_client.publish(countTopic.c_str(), message.c_str());
    Serial.println("send message+++++");
    
    gasDetected = false;
    isProcessing = false; // 解锁
  }
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void syncTime() {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
    Serial.print("Waiting for NTP time sync: ");
    while (time(nullptr) < 8 * 3600 * 2) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Time synchronized");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Current time: ");
        Serial.println(asctime(&timeinfo));
    } else {
        Serial.println("Failed to obtain local time");
    }
}
// 以下原有函数保持不变，只需添加gas_topic订阅...
void connectToMQTT() {
    BearSSL::X509List serverTrustedCA(ca_cert);
    espClient.setTrustAnchors(&serverTrustedCA);
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
        } else {
            char err_buf[128];
            espClient.getLastSSLError(err_buf, sizeof(err_buf));
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.println(mqtt_client.state());
            Serial.print("SSL error: ");
            Serial.println(err_buf);
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.print(topic);
    Serial.print("]: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
}
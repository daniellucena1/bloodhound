const char* ssid = "brisa-2042583";
const char* password = "dkcghxqs";

// Telegram
#define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
#define CHAT_ID "6310061051"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
  client.setInsecure();
}

void escanearRede() {
  IPAddress localIP = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress baseIP = localIP & subnet;

  Serial.println("Escaneando rede...");

  String mensagem = "📡 *Dispositivos ativos na rede:*\n\n";
  bool algumAtivo = false;

  for (int i = 1; i < 255; i++) {
    IPAddress ip = baseIP;
    ip[3] = i;

    Serial.print("Ping para ");
    Serial.print(ip.toString());
    Serial.print(" ... ");

    if (Ping.ping(ip, 1)) {
      Serial.println("OK");
      mensagem += "🔹 " + ip.toString() + "\n";
      algumAtivo = true;
    } else {
      Serial.println("Sem resposta");
    }
  }

  if (!algumAtivo) {
    mensagem += "⚠️ Nenhum dispositivo ativo foi detectado.";
  }

  Serial.println("Enviando mensagem para Telegram...");
  bot.sendMessage(CHAT_ID, mensagem, "Markdown");
  Serial.println("Mensagem enviada!");
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  bot.sendMessage(CHAT_ID, "🚀 ESP32 iniciado e conectado!", "");
  delay(1000);
  escanearRede();
}

void loop() {
  static unsigned long ultimoScan = 0;
  if (millis() - ultimoScan > 5 * 60 * 1000) { // a cada 5 minutos
    escanearRede();
    ultimoScan = millis();
  }
}

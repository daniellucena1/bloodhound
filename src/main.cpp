#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

// Wi-Fi
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
// Telegram
#define BOT_TOKEN "SEU_TOKEN"
#define CHAT_ID "SEU_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

String ultimoResultado = "";
unsigned long tempoUltimoScan = 0;
unsigned long tempoUltimaMensagem = 0;
unsigned long tempoInicio = 0;

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

String escanearRede() {
  IPAddress localIP = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress baseIP = localIP & subnet;

  Serial.println("Escaneando rede...");
  String mensagem = "📡 *Dispositivos ativos na rede:*\n\n";
  bool algumAtivo = false;

  for (int i = 1; i < 255; i++) {
    IPAddress ip = baseIP;
    ip[3] = i;

    if (Ping.ping(ip, 1)) {
      mensagem += "🔹 " + ip.toString() + "\n";
      algumAtivo = true;
    }
  }

  if (!algumAtivo) {
    mensagem += "⚠️ Nenhum dispositivo ativo foi detectado.";
  }

  ultimoResultado = mensagem;
  return mensagem;
}

String getUptime() {
  unsigned long segundos = (millis() / 1000);
  unsigned long minutos = segundos / 60;
  unsigned long horas = minutos / 60;
  minutos = minutos % 60;
  segundos = segundos % 60;

  char buffer[50];
  sprintf(buffer, "%luh %lum %lus", horas, minutos, segundos);
  return String(buffer);
}

void handleComando(String comando) {
  if (comando == "/scan") {
    String resultado = escanearRede();
    bot.sendMessage(CHAT_ID, resultado, "Markdown");
  } else if (comando == "/last") {
    if (ultimoResultado != "") {
      bot.sendMessage(CHAT_ID, "📄 Último resultado salvo:\n\n" + ultimoResultado, "Markdown");
    } else {
      bot.sendMessage(CHAT_ID, "⚠️ Nenhum escaneamento salvo ainda.", "");
    }
  } else if (comando == "/status") {
    String msg = "📶 *Status do ESP32:*\n";
    msg += "IP: " + WiFi.localIP().toString() + "\n";
    msg += "RSSI: " + String(WiFi.RSSI()) + " dBm\n";
    msg += "Modo: " + String(WiFi.getMode() == WIFI_STA ? "Estação (STA)" : "AP") + "\n";
    msg += "Uptime: " + getUptime();
    bot.sendMessage(CHAT_ID, msg, "Markdown");
  } else if (comando == "/wifi") {
    int redes = WiFi.scanNetworks();
    String msg = "📡 *Redes Wi-Fi próximas:*\n\n";
    for (int i = 0; i < redes; i++) {
      msg += String(i + 1) + ". " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm)\n";
    }
    bot.sendMessage(CHAT_ID, msg, "Markdown");
  } else if (comando == "/uptime") {
    bot.sendMessage(CHAT_ID, "⏱️ Uptime: " + getUptime(), "");
  } else {
    bot.sendMessage(CHAT_ID, "❓ Comando não reconhecido. Use:\n/scan\n/last\n/status\n/wifi\n/uptime", "");
  }
}

void checarComandosTelegram() {
  int numNovasMensagens = bot.getUpdates(bot.last_message_received + 1);
  while (numNovasMensagens) {
    for (int i = 0; i < numNovasMensagens; i++) {
      String texto = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      if (chat_id == CHAT_ID) {
        Serial.println("Comando recebido: " + texto);
        handleComando(texto);
      } else {
        bot.sendMessage(chat_id, "❌ Acesso não autorizado!", "");
      }
    }
    numNovasMensagens = bot.getUpdates(bot.last_message_received + 1);
  }
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  bot.sendMessage(CHAT_ID, "🤖 Bot iniciado! Comandos disponíveis:\n/scan\n/last\n/status\n/wifi\n/uptime", "");
  tempoInicio = millis();
}

void loop() {
  if (millis() - tempoUltimoScan > 5 * 60 * 1000) {
    escanearRede();
    tempoUltimoScan = millis();
  }

  if (millis() - tempoUltimaMensagem > 2000) {
    checarComandosTelegram();
    tempoUltimaMensagem = millis();
  }
}

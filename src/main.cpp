// const char* ssid = "brisa-2042583";
// const char* password = "dkcghxqs";

// // Telegram
// #define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
// #define CHAT_ID "6310061051"

// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <ESP32Ping.h>

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// void conectarWiFi() {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Conectando ao Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWi-Fi conectado!");
//   Serial.print("IP local: ");
//   Serial.println(WiFi.localIP());
//   client.setInsecure();
// }

// void escanearRede() {
//   IPAddress localIP = WiFi.localIP();
//   IPAddress subnet = WiFi.subnetMask();
//   IPAddress baseIP = localIP & subnet;

//   Serial.println("Escaneando rede...");

//   String mensagem = "📡 *Dispositivos ativos na rede:*\n\n";
//   bool algumAtivo = false;

//   for (int i = 1; i < 255; i++) {
//     IPAddress ip = baseIP;
//     ip[3] = i;

//     Serial.print("Ping para ");
//     Serial.print(ip.toString());
//     Serial.print(" ... ");

//     if (Ping.ping(ip, 1)) {
//       Serial.println("OK");
//       mensagem += "🔹 " + ip.toString() + "\n";
//       algumAtivo = true;
//     } else {
//       Serial.println("Sem resposta");
//     }
//   }

//   if (!algumAtivo) {
//     mensagem += "⚠️ Nenhum dispositivo ativo foi detectado.";
//   }

//   Serial.println("Enviando mensagem para Telegram...");
//   bot.sendMessage(CHAT_ID, mensagem, "Markdown");
//   Serial.println("Mensagem enviada!");
// }

// void setup() {
//   Serial.begin(115200);
//   conectarWiFi();
//   bot.sendMessage(CHAT_ID, "🚀 ESP32 iniciado e conectado!", "");
//   delay(1000);
//   escanearRede();
// }

// void loop() {
//   static unsigned long ultimoScan = 0;
//   if (millis() - ultimoScan > 5 * 60 * 1000) { // a cada 5 minutos
//     escanearRede();
//     ultimoScan = millis();
//   }
// }

// #include <WiFi.h>
// #include <WebServer.h>
// #include <DNSServer.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <SPIFFS.h>

// const char* ssid = "CadastroESP32";
// WebServer server(80);
// DNSServer dnsServer;
// const byte DNS_PORT = 53;

// #define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
// #define CHAT_ID "6310061051"

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// void handleRoot() {
//   File file = SPIFFS.open("/index.html", "r");
//   if (!file) {
//     server.send(500, "text/plain", "Erro ao carregar formulário.");
//     return;
//   }
//   String html = file.readString();
//   server.send(200, "text/html", html);
//   file.close();
// }

// // URLs específicas para acionar a notificação
// void handleGenerate204() {
//   // Android espera conteúdo HTML e código 200
//   server.send(200, "text/html", "<html><body>Redirecionando...</body></html>");
// }

// void handleAppleCaptive() {
//   server.send(200, "text/html", "<HTML><HEAD><TITLE>Login</TITLE></HEAD><BODY>Redirecionando...</BODY></HTML>");
// }

// void handleNCSI() {
//   server.send(200, "text/html", "<html><body>NCSI</body></html>");
// }

// void setup() {
//   Serial.begin(115200);
//   if (!SPIFFS.begin(true)) {
//     Serial.println("Erro ao iniciar SPIFFS");
//     return;
//   }

//   // Cria o Access Point
//   WiFi.softAP(ssid);
//   delay(100);

//   IPAddress apIP = WiFi.softAPIP();
//   dnsServer.start(DNS_PORT, "*", apIP); // Redireciona tudo

//   // Rotas principais
//   server.on("/", handleRoot);
//   server.on("/index.html", handleRoot); // segurança extra

//   // Rotas para captive portal
//   server.on("/generate_204", handleGenerate204);         // Android
//   server.on("/hotspot-detect.html", handleAppleCaptive); // iOS
//   server.on("/ncsi.txt", handleNCSI);                    // Windows

//   server.onNotFound([]() {
//     server.sendHeader("Location", "/", true);
//     server.send(302, "text/plain", "");
//   });

//   server.begin();
//   Serial.println("Servidor iniciado.");
// }

// void loop() {
//   dnsServer.processNextRequest();
//   server.handleClient();
// }

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

// Wi-Fi
const char* ssid = "brisa-2042583";
const char* password = "dkcghxqs";

// Telegram
#define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
#define CHAT_ID "6310061051"

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
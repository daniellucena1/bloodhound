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

const char* ssid = "brisa-2042583";
const char* password = "dkcghxqs";

#define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
#define CHAT_ID "6310061051"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

const char* macWhitelist[] = {
  "84:F3:EB:12:34:56",
  "D8:27:0C:AB:CD:EF",
  "F4:12:FA:AA:BB:CC"
};
const int whitelistSize = sizeof(macWhitelist) / sizeof(macWhitelist[0]);

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

// Função para formatar MAC do tipo uint8_t[6] para String "XX:XX:XX:XX:XX:XX"
String macToString(uint8_t* mac) {
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

bool isKnownMAC(String mac) {
  mac.toUpperCase();
  for (int i = 0; i < whitelistSize; i++) {
    if (mac == macWhitelist[i]) return true;
  }
  return false;
}

// Função para obter o MAC da tabela ARP para o IP dado
bool getMACFromARP(IPAddress ip, uint8_t* mac) {
  // Retorna true se MAC encontrado, false caso contrário
  return WiFi.getARPTable(ip, mac);
}

void escanearRede() {
  IPAddress localIP = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress baseIP = localIP & subnet;

  Serial.println("Escaneando rede...");

  String mensagem = "🚨 *Dispositivos desconhecidos detectados:*\n\n";
  bool algumInvasor = false;

  for (int i = 1; i < 255; i++) {
    IPAddress ip = baseIP;
    ip[3] = i;

    Serial.print("Ping para ");
    Serial.print(ip.toString());
    Serial.print(" ... ");

    if (Ping.ping(ip, 1)) {
      Serial.print("OK, tentando obter MAC... ");

      uint8_t mac[6];
      if (getMACFromARP(ip, mac)) {
        String macStr = macToString(mac);
        Serial.println(macStr);

        if (!isKnownMAC(macStr)) {
          mensagem += "❌ IP: " + ip.toString() + " MAC: " + macStr + "\n";
          algumInvasor = true;
        }
      } else {
        Serial.println("MAC não encontrado");
        mensagem += "❓ IP: " + ip.toString() + " (MAC desconhecido)\n";
        algumInvasor = true;
      }
    } else {
      Serial.println("Sem resposta");
    }
  }

  if (algumInvasor) {
    Serial.println("Enviando alerta para Telegram...");
    bot.sendMessage(CHAT_ID, mensagem, "Markdown");
    Serial.println("Alerta enviado!");
  } else {
    Serial.println("Nenhum dispositivo desconhecido detectado.");
  }
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

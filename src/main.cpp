// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <map>

// extern "C" {
//   #include "esp_wifi.h"
// }

// const char* ssid = "brisa-2042583";
// const char* password = "dkcghxqs";

// Telegram
#define BOT_TOKEN "7806065104:AAH2JnEC4V5XFw6Uk4l1W4q40uVFL5Q5tAI"
#define CHAT_ID "6310061051"

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// // Armazena dados dos dispositivos
// struct DispositivoInfo {
//   int pacotes = 0;
//   int rssiTotal = 0;
// };

// std::map<String, DispositivoInfo> dispositivos;

// unsigned long ultimaAtualizacao = 0;
// const unsigned long intervaloEnvio = 60 * 1000; // 20 segundos

// wifi_promiscuous_pkt_t *pkt;

// // Converte MAC para string
// String macToString(const uint8_t *mac) {
//   char buf[18];
//   sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
//           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//   return String(buf);
// }

// void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
//   wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*) buf;

//   const uint8_t* macOrigem = pkt->payload + 10;
//   String mac = macToString(macOrigem);
//   int rssi = pkt->rx_ctrl.rssi;

//   Serial.printf("📡 MAC: %s | RSSI: %d dBm\n", mac.c_str(), rssi);

//   dispositivos[mac].pacotes++;
//   dispositivos[mac].rssiTotal += rssi;
// }


// // Envia o resumo via Telegram
// void enviarResumoTelegram() {
//   if (dispositivos.empty()) {
//     Serial.println("Nenhum dispositivo");
//     return;
//   } 

//   String mensagem = "📡 *Resumo de tráfego detectado:*\n\n";

//   for (const auto& item : dispositivos) {
//     String mac = item.first;
//     const DispositivoInfo& info = item.second;
//     int mediaRSSI = info.rssiTotal / info.pacotes;

//     mensagem += "🔹 MAC: `" + mac + "`\n";
//     mensagem += "📦 Pacotes: *" + String(info.pacotes) + "*\n";
//     mensagem += "📶 RSSI médio: *" + String(mediaRSSI) + "* dBm\n\n";
//   }

//   bot.sendMessage(CHAT_ID, mensagem, "Markdown");
//   dispositivos.clear();
// }

// void setup() {
//   Serial.begin(115200);

//   // Conecta ao Wi-Fi inicialmente
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Conectando ao Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWi-Fi conectado!");
//   client.setInsecure();

//   bot.sendMessage(CHAT_ID, "🚀 Dispositivo iniciado", "");

//   // Desconecta e ativa modo promíscuo
//   // WiFi.disconnect(true);
//   WiFi.mode(WIFI_MODE_STA);
//   esp_wifi_set_promiscuous(true);
//   esp_wifi_set_promiscuous_filter(nullptr);
//   esp_wifi_set_promiscuous_rx_cb(&snifferCallback);

//   Serial.println("Sniffer iniciado!");
//   ultimaAtualizacao = millis();
// }

// void loop() {
//   unsigned long agora = millis();
//   if (agora - ultimaAtualizacao >= intervaloEnvio) {
//     // 1. Para sniffing e reconecta ao Wi-Fi
//     esp_wifi_set_promiscuous(false);
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.print("Reconectando ao Wi-Fi");
//     while (WiFi.status() != WL_CONNECTED) {
//       delay(200);
//       Serial.print(".");
//     }
//     Serial.println("\nWi-Fi reconectado!");

//     // 2. Envia mensagem
//     enviarResumoTelegram();

//     // 3. Desconecta e ativa novamente o modo promíscuo
//     WiFi.disconnect(true);
//     WiFi.mode(WIFI_MODE_NULL);
//     esp_wifi_set_promiscuous(true);
//     Serial.println("Modo promíscuo reativado.");

//     ultimaAtualizacao = agora;
//   }
// }

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

const char* ssid = "UPE-Estudantes(UnL)";
const char* password = "OrgulhodeserUPE";

// #define BOT_TOKEN "SEU_BOT_TOKEN"
// #define CHAT_ID "SEU_CHAT_ID"

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

// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include "esp_wifi.h"

// // Configurações Wi-Fi e Telegram (troque pelos seus)
// // const char* ssid = "SEU_SSID";
// // const char* password = "SUA_SENHA";

// // #define BOT_TOKEN "SEU_BOT_TOKEN"
// // #define CHAT_ID "SEU_CHAT_ID"

// WiFiClientSecure client;
// UniversalTelegramBot bot(BOT_TOKEN, client);

// // Dados para scan ARP
// uint8_t my_mac[6];
// IPAddress local_ip;
// IPAddress gateway_ip;

// // Estrutura para armazenar dispositivos ativos
// struct Dispositivo {
//   String mac;
//   IPAddress ip;
// };

// #define MAX_DISPOSITIVOS 50
// Dispositivo dispositivos[MAX_DISPOSITIVOS];
// int dispositivos_count = 0;

// // Protótipos
// void send_arp_request(IPAddress target_ip);
// void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type);
// String macToStr(const uint8_t* mac);
// String ipToStr(const uint8_t* ip);
// bool jaRegistrado(IPAddress ip);

// // Constantes
// const unsigned long intervaloEnvio = 60 * 1000; // 1 minuto
// unsigned long ultimoEnvio = 0;

// void setup() {
//   Serial.begin(115200);
//   WiFi.mode(WIFI_MODE_STA);
//   WiFi.begin(ssid, password);

//   Serial.print("Conectando WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWi-Fi conectado!");

//   client.setInsecure();

//   bot.sendMessage(CHAT_ID, "🚀 Scanner ARP iniciado", "");

//   local_ip = WiFi.localIP();

//   // Pega MAC
//   esp_wifi_get_mac(WIFI_IF_STA, my_mac);

//   // Pega gateway (roteador)
//   gateway_ip = WiFi.gatewayIP();

//   // Ativa modo promíscuo
//   esp_wifi_set_promiscuous(true);
//   esp_wifi_set_promiscuous_rx_cb(&snifferCallback);

//   Serial.printf("Meu IP: %s\n", local_ip.toString().c_str());
//   Serial.printf("Gateway: %s\n", gateway_ip.toString().c_str());
// }

// void loop() {
//   unsigned long agora = millis();

//   // A cada minuto varre a rede
//   if (agora - ultimoEnvio > intervaloEnvio) {
//     dispositivos_count = 0; // limpa lista

//     // Varre IPs da rede: exemplo 192.168.1.1 a 192.168.1.254
//     // Ajuste de acordo com sua sub-rede
//     IPAddress baseIP = IPAddress(local_ip[0], local_ip[1], local_ip[2], 0);

//     Serial.println("Iniciando varredura ARP...");

//     for (int i = 1; i < 255; i++) {
//       IPAddress ip = IPAddress(baseIP[0], baseIP[1], baseIP[2], i);
//       if (ip == local_ip) continue; // pula meu IP
//       send_arp_request(ip);
//       delay(10); // pequeno delay para evitar flood
//     }

//     ultimoEnvio = agora;

//     // Espera alguns segundos para respostas chegarem
//     delay(5000);

//     // Envia resumo para Telegram
//     if (dispositivos_count == 0) {
//       bot.sendMessage(CHAT_ID, "⚠️ Nenhum dispositivo ativo foi detectado.", "");
//     } else {
//       String mensagem = "📡 *Dispositivos ativos na rede:*\n\n";
//       for (int i = 0; i < dispositivos_count; i++) {
//         mensagem += "🔹 IP: `" + dispositivos[i].ip.toString() + "`\n";
//         mensagem += "📶 MAC: `" + dispositivos[i].mac + "`\n\n";
//       }
//       bot.sendMessage(CHAT_ID, mensagem, "Markdown");
//     }
//   }
// }

// void send_arp_request(IPAddress target_ip) {
//   uint8_t broadcast_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
//   uint8_t buffer[100] = {0};
//   int pos = 0;

//   // Cabeçalho 802.11 simplificado
//   typedef struct {
//     uint16_t frame_control;
//     uint16_t duration_id;
//     uint8_t addr1[6]; // Dest MAC (broadcast)
//     uint8_t addr2[6]; // Orig MAC (meu MAC)
//     uint8_t addr3[6]; // BSSID (igual meu MAC)
//     uint16_t seq_ctrl;
//   } __attribute__((packed)) wifi_mac_header_t;

//   wifi_mac_header_t *hdr = (wifi_mac_header_t*) buffer;
//   hdr->frame_control = 0x4000; // frame tipo Data
//   hdr->duration_id = 0;
//   memcpy(hdr->addr1, broadcast_addr, 6);
//   memcpy(hdr->addr2, my_mac, 6);
//   memcpy(hdr->addr3, my_mac, 6);
//   hdr->seq_ctrl = 0;
//   pos += sizeof(wifi_mac_header_t);

//   // Cabeçalho Ethernet
//   typedef struct {
//     uint8_t dest[6];
//     uint8_t src[6];
//     uint16_t ethertype;
//   } __attribute__((packed)) ethernet_header_t;

//   ethernet_header_t *eth = (ethernet_header_t*) (buffer + pos);
//   memcpy(eth->dest, broadcast_addr, 6);
//   memcpy(eth->src, my_mac, 6);
//   eth->ethertype = htons(0x0806); // ARP
//   pos += sizeof(ethernet_header_t);

//   // Pacote ARP
//   typedef struct {
//     uint16_t hw_type;
//     uint16_t proto_type;
//     uint8_t hw_size;
//     uint8_t proto_size;
//     uint16_t opcode;
//     uint8_t sender_mac[6];
//     uint8_t sender_ip[4];
//     uint8_t target_mac[6];
//     uint8_t target_ip[4];
//   } __attribute__((packed)) arp_packet_t;

//   arp_packet_t *arp = (arp_packet_t*) (buffer + pos);
//   arp->hw_type = htons(1);    // Ethernet
//   arp->proto_type = htons(0x0800); // IPv4
//   arp->hw_size = 6;
//   arp->proto_size = 4;
//   arp->opcode = htons(1); // ARP request
//   memcpy(arp->sender_mac, my_mac, 6);
//   memcpy(arp->sender_ip, &local_ip, 4);
//   memset(arp->target_mac, 0, 6);
//   memcpy(arp->target_ip, &target_ip, 4);
//   pos += sizeof(arp_packet_t);

//   esp_wifi_80211_tx(WIFI_IF_STA, buffer, pos, false);

//   Serial.printf("Enviado ARP request para %s\n", target_ip.toString().c_str());
// }

// void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
//   if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;

//   const wifi_promiscuous_pkt_t* p = (wifi_promiscuous_pkt_t*) buf;
//   const uint8_t* payload = p->payload;
//   int len = p->rx_ctrl.sig_len;

//   // Pula cabeçalho 802.11 (24 bytes)
//   if (len < 42) return;

//   const uint8_t* ethernet_frame = payload + 24;

//   // Cabeçalho Ethernet
//   typedef struct {
//     uint8_t dest[6];
//     uint8_t src[6];
//     uint16_t ethertype;
//   } __attribute__((packed)) ethernet_header_t;

//   const ethernet_header_t* eth = (ethernet_header_t*) ethernet_frame;

//   uint16_t ethertype = (eth->ethertype << 8) | (eth->ethertype >> 8);
//   if (ethertype != 0x0806) return; // ARP só

//   // Cabeçalho ARP
//   typedef struct {
//     uint16_t hw_type;
//     uint16_t proto_type;
//     uint8_t hw_size;
//     uint8_t proto_size;
//     uint16_t opcode;
//     uint8_t sender_mac[6];
//     uint8_t sender_ip[4];
//     uint8_t target_mac[6];
//     uint8_t target_ip[4];
//   } __attribute__((packed)) arp_packet_t;

//   const arp_packet_t* arp = (arp_packet_t*) (ethernet_frame + 14);

//   uint16_t opcode = (arp->opcode << 8) | (arp->opcode >> 8);
//   if (opcode != 2) return; // só ARP Reply (resposta)

//   IPAddress ip(arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3]);
//   String mac = macToStr(arp->sender_mac);

//   // Evita duplicados
//   if (!jaRegistrado(ip)) {
//     if (dispositivos_count < MAX_DISPOSITIVOS) {
//       dispositivos[dispositivos_count].ip = ip;
//       dispositivos[dispositivos_count].mac = mac;
//       dispositivos_count++;
//       Serial.printf("Dispositivo ativo: IP %s MAC %s\n", ip.toString().c_str(), mac.c_str());
//     }
//   }
// }

// String macToStr(const uint8_t* mac) {
//   char buf[18];
//   sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
//           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//   return String(buf);
// }

// bool jaRegistrado(IPAddress ip) {
//   for (int i = 0; i < dispositivos_count; i++) {
//     if (dispositivos[i].ip == ip) return true;
//   }
//   return false;
// }

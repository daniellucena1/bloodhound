# 🔍 ESP32 Subnet Scanner com Telegram Bot

Este projeto utiliza um ESP32 para escanear toda a sub-rede local (ex: `192.168.1.1` a `192.168.1.254`) em busca de dispositivos ativos. A detecção é feita através de conexões TCP rápidas (na porta 80). Os IPs encontrados são enviados automaticamente via Telegram para um chat especificado.

---

## 📦 Funcionalidades

- 📡 Escaneia todos os IPs da sub-rede local (exceto o próprio ESP32)
- ⚡ Detecta dispositivos ativos via tentativa de conexão TCP
- 📬 Envia relatório de IPs ativos para um chat no Telegram
- 🔁 Executa o scan periodicamente (ex: a cada 1 minuto)

---

## 🧰 Requisitos

- Placa ESP32 (ex: ESP32 DevKit v1)
- Biblioteca [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
- Conta no Telegram e um **Bot Token**
- Conexão Wi-Fi funcional

---

## 📁 Estrutura do Projeto (PlatformIO)

```py
bloodhound/
├── include/
├── lib/
├── src/
│ └── main.cpp
├── platformio.ini
└── README.md
```


---

## 🛠️ Configuração

1. **Crie um bot no Telegram**  
   Converse com [@BotFather](https://t.me/botfather) e pegue o `BOT_TOKEN`.

2. **Pegue o seu chat ID**  
   Use um bot como [@userinfobot](https://t.me/userinfobot) para descobrir seu `CHAT_ID`.

3. **Configure o código**  
   No `main.cpp`, altere as seguintes variáveis:

```cpp
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";
#define BOT_TOKEN "SEU_BOT_TOKEN"
#define CHAT_ID "SEU_CHAT_ID"
```
4. **Compile e envie para o ESP32**

## ⏱️ Intervalo de Varredura

Por padrão, a varredura é feita a cada 1 minuto.
Você pode alterar isso no main.cpp:

```cpp
unsigned long intervaloEnvio = 60 * 1000;  // 60 segundos
```

## 📷 Exemplo de Mensagem no Telegram

```cpp
📡 *Dispositivos ativos na rede:*

🔹 IP: `192.168.1.10`

🔹 IP: `192.168.1.25`

🔹 IP: `192.168.1.42`

```
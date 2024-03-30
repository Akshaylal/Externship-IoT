#include <FS.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <WiFiManager.h> 

char botToken[50] = "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
char chatId[10] = "XXXXXXXXX";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot *bot;

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
bool shouldSaveConfig = false;

const int ledPin = 2;
bool ledState = LOW;

StaticJsonDocument<200> config;

void saveConfigCallback() {
  shouldSaveConfig = true;
}

void readConfig() {
  if (!LittleFS.exists("config.json")){
    Serial.println("Config does not exist.");
    return;
  }

  File file = LittleFS.open("config.json", "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return;
  }

  deserializeJson(config, file);
  strcpy(botToken, config["token"]);
  strcpy(chatId, config["chatid"]);

  file.close();
}

void writeConfig() {
  File file = LittleFS.open("config.json", "w");
  if (!file) {
    Serial.println("Failed to write config file");
    return;
  }

  config["token"] = botToken;
  config["chatid"] = chatId;
  serializeJson(config, file);
  file.close();
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot->messages[i].chat_id);
    if (chat_id != chatId){
      bot->sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot->messages[i].text;
    Serial.println(text);

    String from_name = bot->messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/led_on to turn led ON \n";
      welcome += "/led_off to turn led OFF \n";
      welcome += "/state to request current led state \n";
      bot->sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot->sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/led_off") {
      bot->sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/state") {
      if (digitalRead(ledPin)){
        bot->sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot->sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  Serial.println("FS mounted");

  readConfig();
  Serial.print("Token: ");
  Serial.println(botToken);
  Serial.print("Chat ID: ");
  Serial.println(chatId);

  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  WiFiManager wm;
  wm.setSaveConfigCallback(saveConfigCallback);
  //Adding an additional config on the WIFI manager webpage for the bot token and chat id
  WiFiManagerParameter custom_bot_token("bottoken", "Bot Token", botToken, 50);
  WiFiManagerParameter custom_chat_id("chatid", "Chat Id", chatId, 10);
  wm.addParameter(&custom_bot_token);
  wm.addParameter(&custom_chat_id);
  //If it fails to connect it will create a TgBot access point
  wm.autoConnect("TgBot");

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  if (shouldSaveConfig) {
    Serial.println("Writing new config");
    strcpy(botToken, custom_bot_token.getValue());
    strcpy(chatId, custom_chat_id.getValue());
    writeConfig();
  }

  LittleFS.end();

  bot = new UniversalTelegramBot(botToken, client);
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
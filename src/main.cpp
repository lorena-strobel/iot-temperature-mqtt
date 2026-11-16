#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h> // Nova biblioteca para Wi-Fi dinâmico
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// -----------------------------------------
// 1. CONFIGURAÇÕES DO MQTT E SENSOR
// -----------------------------------------
const char* mqtt_server = "test.mosquitto.org"; // Broker público
const int mqtt_port = 1883;
const char* topico_temperatura = "trabalho/iot/temperatura"; 

const int oneWireBus = 4; // GPIO 4 (fio de dados do DS18B20)

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexao MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  
  WiFiManager wifiManager;

  // wifiManager.resetSettings();
  
  Serial.println("Iniciando conexao Wi-Fi...");
  
  if (!wifiManager.autoConnect("ESP32_Config")) {
    Serial.println("Falha ao conectar e tempo limite atingido. Reiniciando...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("");
  Serial.println("WiFi conectado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  
  // Configura o servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors.requestTemperatures(); 
  float temperaturaC = sensors.getTempCByIndex(0);
  
  if (temperaturaC != -127.00) {
    Serial.print("Temperatura lida: ");
    Serial.print(temperaturaC);
    Serial.println("ºC");

    char tempString[8];
    dtostrf(temperaturaC, 1, 2, tempString);
    client.publish(topico_temperatura, tempString);
  } else {
    Serial.println("Erro: Sensor DS18B20 não encontrado.");
  }
  
  delay(5000);
}
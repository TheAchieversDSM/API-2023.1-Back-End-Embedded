#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>
HTTPClient hpost;
#define LED 2
/* CONFIGURAÇÃO DA PLACA */
/* CONFIGURAÇÕES DE CONEXÃO COM A REDE */
char * ssid = "<REDE-WIFI>";
char * pwd = "<SENHA-WIFI>";

char * server = "http://34.195.208.67:5000/";

String nomeParametro = "vento";


/* VARIAVEIS PARA GERAÇÃO DE DADOS */
String uid;
unsigned long epochTime;
unsigned long dataMillis = 0;
int x = 0;
int count2 = 23;
String result_string;

void connectWiFi() {
  Serial.println("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi com o Ip: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  digitalWrite(LED, HIGH);
  WiFi.begin(ssid, pwd);
  uid = WiFi.macAddress();
  uid.replace(":", "");
  pinMode(LED,OUTPUT);
  connectWiFi();
  Serial.println("================================");
}

/*RECUPERAÇÃO DE UNIXTIME*/
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime( & timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time( & now);
  return now;
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - dataMillis > 10000) {
      dataMillis = millis();

      /*URL PARA O FLASK*/
      String url = String(server) + "submitToFirebase";
      
      WiFiClient wClient;
      hpost.begin(wClient, url);

      /*GERAÇÃO DE DADOS ALEATORIOS*/
      x = x + 1;
      double random_value = ((double) rand() / RAND_MAX); // gera um valor aleatório entre 0 e 1
      double result = sin(2.0 * (x + random_value)) + sin(M_PI * (x + random_value));

      /*CRIAÇÃO DO JSON*/
      hpost.addHeader("Content-Type", "application/json");
      String data = "{\"_uid\":\"" + uid + "\",\"_unixtime\":\"" + String(epochTime) + "\",\"_nomeParametro\":\"" + nomeParametro + "\",\"_medida\":" + result + "}";
      /*ENVIAR PARA O IP*/
      int httpReturn = hpost.POST(data);
    
      /*RETORNO DO SERIAL*/
      if (httpReturn > 0) {
        Serial.println(" ");
        Serial.println("JSON ENVIADO: ");
        Serial.println(data);
        Serial.println("URL: " + url);
        Serial.println(" ");
        Serial.print("Retorno do echo: ");
        Serial.println(hpost.getString());
        Serial.println(" ");
        Serial.println("================================");

      } else {
        Serial.println("HTTP POST error");
      }

      digitalWrite(LED, HIGH);
    }
  } else {
    /*RECONEXÃO CASO FALHA NA CONEXÃO*/
    Serial.println("Sem internet");
    connectWiFi();
  }
  digitalWrite(LED, LOW);
}

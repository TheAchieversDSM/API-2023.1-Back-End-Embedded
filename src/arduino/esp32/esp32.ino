#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>
#include <String.h>
HTTPClient hpost;
#define LED 2
/* CONFIGURAÇÃO DA PLACA */
/* CONFIGURAÇÕES DE CONEXÃO COM A REDE */
char * ssid = "<NOME-DA-REDE>";
char * pwd = "<SENHA-DA-REDE>";

class params {
  public:
    char nomeParametro[30];
    int medida;
    int acrescimoDeMedida;
};
int chuvaAcrescimo = 0;
const int paramArrayQnt = 3;

params paramList[paramArrayQnt];

char * server = "http://54.226.206.196:5000/";

/* VARIAVEIS PARA GERAÇÃO DE DADOS */
String uid;
unsigned long epochTime;
const char* ntpServer = "pool.ntp.org";
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
  configTime(0, 0, ntpServer);
  pinMode(LED,OUTPUT);
  connectWiFi();

  strcpy(paramList[0].nomeParametro, "temperatura");
  paramList[0].acrescimoDeMedida = 0;
  strcpy(paramList[1].nomeParametro, "umidade");
  paramList[1].acrescimoDeMedida = 0;
  strcpy(paramList[2].nomeParametro, "chuva");
  paramList[2].acrescimoDeMedida = 1;
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
    if (millis() - dataMillis > 30000) {
      dataMillis = millis();
      epochTime = getTime();
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

      String data = "{\"_uid\": \"" + uid + "\",\"_unixtime\": \"" + String(epochTime) + "\",\"parametros\": [";
      int dataLength = data.length() + 1;
      char dataArray[dataLength];

      data.toCharArray(dataArray, dataLength);
      
      for(int i = 0; i < paramArrayQnt; i++){
        double random_value = ((double) rand() / RAND_MAX); // gera um valor aleatório entre 0 e 1
        double result = 0;
        if (paramList[i].acrescimoDeMedida == 1){
          result = chuvaAcrescimo;
          chuvaAcrescimo = chuvaAcrescimo + 15;
        }else{
          result = sin(2.0 * (x + random_value)) + sin(M_PI * (x + random_value));
          
        }
        if(i == paramArrayQnt - 1){
          data = data + "{\"_nomeParametro\": \"" + paramList[i].nomeParametro + "\",\"_medida\": " + String(result) + "}";          
        }else{
          data = data + "{\"_nomeParametro\": \"" + paramList[i].nomeParametro + "\",\"_medida\": " + String(result) + "},";
        }
      }

      data = data + "], \"enviado\": false}";
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

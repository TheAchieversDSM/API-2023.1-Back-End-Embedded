#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <math.h>

#define LED 2

// FIREBASE
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>

#include "time.h"

const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 

// WIFI
/* 1. Definir credenciais de WIFI */
#define WIFI_SSID "REDE-DE-WIFI"
#define WIFI_PASSWORD "SENHA-DO-WIFI"

/* 2. Definir o URL e o secret do banco */
#define DATABASE_URL "https://tecsus-59210-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "saLWJGgsVB1SHf4HWe9Oi36nEEZJE6BE54zKYdOg"

/* 3. Definir o objeto de dados do firebase */
FirebaseData fbdo;

/* 4, Definir a autenticação (TODO) */
FirebaseAuth auth;

/* Definir os dados de configuração do firebase */
FirebaseConfig config;

unsigned long dataMillis = 0;
int x = 0;
int count2 = 23;

String uid;
String result_string;

void setup()
{

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the certificate file (optional) */
    // config.cert.file = "/cert.cer";
    // config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL and database secret(required) */
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    // Or use legacy authenticate method
    // Firebase.begin(DATABASE_URL, DATABASE_SECRET);

    uid = WiFi.macAddress();
    uid.replace(":","");

    configTime(0, 0, ntpServer);

    pinMode(LED,OUTPUT);
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void loop()
{
    if (millis() - dataMillis > 60000)
    {
        dataMillis = millis();
        epochTime = getTime();
        x = x + 1;
        double random_value = ((double)rand() / RAND_MAX); // gera um valor aleatório entre 0 e 1
        double result = sin(2.0 * (x + random_value)) + sin(M_PI * (x + random_value));
        result_string = String(epochTime) + "_" + result + "_" + "temperatura";
        Serial.printf("Registrando %s\n", Firebase.RTDB.setString(&fbdo, "/esp32/" + uid + "/" + String(epochTime), result_string) ? "; Pulso gerado" : fbdo.errorReason().c_str());
        digitalWrite(LED, HIGH);
        delay(5000);
    }
    digitalWrite(LED, LOW);
}
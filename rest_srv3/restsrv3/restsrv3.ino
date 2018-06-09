#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

struct Tled_resource {
    byte id;
    int  gpio;
    byte status;
    byte action;
};

Tled_resource led_resource[8];

const char* wifi_ssid = "GVT-B41A";
//const char* wifi_ssid = "MRF ASUS";
const char* wifi_passwd = "(182213)";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

/*
 *Inicializa o recurso com dados default
 */
void init_led_resource(int nId) {
    led_resource[nId].id = 0;
    led_resource[nId].gpio = 0;
    led_resource[nId].status = LOW;
    led_resource[nId].action = 0;  //0 não informado; 1 Inicializar; 2 Executar;
}

/*
 *Define o tipo de operação para a porta
*/
void setled_mode(int ngpio){
   if (ngpio == 1) {
      pinMode(D1, OUTPUT); 
   } else if (ngpio == 2) {
      pinMode(D2, OUTPUT); 
   } else if (ngpio == 3) {
      pinMode(D3, OUTPUT); 
   } else if (ngpio == 4) {
      pinMode(D4, OUTPUT);     
   } else if (ngpio == 5) {
      pinMode(D5, OUTPUT); 
   } else if (ngpio == 6) {
      pinMode(D6, OUTPUT); 
   } else if (ngpio == 7) {
      pinMode(D7, OUTPUT); 
   } else if (ngpio == 8) {
      pinMode(D8, OUTPUT); 
   }
}

/*
 *Liga/desliga a porta selecionada
 */
void setled_onoff(int ngpio, int nSt){
   if (ngpio == 1) {
      digitalWrite(D1, nSt);
   } else if (ngpio == 2) {
      digitalWrite(D2, nSt);
   } else if (ngpio == 3) {
      digitalWrite(D3, nSt);
   } else if (ngpio == 4) {
      digitalWrite(D4, nSt);
   } else if (ngpio == 5) {
      digitalWrite(D5, nSt);
   } else if (ngpio == 6) {
      digitalWrite(D6, nSt);
   } else if (ngpio == 7) {
      digitalWrite(D7, nSt);
   } else if (ngpio == 8) {
      digitalWrite(D8, nSt);
   }
}


int init_wifi() {
    int retries = 0;

    Serial.println("Connecting to WiFi AP..........");

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_passwd);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    return WiFi.status(); // return the WiFi connection status
}

/*
*Mostra a situação de cada porta
*/
void get_leds() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonObj = jsonBuffer.createObject();
    char JSONmessageBuffer[200];
    int nId; 

    for (nId=1; nId <=8; nId++) {
      if (led_resource[nId].id == 0)
          http_rest_server.send(204);
      else {
          jsonObj["id"] = led_resource[nId].id;
          jsonObj["gpio"] = led_resource[nId].gpio;
          jsonObj["status"] = led_resource[nId].status;
          jsonObj.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
          http_rest_server.send(200, "application/json", JSONmessageBuffer);
      }
    }
}

void json_to_resource(JsonObject& jsonBody) {
    int id, gpio, status, action;
    int nId;

    nId = jsonBody["id"];

    id = jsonBody["id"];
    gpio = jsonBody["gpio"];
    status = jsonBody["status"];
    action = jsonBody["action"];

    led_resource[nId].id = id;
    led_resource[nId].gpio = gpio;
    led_resource[nId].status = status;
    led_resource[nId].action = action;
}

void post_put_leds() {
    int nId;    
  
    StaticJsonBuffer<500> jsonBuffer;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);

    JsonObject& jsonBody = jsonBuffer.parseObject(post_body);

    Serial.print("HTTP Method: ");
    Serial.println(http_rest_server.method());
    Serial.println(http_rest_server.arg("plain"));

    if (!jsonBody.success()) {
        //Erro na validação do Json
        Serial.println("error in parsin json body");
        http_rest_server.send(400);
    } else {
        //Se for o método HTTP_POST OU HTTP_PUT 
        if ((http_rest_server.method() == HTTP_POST) || (http_rest_server.method() == HTTP_PUT)) {
            if ((jsonBody["id"] < 1) || (jsonBody["id"] > 8) ) { //ID INCORRETO
               Serial.print("ID INCORRETO ");
               Serial.println((const char*)jsonBody["id"]);
               http_rest_server.send(404);
            } else {
              
               nId = jsonBody["id"];
               if (jsonBody["action"] == "1") {
                  //se o LED ainda não foi setado
                  if (jsonBody["id"] != led_resource[nId].id) {
                     //Inicializar o recurso
                     Serial.print("Inicializando Recurso: ");
                     Serial.println((const char*)jsonBody["id"]);                     

                     json_to_resource(jsonBody);
                     http_rest_server.sendHeader("Location", "/leds/" + String(led_resource[nId].id));
                     http_rest_server.send(201);
                     setled_mode(led_resource[nId].gpio);  
                     
                  } else {
                     //Recurso ja inicializado
                     Serial.print("Recurso já inicializado: ");
                     Serial.println((const char*)jsonBody["id"]);
                     http_rest_server.send(409);
                  }
               } else if (jsonBody["action"] == "2" ) {
                   if ( (jsonBody["id"] == led_resource[nId].id) ) {
                      //Executar o recurso
                      Serial.print("Executar o recurso: ");
                      Serial.println((const char*)jsonBody["id"]);
                      Serial.print("Status: ");
                      Serial.println((const char*)jsonBody["status"]);
                      json_to_resource(jsonBody);
                      http_rest_server.sendHeader("Location", "/leds/" + String(led_resource[nId].id));
                      http_rest_server.send(200);
                      setled_onoff(led_resource[nId].gpio, led_resource[nId].status );

                   } else {
                      //Recurso não inicializado ou incorreto
                      //(jsonBody["id"] != led_resource.id)
                      Serial.println("Recurso não inicializado ou incorreto ");
                      Serial.print("Recebido: ");
                      Serial.print((const char*)jsonBody["id"]);
                      Serial.print("  /  Inicializado: ");
                      Serial.println(led_resource[nId].id);
                      http_rest_server.send(409);
                   }
               } else {
                   //Ação não esperada
                   Serial.print("Ação não esperada: ");
                   Serial.println((const char*)jsonBody["action"]);
                   //exibe("action");
                   Serial.println("xxxxxxxx");
                   http_rest_server.send(404);
               }
           }
       }
    }
}

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/leds", HTTP_GET, get_leds);
    http_rest_server.on("/leds", HTTP_POST, post_put_leds);
    http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}

void setup(void) {
    int nId;
    Serial.begin(115200);

    //Carrega parametros iniciais do recurso LED
    for (nId = 1; nId <= 8; nId++) {
       init_led_resource(nId);
    }
    
    if (init_wifi() == WL_CONNECTED) {
        Serial.print("Connetted to ");
        Serial.print(wifi_ssid);
        Serial.print("---> IP: ");
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.print("Error connecting to: ");
        Serial.println(wifi_ssid);
    }

    config_rest_server_routing();

    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    http_rest_server.handleClient();
}

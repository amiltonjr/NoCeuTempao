/****************************************

PROGRAMAÇÃO DA ESTAÇÃO ARDUINO BLACKBOARD

Autor: Amilton Fontoura de Camargo Junior
Site: www.dicasemgeral.com
Data: 01/06/2017
Baudrate: 19200 baud
Board: Arduino/Genuino Uno

Aceita comandos do tipo:

  ESP8266>algumacoisa - Onde "algumacoisa"
  é um comando aceito pelo ESP8266

*****************************************/

// Inclui a biblioteca do LCD
#include <LiquidCrystal.h>

// Inclui a biblioteca do ESP8266
#include <SoftwareSerial.h>

/////////////////////////////////////////

// Define os dados da rede WiFi
String ssid = "Comandante";
String pass = "1234";

// Respostas dos comandos do ESP8266
#define EXECUTADO "10"
#define OK "1"
#define ERRO "-1"
#define TEMPOESGOTADO "-2"
#define INVALIDO "-3"

// Define se deve fazer o debug na Serial
#define DEBUG false

// Variável auxiliar
bool firstRun = true;

// Define os pinos TX e RX do ESP8266, respectivamente
SoftwareSerial esp8266(10, 11);

// Variável auxiliar
String _cmd = "";

// Inclui a biblioteca do sensor DHT
#include "DHT.h"

// Define a porta onde o sensor está conectado
#define DHTPIN A0

// Define o tipo do sensor como DHT11
#define DHTTYPE DHT11

// Define as porta digital do LED
#define LED 6

// Define o delay para a leitura do sensor (em segundos)
#define DELAY 30

// Define o timeout da resposta do SP8266 (em milissegundos)
#define TIMEOUT 3000

// Inicia o LCD
LiquidCrystal lcd(8, 7, 5, 4, 3, 2);

// Define o caractere grau º
byte grau[8] ={ B00001100,

                B00010010,

                B00010010,

                B00001100,

                B00000000,

                B00000000,

                B00000000,

                B00000000 };

// Inicializa o sensor
DHT dht(DHTPIN, DHTTYPE);

/////////////////////////////////////////

// Função que envia dados ao ESP8266
String sendData(String command, const int timeout, boolean debug) {
  if (debug) {
    Serial.print("> Enviando comando ao ESP8266: ");
    Serial.println(command);
  }
  
  String response = "";
  
  // Envia o comando
  esp8266.print(command);
  // Obtém o timestamp atual
  long int time = millis();

  while ((time+timeout) > millis()) {
    while(esp8266.available()) {
      // Resposta do servidor
      char c = esp8266.read(); // Lê caractere por caractere
      response += c; 
    } 
  }

  // Imprime o debug
  if (debug) {
    Serial.print("> Resposta do ESP8266: ");
    Serial.println(response);
  }
   
  return response;
}

// Função que conecta à uma rede wifi no ESP8266
void Conectar(String ssid, String pass, bool debug) {
  if (debug) Serial.println("Enviando comando de reconectar ao ESP8266...");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conectando em");
  lcd.setCursor(0,1);
  lcd.print(ssid);

  _cmd = "ssid:";
  _cmd.concat(ssid);
  sendData(_cmd, TIMEOUT, debug);

  _cmd = "pass:";
  _cmd.concat(pass);
  sendData(_cmd, TIMEOUT, debug);

  _cmd = "reconectar:1";
  sendData(_cmd, TIMEOUT, debug);
}

// Função que retorna o valor de um comando enviado ao ESP8266
String retornoComando(String cmd) {
  // Envia os dados ao ESP8266
  String res = sendData(cmd, TIMEOUT, DEBUG);
  // Remove caracteres especiais como \t e \n do início e do fim
  res.trim();
  
  if (DEBUG) {
    Serial.print("- Resposta do comando em retornoComando(): ");
    Serial.println(res);
  }
  
  return res;
}

// Função que é executada uma vez no início
void setup() {

  // Inicia a comunicação serial
  Serial.begin(19200);
  esp8266.begin(9600);
  
  if (DEBUG) Serial.print("> Iniciando a estacao meteorologica... ");
  
  // Define a porta do LED como output
  pinMode(LED, OUTPUT);

  // Inicia a configuração do LCD
  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(0, grau);

  // Inicia o sensor
  dht.begin();

  if (DEBUG) Serial.println("OK!");
  
}

// Função que é executada em loop no Arduino
void loop() {

  // Verifica se é a primeira execução
  if (firstRun) {
    // Obtém a rede conectada no ESP8266
    String rede = retornoComando("getssid:1");
    
    // Verifica se a rede é diferente da conectada no ESP8266
    if (!ssid.equals(rede))
      Conectar(ssid, pass, DEBUG);
    
    firstRun = false;
  }
  
  // Se recebeu algum comando via serial
  if (Serial.available() > 0) {
    // Faz a leitura do comando recebido
    String comando = Serial.readString();
    // Remove caracteres como \t ou \n
    comando.trim();
    
    // Imprime o comando recebido
    Serial.print("-- Comando recebido: ");
    Serial.print(comando);
    Serial.print("... ");

    // Separa o comando do seu valor
    int separador = comando.indexOf('>');

    // Se o comando é inválido
    if (separador < 0) {
      Serial.println("Comando inválido!");
    
    // Se o comando é válido
    } else {
      // Separa em comando e valor
      String cmd = comando.substring(0, separador);
      String vlr = comando.substring(separador+1, comando.length());

      // Se é comando para o ESP8266
      if (cmd.equalsIgnoreCase("ESP8266")) {
        Serial.println("Comando enviado ao ESP8266!");
        sendData(vlr, TIMEOUT, true);
      // Comando inválido
      } else
        Serial.println("Comando inválido!");
    }
  }
  
  // Limpa o LCD
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print("Verificando o");
  lcd.setCursor(0,1);
  lcd.print("WiFi");

  // Verifica se o servidor está online
  String online = retornoComando("status:1");
  
  if (online.equals(OK)) {

    lcd.print(": Conectado!");
    delay(2 * 1000);
    
    // Obtém o SSID da rede wifi conectada
    ssid = retornoComando("getssid:1");
    
    // Imprime o SSID por 10 segundos
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Rede WiFi:");
    lcd.setCursor(0,1);
    lcd.print(ssid);

    delay(8 * 1000); // Aguarda 8 segundos
    
    // Obtém o endereço IP do servidor
    String IP = retornoComando("getip:1");
    
    // Imprime o IP do servidor por 10 segundos
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Online no IP:");
    lcd.setCursor(0,1);
    lcd.print(IP);
  
  // Servidor não está online
  } else
    lcd.print(": ERRO!");

  // Aguarda 10 segundos antes de prosseguir
  delay (10 * 1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lendo o sensor..");
  
  // Pisca o LED
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  
  if (DEBUG) Serial.print("> Lendo dados do sensor... ");
  
  // Lê a umidade relativa do ar (em porcentagem)
  float umidade = dht.readHumidity();
  // Lê a temperatura do ar (em Celsius)
  float temp_c = dht.readTemperature();
  // Lê a temperatura do ar (em Fahrenheit)
  float temp_f = dht.readTemperature(true);

  // Verifica se ocorreu algum erro na leitura
  if (isnan(umidade) || isnan(temp_c)) {
    if (DEBUG) Serial.println("Erro na leitura!");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Erro na leitura!");

    // Pisca o LED indicando erro
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    
    return;
  }

  if (DEBUG) Serial.println("Leitura OK!");

  // Computa o índice de calor em Celsius
  float indiceCalor = dht.computeHeatIndex(temp_c, umidade, false);

  // Envia os dados para o ESP8266
  _cmd = "temp_c:";
  _cmd.concat(temp_c);
  sendData(_cmd, TIMEOUT, DEBUG);
  _cmd = "temp_f:";
  _cmd.concat(temp_f);
  sendData(_cmd, TIMEOUT, DEBUG);
  _cmd = "umidade:";
  _cmd.concat(umidade);
  sendData(_cmd, TIMEOUT, DEBUG);
  _cmd = "indice:";
  _cmd.concat(indiceCalor);
  sendData(_cmd, TIMEOUT, DEBUG);
  
  lcd.clear();
  lcd.setCursor(0,0);

  // Acende o LED
  digitalWrite(LED, HIGH);

  lcd.print("Temp: ");
  lcd.print(" ");
  lcd.setCursor(6,0);
  lcd.print(temp_c,1);
  lcd.setCursor(10,0);
  lcd.write((byte)0);
  lcd.setCursor(0,1);
  lcd.print("Umidade: ");
  lcd.print(" ");
  lcd.setCursor(9,1);
  lcd.print(umidade,1);
  lcd.setCursor(13,1);
  lcd.print("%");

  // Aguarda antes de começar a ler o sensor novamente
  delay(DELAY * 1000);
  
}


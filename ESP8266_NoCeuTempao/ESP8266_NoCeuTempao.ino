/*******************************************************

PROGRAMAÇÃO DO MÓDULO ESP8266 01

Autor: Amilton Fontoura de Camargo Junior
Site: www.dicasemgeral.com
Data: 01/06/2017
Baudrate: 9600 baud
Board: Generic ESP8266 Module
Flash Size: 1M (512K SPIFFS)
Upload Speed: 115200

>> ! IMPORTANTE !
>> Para programar, ligue o pino GPIO0 no GND com um resistor,
>> RST do Arduino no GND e ligue TX com TX e RX com RX

Lista de comandos disponíveis no formato (comando:valor):

  status:1 - Retorna o status do servidor (OK ou ERRO)
  reiniciar:1 - Reinicia o módulo ESP8266
  ssid:abc - Muda o SSID da rede para abc
  pass:123 - Muda a senha da rede para 123
  reconectar:1 - Força a reconexão com a rede wifi ssid
  getssid:1 - Retorna o SSID da rede WiFi
  getip:1 - Retorna o endereço IP do servidor
  getporta:1 - Retorna a porta do servidor
  temp_c:123 - Define a temperatura em Célsius
  temp_f:123 - Define a temperatura em Fahrenheit
  umidade:123 - Define a umidade relativa do ar
  indice:123 - Define o índice de calor

*******************************************************/

// Inclui a bilioteca de funções
#include <ESP8266WiFi.h>

/////////////////////////////////////

// Variáveis da rede sem fio
String ssid = "Comandante";
String pass = "1234";
String IP = "";
int porta = 80;

// Variáveis globais
float temp_c = 0.0;
float temp_f = 0.0;
float umidade = 0.0;
float indiceCalor = 0.0;
bool status = false;

// Respostas dos comandos do ESP8266
#define EXECUTADO "10"
#define OK "1"
#define ERRO "-1"
#define TEMPOESGOTADO "-2"
#define INVALIDO "-3"

/////////////////////////////////////

// Define a porta do servidor
WiFiServer server(porta);

// Função que reinicia o ESP8266
void Reiniciar() {
  Serial.println("Reiniciando o ESP8266...");
  status = false;
  ESP.restart();
}

// Função que conecta à uma rede wifi
void Conectar(String ssid, String pass, bool reconectar) {
  volatile unsigned int cont = 0;

  // Verifica se já há uma conexão estabelecida
  if (status)
    WiFi.disconnect();
  
  WiFi.mode(WIFI_STA);

  if (!reconectar) {
    Serial.print("> Conectando na rede: ");
    Serial.print(ssid);
  }

  // Converte os valores de String para char*
  char _ssid[sizeof(ssid)];
  ssid.toCharArray(_ssid, sizeof(_ssid));
  char _pass[sizeof(pass)];
  pass.toCharArray(_pass, sizeof(_pass));

  status = false;
  
  WiFi.begin(_ssid, _pass);

  // Aguarda até a conexão ser estabelecida
  while (WiFi.status() != WL_CONNECTED && cont++ < 60) {
    delay(500);
    if (!reconectar) Serial.print(".");
  }

  // Se expirou o tempo
  if (cont >= 60) {
    if (!reconectar)
      Serial.println("Erro: tempo expirado!");
    else Serial.println(TEMPOESGOTADO);

  // Se conectou à rede
  } else {
    status = true;

    // Inicia o servidor
    server.begin();

    // Salva e o endereço IP
    IP = WiFi.localIP().toString();
    
    if (!reconectar) {
      
      Serial.println(" WiFi conectado!");
      
      Serial.print("Iniciando o servidor... ");
      
      Serial.print("Servidor iniciado! Online no IP: ");
      
      Serial.println(IP);
    
    } else
      Serial.println(OK);
  }
}

// Função que executa na primeira vez em que o ESP8266 for iniciado
void setup() {

  // Inicia a comunicação serial
  Serial.begin(9600);

  // Aguarda 10 ms
  delay(10);
  
  Serial.println("Iniciando o ESP8266...");
  Serial.println();
  
  // Conecta à rede WiFi
  Conectar(ssid, pass, false);

  Serial.println("\n> Aceitando comandos via Serial");
}

// Função que executa em loop no ESP8266
void loop() {
  
  // Se recebeu algum comando via serial
  if (Serial.available() > 0) {

    // Faz a leitura do comando recebido
    String comando = Serial.readString();
    // Remove caracteres como \t ou \n
    comando.trim();
    
    // Imprime o comando recebido
    //Serial.print("-- Comando recebido: ");
    //Serial.print(comando);
    //Serial.print("... ");

    // Separa o comando do seu valor
    int separador = comando.indexOf(':');

    // Se o comando é inválido
    if (separador < 0) {
      Serial.println(INVALIDO);
    
    // Se o comando é válido
    } else {
      // Separa em comando e valor
      String cmd = comando.substring(0, separador);
      String vlr = comando.substring(separador+1, comando.length());

      // Se é comando para exibir o status do servidor
      if (cmd.equalsIgnoreCase("status")) {
        //Serial.println("Comando executado!");
        if (status) Serial.println(OK);
        else Serial.println(ERRO);
      
      // Se é comando para reiniciar
      } else if (cmd.equalsIgnoreCase("reiniciar")) {
        Serial.println(EXECUTADO);
        Reiniciar();

      // Se é comando para alterar o SSID da rede wifi
      } else if (cmd.equalsIgnoreCase("ssid")) {
        ssid = vlr;
        Serial.println(EXECUTADO);

      // Se é comando para alterar a senha da rede wifi
      } else if (cmd.equalsIgnoreCase("pass")) {
        pass = vlr;
        Serial.println(EXECUTADO);

      // Se é comando para reconectar à rede wifi
      } else if (cmd.equalsIgnoreCase("reconectar")) {
        Serial.println(EXECUTADO);
        Conectar(ssid, pass, true);
        delay(5 * 1000); // Aguarda 5 segundos antes de prosseguir

      // Se é comando para obter o SSID do WiFi
      } else if (cmd.equalsIgnoreCase("getssid")) {
        Serial.println(ssid);
      
      // Se é comando para obter o endereço IP do servidor
      } else if (cmd.equalsIgnoreCase("getip")) {
        IP = WiFi.localIP().toString();
        Serial.println(IP);

      // Se é comando para obter a porta do servidor
      } else if (cmd.equalsIgnoreCase("getporta")) {
        Serial.println(porta);
      
      // Se é o comando para definir a temperatura em Célsius
      } else if (cmd.equalsIgnoreCase("temp_c")) {
        temp_c = vlr.toFloat();
        Serial.println(EXECUTADO);

      // Se é o comando para definir a temperatura em Fahrenheit
      } else if (cmd.equalsIgnoreCase("temp_f")) {
        temp_f = vlr.toFloat();
        Serial.println(EXECUTADO);
      
      // Se é o comando para definir a umidade
      } else if (cmd.equalsIgnoreCase("umidade")) {
        umidade = vlr.toFloat();
        Serial.println(EXECUTADO);

      // Se é o comando para definir o índice de calor
      } else if (cmd.equalsIgnoreCase("indice")) {
        indiceCalor = vlr.toFloat();
        Serial.println(EXECUTADO);
      
      // Algum comando inválido
      } else
        Serial.println(INVALIDO);
    }
    
  // Se não recebeu nenhum comando via serial
  } else {

    // Se está conectado à uma rede wifi
    if (WiFi.isConnected()) {

      // Inicia um cliente web
      WiFiClient client = server.available();

      // Imprime o resultado em XML
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/xml");
      client.println("Connection: close");  // the connection will be closed after completion of the response
      client.println();
      client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
      client.println("<clima>");
      client.println("\t<temperatura>");
      client.print("\t\t<celsius>");
      client.print(temp_c);
      client.println("</celsius>");
      client.print("\t\t<fahrenheit>");
      client.print(temp_f);
      client.println("</fahrenheit>");
      client.println("\t</temperatura>");
      client.println("\t<umidade>");
      client.print("\t\t<relativa>");
      client.print(umidade);
      client.println("</relativa>");
      client.print("\t\t<indice_calor>");
      client.print(indiceCalor);
      client.println("</indice_calor>");
      client.println("\t</umidade>");
      client.println("</clima>");

      client.stop();

    }

  }

  // Aguarda 50 ms
  delay(50);
  
}


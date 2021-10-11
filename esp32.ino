#if ! (ESP8266 || ESP32 )
  #error This code is intended to run on the ESP8266/ESP32 platform! Please check your Tools->Board setting
#endif
#include <time.h> 
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Credentials.h"
#define MYSQL_DEBUG_PORT      Serial
#define _MYSQL_LOGLEVEL_      1 // Debug Level from 0 to 4
#include <MySQL_Generic.h>
#define USING_HOST_NAME     true
//---------------------------------------------------------------------Time
long timezone = 1; 
byte daysavetime = 1;
//--------------------------------------------------------------------Fin de time
//---------------------------------------------------------------------hostname
#if USING_HOST_NAME
  char server[] = "sql176.main-hosting.eu"; // change to your server's hostname/URL
#else
  IPAddress server(185, 201, 11, 212);
#endif
//---------------------------------------------------------------------fin hostname
//---------------------------------------------------------------------Base de datos, tabla y puerto
uint16_t server_port = 3306;
char default_database[] = "u990593344_FASE1";
char default_table[]    = "REGISTROS";
//---------------------------------------------------------------------fin de Base de datos, tabla y puerto
//---------------------------------------------------------------------Variables para almacenar parametros
String  fec  = "";
String  hor  = "";
String  enc  = "";
String  equ  = "";
String  lim  = "";
String  d1  = "";
String  d2  = "";
String  d3  = "";
String  d4 = "";
String  d5 = "";
String  d6 = "";
String  d7 = "";
String  je = "";
String  des = "";
String INSERT_SQL;
//---------------------------------------------------------------------Fin variables para almacenar parametros
MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;
File historico;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
  }else{
    Serial.println("Iniciada");
  }
}

void loop(){
  if (Serial.available() > 0) {
    interpretIncome();
    delay(50);
  }
}

void interpretIncome(){
  String controlYDato = Serial.readStringUntil(',');
  if (controlYDato == "-E") {
    insertFill();
    Connect();
  }else{
    int separador = controlYDato.indexOf('-');
    char control = controlYDato.charAt(separador - 1);
    String dato = controlYDato;
    switch (control) {
      case 'A':
        dato.replace("A-", "");
        fec = dato ;
        break;
      case 'B':
        dato.replace("B-", "");
        hor = dato;
        break;
      case 'C':
        dato.replace("C-", "");
        enc = dato;
        break;
      case 'D':
        dato.replace("D-", "");
        equ = dato;
        break;
      case 'E':
        dato.replace("E-", "");
        lim = dato;
        break;
      case 'F':
        dato.replace("F-", "");
        d1 = dato;
        break;
      case 'G':
        dato.replace("G-", "");
        d2 = dato;
        break;
      case 'H':
        dato.replace("H-", "");
        d3 = dato;
        break;
      case 'I':
        dato.replace("I-", "");
        d4 = dato;
        break;
      case 'J':
        dato.replace("J-", "");
        d5 = dato;
        break;
      case 'K':
        dato.replace("K-", "");
        d6 = dato;
        break;
      case 'L':
        dato.replace("L-", "");
        d7 = dato;
        break;
      case 'M':
        dato.replace("M-", "");
        je = dato;
        break;
      case 'N':
        dato.replace("N-", "");
        des = dato;
        break;
    }
  }
}
//---------------------------------------------------------------------Conectar a MySQL
void Connect(){
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL){ 
    delay(1000);
    Insert();
    conn.close();
    appendFile(SD, "/belen historico.txt", INSERT_SQL + " - Enviado\n");
  } 
  else{
    appendFile(SD, "/belen historico.txt", INSERT_SQL + " - No enviado\n");
  }
  vaciarSerial();
}
//---------------------------------------------------------------------Fin conectar a MySQL
//---------------------------------------------------------------------Ejecutar insert  
void Insert(){
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected()){
    query_mem.execute(INSERT_SQL.c_str());
  }else{
    appendFile(SD, "/belen historico.txt"," - No se pudo insertar\n");
  }
  vaciarSerial();
}
//---------------------------------------------------------------------Fin ejecutar insert
//---------------------------------------------------------------------Insertar en .txt historico
void appendFile(fs::FS &fs, const char * path, String message){
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        return;
    }
    file.print(message);
    file.close();
}
//---------------------------------------------------------------------Fin insertar en .txt historico
void vaciarSerial(){
  while (Serial.available() > 0)  Serial.read(); 
}

void insertFill(){
INSERT_SQL = String("INSERT INTO ") + default_database + "." + default_table 
                 + " (`FECHA`, `HORA`, `ENCARGADO`, `ENDOSCOPIO`, `LP`, `D1`, `D2`, `D3`, `D4`, `D5`, `D6`, `D7`, `PHJE`, `PHDES`, `NOVEDAD`, `SEDE`, `OPCIONAL1`) VALUES ('" + fec + "','" + hor + "','" + enc + "','" + equ + "','" + lim + "','" + d1 + "','" + d2 + "','" + d3 + "','" + d4 + "','" + d5 + "','" + d6 + "','" + d7 + "','" + je + "','" + des + "','" "','SEDE1','" "')";
appendFile(SD, "/belen historico.txt", INSERT_SQL);
}

void vaciarStructure(){
  String  fec = "";
  String  hor = "";
  String  enc = "";
  String  equ = "";
  String  lim = "";
  String  d1 = "";
  String  d2 = "";
  String  d3 = "";
  String  d4 = "";
  String  d5 = "";
  String  d6 = "";
  String  d7 = "";
  String  je = "";
  String  des = "";
}

void timeServer(){
  configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,(tmstruct.tm_hour)-7 , tmstruct.tm_min, tmstruct.tm_sec);
  while (Serial.available() > 0)  Serial.read(); 
}

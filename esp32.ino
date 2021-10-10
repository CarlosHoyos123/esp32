#if ! (ESP8266 || ESP32 )
  #error This code is intended to run on the ESP8266/ESP32 platform! Please check your Tools->Board setting
#endif
#include "Credentials.h"
#define MYSQL_DEBUG_PORT      Serial
#define _MYSQL_LOGLEVEL_      1 // Debug Level from 0 to 4
#include <MySQL_Generic.h>
#define USING_HOST_NAME     true
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
//---------------------------------------------------------------------Fin muestra de insert Basico
String default_value    = "Hello, Arduino!"; 
String INSERT_SQL = String("INSERT INTO ") + default_database + "." + default_table 
                 + " (OPCIONAL1) VALUES ('" + default_value + "')";
//---------------------------------------------------------------------Fin muestra de insert Basico

MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
    
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL)
  {
    Serial.println("CONETADO..................MYSQL");
    delay(1000);
    runInsert();
    conn.close();                     // close the connection
  } 
  else 
  {
    MYSQL_DISPLAY("\nConnect failed. Trying again on next iteration.");
  }
   delay(60000);
}

void runInsert()
{
  MySQL_Query query_mem = MySQL_Query(&conn);

  if (conn.connected()){
    if ( !query_mem.execute(INSERT_SQL.c_str()) ){
      MYSQL_DISPLAY("Insert error");
    }else{
      MYSQL_DISPLAY("Data Inserted.");
    }
  }
  else
  {
    MYSQL_DISPLAY("Disconnected from Server. Can't insert.");
  }
}

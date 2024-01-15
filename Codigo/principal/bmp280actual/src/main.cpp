
/*
// INICIALMENTE COMO VOY A COMUNICARME POR I2C TENGO QUE AVERIGUAR EL DIRECCIÓN DEL SENSOR
// en mi caso me da 0x76 y tengo que ir a la libreria en >.pio>libdeps>Adafruit BMP280 Library>Adafruit_BMP280
// y sustituir 0x77 por 0x76

#include <Arduino.h>
#include <Wire.h>

void setup()
{
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);             // Espera al monitor serie
  Serial.println("\nEscaner I2C");
}

void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Escaneando...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // El escaner i2c utiliza el valor devuelto por la instrucción 
    // Write.endTransmisstion para reconocer la dirección a la que 
    // está conectado cada dispositivo.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("Dispositivo I2C encontrado en la direccion 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Error desconocido en la direccion 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("Ningun dispositivo I2C encontrado\n");
  else
    Serial.println("Hecho\n");

  delay(5000);           // espera 5 segundos para el siguiente escaneo
}
   

 */



#include <Arduino.h>
//#include <Wifi.h> 
#include <HTTPClient.h>      // no es necesario introducirla solo llamarla
#include <WiFiClient.h>
#include <Adafruit_BMP280.h>
#include <DFRobot_SHT20.h>


Adafruit_BMP280 bmp; // I2C
WiFiClient client; 
//COLEGIO
//const char* ssid = "CENTRO";
//const char* password = "";
const char *ssid = "Redmi";
const char *password = "92b06030e426a";
//const char* ssid = "MIWIFI_9E55";
//const char* password = "HHeYKd92";

//FUNCIONES aquí se declaran
void lecturaBMP280();
void envioDatos();


void setup() {
  Serial.begin(115200);
  sht20.initSHT20();
  delay(100);
  Serial.println("Sensor init finish!");
  sht20.checkSHT20();
  Serial.println(F("BMP280 Forced Mode Test."));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500); 

 Serial.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }


}

void loop() {

 lecturaBMP280();
 envioDatos();


}

// aquí van las funciones
void lecturaBMP280(){
    float humd = sht20.readHumidity();

    Serial.print(" Humidity:");
    Serial.print(humd, 1);  
    Serial.print("%");
    Serial.println();
  if (bmp.takeForcedMeasurement()) {      // must call this to wake sensor up and get new measurement data it blocks until measurement is complete
    // can now print out the new measurements

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); 
    Serial.println(" m");

    Serial.println();
    delay(2000);
  } else {
    Serial.println("Forced measurement failed!");
  }
}

void envioDatos(){

if (WiFi.status() == WL_CONNECTED){ 
     HTTPClient http;  // creo el objeto http
     http.begin(client,"http://estacioncastro.000webhostapp.com/EspPost.php");
     http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // defino texto plano..
    
    //String datos_a_enviar = "temperatura=" + String(20) + "&presion=" + String(30);  
    String datos_a_enviar = "temperatura=" + String(bmp.readTemperature()) + "&presion=" + String(bmp.readPressure());  

     int codigo_respuesta = http.POST(datos_a_enviar);

     if (codigo_respuesta>0){
      Serial.println("Código HTTP: "+ String(codigo_respuesta));
        if (codigo_respuesta == 200){
          String cuerpo_respuesta = http.getString();
          Serial.println("El servidor respondió: ");
          Serial.println(cuerpo_respuesta);
        }
     } else {
        Serial.print("Error enviado POST, código: ");
        Serial.println(codigo_respuesta);
     }

       http.end();  // libero recursos
       
  } else {
     Serial.println("Error en la conexion WIFI");
  }
  delay(60000); //espera 60s
}

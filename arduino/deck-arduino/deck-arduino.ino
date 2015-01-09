#include "DHT.h"

#include <stdlib.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

#define	DHTPIN	2
#define DHTTYPE	DHT22

#define LOOP_DELAY	60000

#define IDLE_TIMEOUT_MS  30000

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed but DI

#define WLAN_SSID       "flypaper"        // cannot be longer than 32 characters!
#define WLAN_PASS       "IamIthinkIwill"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define REST_SERVER_PORT	9000

DHT dht(DHTPIN, DHTTYPE);



void setup() {
	Serial.begin(115200);
	Serial.println(F("Initializing..."));

	// Serial.println("");
	// Serial.print("Starting DHT sensor...");
	dht.begin();
	// Serial.println("done.");

	// Serial.println("");
	// Serial.println("Initializing the CC3000...");
	if (!cc3000.begin()) {
		Serial.println(F("CC3000 initialization failed"));
	} else {
		// uint16_t firmware = checkCc3000FirmwareVersion();
		// if (firmware < 0x113) {
		// 	Serial.println(F("Wrong firmware version!"));
		// } 
  
  	// displayCc3000MACAddress();

		// Serial.println(F("\nDeleting old connection profiles"));
		if (!cc3000.deleteProfiles()) {
			Serial.println(F("Could not delete old connection profiles!"));
		}

		char *ssid = WLAN_SSID;             /* Max 32 chars */
		Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);

		if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
			Serial.println(F("Failed!"));
		} else {
			Serial.println("Connected!");
		}

		// Serial.println(F("Request DHCP"));
		while (!cc3000.checkDHCP()) {
			delay(100); // ToDo: Insert a DHCP timeout!
		}  

		while (! displayCc3000ConnectionDetails()) {
			delay(1000);
		}

		// Serial.println("Done.");
	}

	Serial.println(F("\r\nInitialization complete\r\n"));
}

void loop() {

	readDht();

  delay(LOOP_DELAY);
}

void readDht() {
	float humidity = dht.readHumidity();

	if (isnan(humidity)) {
		Serial.println(F("Failed to read humidity from DHT22"));
	} else {
		// Serial.print("Humidity: ");
		// Serial.println(humidity);
    sendData(humidity, "percent-humidity", "DHT22");
	}

	float temperatureCelcius = dht.readTemperature();

	if (isnan(temperatureCelcius)) {
		Serial.println(F("Failed to read temperature from DHT22"));
	} else {
		// Serial.print("Temperature: ");
		// Serial.print(temperatureCelcius);
		// Serial.println(" degrees Celcius");
    sendData(temperatureCelcius, "degrees-celcius", "DHT22");
	}
}

/**************************************************************************/
/*!
    @brief  Tries to read the CC3000's internal firmware patch ID
*/
/**************************************************************************/
// uint16_t checkCc3000FirmwareVersion(void)
// {
//   uint8_t major, minor;
//   uint16_t version;
  
// #ifndef CC3000_TINY_DRIVER  
//   if(!cc3000.getFirmwareVersion(&major, &minor))
//   {
//     Serial.println(F("Unable to retrieve the firmware version!\r\n"));
//     version = 0;
//   }
//   else
//   {
//     Serial.print(F("Firmware V. : "));
//     Serial.print(major); Serial.print(F(".")); Serial.println(minor);
//     version = major; version <<= 8; version |= minor;
//   }
// #endif
//   return version;
// }

/**************************************************************************/
/*!
    @brief  Tries to read the 6-byte MAC address of the CC3000 module
*/
/**************************************************************************/
// void displayCc3000MACAddress(void)
// {
//   uint8_t macAddress[6];
  
//   if(!cc3000.getMacAddress(macAddress))
//   {
//     Serial.println(F("Unable to retrieve MAC Address!\r\n"));
//   }
//   else
//   {
//     Serial.print(F("MAC Address : "));
//     cc3000.printHex((byte*)&macAddress, 6);
//   }
// }

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayCc3000ConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}


void sendData(float data, String uom, String sensor) {
  char dataBuffer[10];
  dtostrf(data, 3, 4, dataBuffer);

  sendData(
    "{\"value\":" +
    String(dataBuffer) +
    ",\"uom\":\"" +
    uom +
    "\",\"sensor\":\"" + 
    sensor +
    "\",\"source\":\"deck-arduino\"}");
}

void sendData(String msg) {

  Serial.println(msg);
  Serial.println();

	uint32_t ip = 0;

	ip = cc3000.IP2U32(192, 168, 1, 65);

	// Serial.print(F("\n\rConnecting... ")); cc3000.printIPdotsRev(ip); Serial.print("...");  

	Adafruit_CC3000_Client www = cc3000.connectTCP(ip, REST_SERVER_PORT);
  if (www.connected()) {
    // headers
    www.fastrprint(F("POST ")); www.fastrprint(F("/1/reading")); www.fastrprint(F(" HTTP/1.1\r\n"));
    www.fastrprint(F("Host: firefly:9000\r\n"));
    www.fastrprint(F("Content-Type: application/json\r\n"));
    www.fastrprint(F("Cache-Control: no-cache\r\n"));
    www.fastrprint(F("Content-Length: "));
    www.print(msg.length());
    www.fastrprint(F("\r\n"));
    www.fastrprint(F("Connection: close\r\n"));
    www.fastrprint(F("\r\n"));

    // body
    www.print(msg);
    www.fastrprint(F("\r\n"));
    www.println();   
    
    Serial.println();
  } else {
    Serial.println(F("\r\nConnection failed\r\n"));    
    return;
  }
  
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      Serial.print(c);
      lastRead = millis();
    }
  }
  
  // Serial.println(F("\r\n-------------------------------------\r\n\r\n"));

  
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  www.close();
  // Serial.println(F("-------------------------------------"));
  // Serial.println(F("\n\nDisconnecting"));
  // cc3000.disconnect();
}
//28358
//26186
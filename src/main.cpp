/***********************************************************************/

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define API_KEY "YOUR_FIREBASE_API_KEY"
#define USER_EMAIL "YOUR_EMAIL"
#define USER_PASSWORD "YOUR_PASSWORD""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/***********************************************************************/

// Libraries
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ISR Servo Library
#define TIMER_INTERRUPT_DEBUG  1
#define ISR_SERVO_DEBUG        1

#include <ESP8266_ISR_Servo.h>

#define SERVO_PIN D5

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo Variables
int servoIndex = -1;

/***********************************************************************/

void Connect_WiFi();

void Firebase_Store(String PATH, String MSG);

String Firebase_getString(String PATH);

/***********************************************************************/
void setup()
{
    Serial.begin(9600);

    // I2C Start
    Wire.begin();

    // LCD Start
    lcd.init();
    lcd.backlight();

    // Connect WiFi + Firebase
    Connect_WiFi();

    // Attach Servo
    servoIndex = ISR_Servo.setupServo(SERVO_PIN);

    if (servoIndex != -1)
    {
        Serial.println("Servo Attached");
    }

    // Initially Gate Closed
    ISR_Servo.setPosition(servoIndex, 0);
}

/***********************************************************************/
void loop()
{
    String emptySlots =
    Firebase_getString("/parking/empty_slots");

    Serial.println(emptySlots);

    int slots = emptySlots.toInt();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Empty Slots:");

    lcd.setCursor(13, 0);
    lcd.print(slots);

    // Gate Logic
    if (slots > 0)
    {
        // Open Gate
        ISR_Servo.setPosition(servoIndex, 90);

        Serial.println("Gate Open");

        lcd.setCursor(0, 1);
        lcd.print("Status: OPEN ");
    }
    else
    {
        // Close Gate
        ISR_Servo.setPosition(servoIndex, 0);

        Serial.println("Gate Closed");

        lcd.setCursor(0, 1);
        lcd.print("Status: FULL ");
    }

    delay(2000);
}

/***********************************************************************/
void Connect_WiFi()
{
    delay(100);

    WiFi.disconnect();

    delay(800);

    Serial.println("Connecting to Wi-Fi");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

    config.api_key = API_KEY;

    auth.user.email = USER_EMAIL;

    auth.user.password = USER_PASSWORD;

    config.database_url = DATABASE_URL;

    config.token_status_callback = tokenStatusCallback;

    #if defined(ESP8266)

    fbdo.setBSSLBufferSize(2048, 2048);

    #endif

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);

    Firebase.setDoubleDigits(5);

    config.timeout.serverResponse = 10 * 1000;
}

/***********************************************************************/
void Firebase_Store(String PATH, String MSG)
{
    Serial.print("Uploading data \" ");

    Serial.print(MSG);

    Serial.print(" \" to the location \" ");

    Serial.print(PATH);

    Serial.println(" \"");

    Firebase.RTDB.setString(&fbdo, PATH, MSG);

    delay(50);
}

/***********************************************************************/
String Firebase_getString(String PATH)
{
    String msg =
    (
        Firebase.RTDB.getString(&fbdo, PATH)
        ?
        fbdo.to<const char *>()
        :
        fbdo.errorReason().c_str()
    );

    delay(50);

    return msg;
}

/***********************************************************************/
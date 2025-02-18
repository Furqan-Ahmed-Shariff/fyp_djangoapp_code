//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ESP32_CAM_Send_Photo_to_Server
//======================================== Including the libraries.
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <ESP32Servo.h>
//======================================== 

//======================================== CAMERA_MODEL_AI_THINKER GPIO.
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
//======================================== 

// LED Flash PIN (GPIO 4)
#define FLASH_LED_PIN 4             

//======================================== Insert your network credentials.
const char* ssid = "ULTIMATUM";
const char* password = "mahin10furq";
const char* ssid_alt = "moto g82 5G_5548";
const char* password_alt = "luqman@123";
//======================================== 

#define trigPin 14
#define echoPin 15
#define servoPin 13 // GPIO pin for servo control

long distance, duration;

Servo myServo;

//======================================== Variables for Timer/Millis.
unsigned long previousMillis = 0; 
const int Interval = 2000; //--> Photo capture every 20 seconds.
//======================================== 

// Server Address or Server IP.
String serverName = "117.216.169.90";  //--> Change with your server computer's IP address or your Domain name.
// The file path "upload_img.php" on the server folder.
// String serverPath = "/ESP32CAM/upload_img.php";
String serverPath = "/";
// Server Port.
const int serverPort = 80;

// Variable to set capture photo with LED Flash.
// Set to "false", then the Flash LED will not light up when capturing a photo.
// Set to "true", then the Flash LED lights up when capturing a photo.
bool LED_Flash_ON = true;

// Initialize WiFiClient.
WiFiClient client;

//________________________________________________________________________________ sendPhotoToServer()
String sendPhotoToServer() {
  String AllData;
  String DataBody;

  Serial.println();
  Serial.println("-----------");
 
  //---------------------------------------- Pre capture for accurate timing.
  Serial.println("Taking a photo...");

  if (LED_Flash_ON == true) {
    digitalWrite(FLASH_LED_PIN, HIGH);
    delay(1000);
  }
  
  for (int i = 0; i <= 3; i++) {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
     if(!fb) {
        Serial.println("Camera capture failed");
        Serial.println("Restarting the ESP32 CAM.");
        delay(1000);
        ESP.restart();
        return "Camera capture failed. Restarting the ESP32 CAM.";
      } 
    esp_camera_fb_return(fb);
    delay(200);
  }
  
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    Serial.println("Restarting the ESP32 CAM.");
    delay(1000);
    ESP.restart();
    return "Camera capture failed. Restarting the ESP32 CAM.";
  } 

  if (LED_Flash_ON == true) digitalWrite(FLASH_LED_PIN, LOW);
  
  Serial.println("Taking a photo was successful.");
  //---------------------------------------- 

  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");   
     
    String post_data = "--dataMarker\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"ESP32CAMCap.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String head =  post_data;
    String boundary = "\r\n--dataMarker--\r\n";
    
    uint32_t imageLen = fb->len;
    uint32_t dataLen = head.length() + boundary.length();
    uint32_t totalLen = imageLen + dataLen;
    String apiKey = "a37eb3f33f2d3c80f6076646a9b6d62dcc9cfffa";
    
    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=dataMarker");
    client.println("Authorization: Token " + apiKey);
    client.println();
    client.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(boundary);
    
    esp_camera_fb_return(fb);
   
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    Serial.println("Response : ");
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(200);
         
      // Skip HTTP headers   
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (AllData.length()==0) { state=true; }
          AllData = "";
        }
        else if (c != '\r') { AllData += String(c); }
        if (state==true) { DataBody += String(c); }
        startTimer = millis();
      }
      if (DataBody.length()>0) { break; }
    }
    client.stop();
    Serial.println(DataBody);
    Serial.println("-----------");
    Serial.println();
    return DataBody;
    
  }
  else {
    client.stop();
    DataBody = "Connection to " + serverName +  " failed.";
    Serial.println(DataBody);
    Serial.println("-----------");
  }
}

//________________________________________________________________________________
int connectToWifi(const char* ssid, const char* password) {
  // Setting the ESP32 WiFi to station mode.
  WiFi.mode(WIFI_STA);
  Serial.println();
  
  Serial.println();
  Serial.print("Connecting to : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // The process timeout of connecting ESP32 CAM with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 CAM has not been successfully connected to WiFi, the ESP32 CAM will restart.
  // I made this condition because on my ESP32-CAM, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      Serial.println();
      Serial.print("Failed to connect to ");
      Serial.println(ssid);
      delay(1000);
      return 0;
    }
  }

  Serial.println();
  Serial.print("Successfully connected to ");
  Serial.println(ssid);
  return 1;
}

//________________________________________________________________________________ 

void rotateServo(int direction) {
  if (direction == 0) {
    Serial.println("Rotating Clockwise");
    myServo.write(0); // Rotate 90 degrees clockwise
    delay(2000);       // Wait for 2 seconds
    myServo.write(90);  // Rotate back to 0 degrees
    Serial.println("Rotating Done");
  } else {
    myServo.write(180); // Rotate 90 degrees anticlockwise
    delay(2000);        // Wait for 2 seconds
    myServo.write(90);   // Rotate back to 0 degrees
  }
}

//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  // Disable brownout detector.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  Serial.println();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(FLASH_LED_PIN, OUTPUT);

  myServo.attach(servoPin); // Attach the servo to the GPIO pin
  myServo.write(90); // Ensure servo starts at 0 degrees

  //---------------------------------------- The process of connecting ESP32 CAM with WiFi Hotspot / WiFi Router.
  int connected_to_wifi = connectToWifi(ssid, password);
  if(!connected_to_wifi) {
    int connected_to_wifi_alt = connectToWifi(ssid_alt, password_alt);
    if(!connected_to_wifi_alt) {
      Serial.println("Restarting the ESP32 CAM.");
      ESP.restart();
    }
  }

  //---------------------------------------- Set the camera ESP32 CAM.
  Serial.println();
  Serial.print("Set the camera ESP32 CAM...");
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //--> 0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 8;  //--> 0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    ESP.restart();
  }
  Serial.println("Camera initialized successfully.");
}

//________________________________________________________________________________ VOID LOOP()
void loop() {
  // Calculate the distance using the ultrasonic sensor.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 10) {
    Serial.print("Distance: ");
    Serial.println(distance);
    String response = sendPhotoToServer();
    Serial.println("Photo sent. Server response: " + response);

    Serial.print("Index: ");
    Serial.println(response.indexOf("left"));

    // Rotate servo based on a condition (e.g., left or right based on response data).
    if (response.indexOf("left") != -1) {
      rotateServo(0); // Rotate clockwise
    } else if (response.indexOf("right") != -1) {
      rotateServo(1); // Rotate anticlockwise
    }
  }

  delay(Interval);
}
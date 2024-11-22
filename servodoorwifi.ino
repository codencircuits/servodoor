#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "Brav";
const char* password = "abcd0987";

// Pin definition
const int servoPin = 33;

// Constants
const int SERVO_OPEN_ANGLE = 90;    // Angle when door is open
const int SERVO_CLOSED_ANGLE = 0;   // Angle when door is closed

// Initialize objects
AsyncWebServer server(80);
Servo myServo;

// State variable
volatile bool isDoorOpen = false;

void setup() {
    Serial.begin(115200);
    
    // Initialize servo in closed position
    myServo.attach(servoPin);
    myServo.write(SERVO_CLOSED_ANGLE);
    isDoorOpen = false;
    Serial.println("Servo initialized to closed position");
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Serve the web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Door Control</title>
                <meta name="viewport" content="width=device-width, initial-scale=1">
                <style>
                    body { 
                        font-family: Arial, sans-serif; 
                        margin: 20px;
                        text-align: center;
                    }
                    .container { 
                        max-width: 600px; 
                        margin: 0 auto;
                        padding: 20px;
                        box-shadow: 0 0 10px rgba(0,0,0,0.1);
                        border-radius: 10px;
                    }
                    .status {
                        padding: 20px;
                        margin: 20px 0;
                        border-radius: 5px;
                        font-size: 1.2em;
                        font-weight: bold;
                    }
                    .open { 
                        background-color: #d4edda; 
                        color: #155724; 
                    }
                    .closed { 
                        background-color: #f8f9fa; 
                        color: #383d41; 
                    }
                    .button {
                        padding: 15px 30px;
                        font-size: 1.2em;
                        margin: 10px;
                        border: none;
                        border-radius: 5px;
                        cursor: pointer;
                        transition: background-color 0.3s;
                    }
                    .open-btn {
                        background-color: #28a745;
                        color: white;
                    }
                    .close-btn {
                        background-color: #dc3545;
                        color: white;
                    }
                    .button:hover {
                        opacity: 0.9;
                    }
                    .button:disabled {
                        opacity: 0.5;
                        cursor: not-allowed;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>Automatic Door Control</h1>
                    <div id="doorStatus" class="status closed">Door Status: Closed</div>
                    <div>
                        <button id="openBtn" class="button open-btn" onclick="controlDoor('open')">Open Door</button>
                        <button id="closeBtn" class="button close-btn" onclick="controlDoor('close')">Close Door</button>
                    </div>
                </div>
                <script>
                    let isProcessing = false;
                    
                    function updateButtonStates(doorState) {
                        const openBtn = document.getElementById('openBtn');
                        const closeBtn = document.getElementById('closeBtn');
                        openBtn.disabled = doorState === 'open' || isProcessing;
                        closeBtn.disabled = doorState === 'closed' || isProcessing;
                    }
                    
                    function updateStatus() {
                        fetch('/status')
                            .then(response => response.json())
                            .then(data => {
                                const status = data.isDoorOpen ? 'Open' : 'Closed';
                                const statusDiv = document.getElementById('doorStatus');
                                statusDiv.innerText = 'Door Status: ' + status;
                                statusDiv.className = 'status ' + (data.isDoorOpen ? 'open' : 'closed');
                                updateButtonStates(status.toLowerCase());
                            });
                    }
                    
                    function controlDoor(action) {
                        if (isProcessing) return;
                        
                        isProcessing = true;
                        updateButtonStates();
                        
                        fetch('/' + action)
                            .then(response => response.text())
                            .then(data => {
                                console.log(data);
                                setTimeout(() => {
                                    isProcessing = false;
                                    updateStatus();
                                }, 1000);
                            })
                            .catch(error => {
                                console.error('Error:', error);
                                isProcessing = false;
                                updateStatus();
                            });
                    }
                    
                    // Update status every second
                    setInterval(updateStatus, 1000);
                    
                    // Initial status update
                    updateStatus();
                </script>
            </body>
            </html>
        )rawliteral";
        request->send(200, "text/html", html);
    });
    
    // Handle open door request
    server.on("/open", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (!isDoorOpen) {
            myServo.write(SERVO_OPEN_ANGLE);
            isDoorOpen = true;
            Serial.println("Door opened");
        }
        request->send(200, "text/plain", "Door opened");
    });
    
    // Handle close door request
    server.on("/close", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (isDoorOpen) {
            myServo.write(SERVO_CLOSED_ANGLE);
            isDoorOpen = false;
            Serial.println("Door closed");
        }
        request->send(200, "text/plain", "Door closed");
    });
    
    // Status endpoint
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{\"isDoorOpen\":" + String(isDoorOpen ? "true" : "false") + "}";
        request->send(200, "application/json", json);
    });
    
    server.begin();
}

void loop() {
    // Main loop is empty as everything is handled by the web server
    delay(100);
}

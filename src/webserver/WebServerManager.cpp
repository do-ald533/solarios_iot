#include "WebServerManager.h"

// HTML, CSS, and JS bundle stored directly in ESP32 Flash memory
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Solaris IoT - Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #121212; color: #fff; margin: 0; padding: 20px; }
        .card { background: #1e1e1e; padding: 20px; border-radius: 12px; margin: 15px auto; max-width: 400px; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
        .status { font-size: 24px; font-weight: bold; padding: 10px; border-radius: 6px; }
        .occupied { background: #e53935; color: white; }
        .free { background: #43a047; color: white; }
        .metric { font-size: 36px; color: #00bcd4; margin: 10px 0; }
    </style>
</head>
<body>
    <h2>Solaris IoT Control Panel</h2>
    <div class="card">
        <h3>Occupancy Status</h3>
        <div id="status" class="status free">VACANT</div>
    </div>
    <div class="card">
        <h3>Daily Visits</h3>
        <div id="visits" class="metric">0</div>
    </div>
    <div class="card">
        <h3>Last Motion Detected</h3>
        <p id="lastEntry">None</p>
    </div>

    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;

        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onmessage = onMessage;
            websocket.onclose = function() { setTimeout(initWebSocket, 2000); };
        }

        function onMessage(event) {
            var data = JSON.parse(event.data);
            var statusDiv = document.getElementById('status');
            
            if (data.occupied) {
                statusDiv.className = 'status occupied';
                statusDiv.innerText = 'OCCUPIED';
            } else {
                statusDiv.className = 'status free';
                statusDiv.innerText = 'VACANT';
            }

            document.getElementById('visits').innerText = data.visits;
            if(data.lastEntry) document.getElementById('lastEntry').innerText = data.lastEntry;
        }

        window.addEventListener('load', initWebSocket);
    </script>
</body>
</html>
)rawliteral";

WebServerManager::WebServerManager(uint16_t port)
    : _server(port), _ws("/ws") {}

void WebServerManager::begin()
{
    setupRoutes();
    _server.addHandler(&_ws);
    _server.begin();
    Serial.println("[WebServer] Local Async WebServer started on port 80.");
}

void WebServerManager::setupRoutes()
{
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send_P(200, "text/html", INDEX_HTML); });
}

void WebServerManager::broadcastStatus(bool isOccupied, uint32_t visitCount, const String &lastEntry)
{
    String json = "{\"occupied\":" + String(isOccupied ? "true" : "false") +
                  ",\"visits\":" + String(visitCount) +
                  ",\"lastEntry\":\"" + lastEntry + "\"}";
    _ws.textAll(json);
}
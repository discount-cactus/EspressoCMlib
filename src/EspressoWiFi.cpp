#include "EspressoWiFi.h"

/// Constructor
EspressoWiFi::EspressoWiFi(uint16_t port)
    : _server(nullptr), _port(port), _mode(ESPRESSO_WIFI_MODE_STA), _initialized(false) {
}

/// Begins Wi-Fi and server in specified mode
void EspressoWiFi::begin(uint8_t mode, const char* ssid, const char* password) {
    _mode = mode;
    _ssid = ssid;
    _password = password;

    Serial.println(F("[EspressoWiFi] Starting..."));

    if (_mode == ESPRESSO_WIFI_MODE_STA) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid.c_str(), _password.c_str());
        Serial.printf("[EspressoWiFi] Connecting to SSID: %s\n", _ssid.c_str());

        uint8_t attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 30) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print(F("[EspressoWiFi] Connected! IP: "));
            Serial.println(WiFi.localIP());
        } else {
            Serial.println(F("[EspressoWiFi] Failed to connect."));
            return;
        }
    }
    else if (_mode == ESPRESSO_WIFI_MODE_AP) {
        WiFi.mode(WIFI_AP);
        bool success = WiFi.softAP(_ssid.c_str(), _password.c_str());
        if (success) {
            Serial.printf("[EspressoWiFi] Access Point created: %s\n", _ssid.c_str());
            Serial.print(F("[EspressoWiFi] AP IP: "));
            Serial.println(WiFi.softAPIP());
        } else {
            Serial.println(F("[EspressoWiFi] Failed to create Access Point."));
            return;
        }
    }
    else if (_mode == ESPRESSO_WIFI_MODE_AP_STA) {
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(_ssid.c_str(), _password.c_str());
        WiFi.softAP((_ssid + "_AP").c_str(), _password.c_str());
        Serial.printf("[EspressoWiFi] Dual mode active (AP+STA). STA SSID: %s\n", _ssid.c_str());
        Serial.print(F("[EspressoWiFi] STA IP: "));
        Serial.println(WiFi.localIP());
        Serial.print(F("[EspressoWiFi] AP IP: "));
        Serial.println(WiFi.softAPIP());
    }

    _server = new WebServer(_port);
    _server->on("/", [this]() {
        _server->send(200, "text/html", _html);
    });

    _server->begin();
    _initialized = true;
    Serial.printf("[EspressoWiFi] Server started on port %d\n", _port);
}

/// Stops Wi-Fi and web server
void EspressoWiFi::end() {
    if (!_initialized) return;

    Serial.println(F("[EspressoWiFi] Stopping server..."));
    _server->stop();
    delete _server;
    _server = nullptr;

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    _initialized = false;
    Serial.println(F("[EspressoWiFi] Server stopped and Wi-Fi disconnected."));
}

/// Handles incoming HTTP requests
void EspressoWiFi::handleClient() {
    if (_initialized && _server) {
        _server->handleClient();
    }
}

/// Sets base HTML content
void EspressoWiFi::setHTML(const String& html) {
    _html = html;
    if (_server) {
        _server->on("/", [this]() {
            _server->send(200, "text/html", _html);
        });
    }
}

/// Replaces existing HTML content completely
void EspressoWiFi::updateHTML(const String& newHtml) {
    _html = newHtml;

    // Re-attach the root route to serve the updated content
    if (_server) {
        _server->on("/", [this]() {
            _server->send(200, "text/html", _html);
        });
    }
}

/// Appends additional HTML content to the existing page
void EspressoWiFi::appendHTML(const String& htmlSegment) {
    _html += htmlSegment;

    // Re-attach the root route to serve the updated content
    if (_server) {
        _server->on("/", [this]() {
            _server->send(200, "text/html", _html);
        });
    }
}

/// Adds a new route with a handler
void EspressoWiFi::addRoute(const String& path, std::function<void()> handler) {
    if (_server) {
        _server->on(path.c_str(), handler);
    }
}

/// Removes a previously added route
/*void EspressoWiFi::removeRoute(const String& path) {
    if (_server) {
        _server->removeHandler(path.c_str());
    }
}*/

/// Returns true if Wi-Fi is connected (STA) or if AP is active
bool EspressoWiFi::isConnected() const {
    if (_mode == ESPRESSO_WIFI_MODE_AP) {
        return WiFi.softAPgetStationNum() > 0; // true if at least one client connected to AP
    } else {
        return WiFi.status() == WL_CONNECTED;
    }
}

/// Returns IP address (STA or AP)
IPAddress EspressoWiFi::getIPAddress(bool apInterface) const {
    if (_mode == ESPRESSO_WIFI_MODE_AP && !apInterface) {
        return IPAddress(); // empty if not connected via STA
    } else if (_mode == ESPRESSO_WIFI_MODE_STA && apInterface) {
        return IPAddress(); // no AP interface in STA-only mode
    }

    if (apInterface) {
        return WiFi.softAPIP();
    } else {
        return WiFi.localIP();
    }
}

/// Returns port number
uint16_t EspressoWiFi::getPort() const {
    return _port;
}

/// Returns SSID
String EspressoWiFi::getSSID() const {
    return _ssid;
}

/// Returns HTML content
String EspressoWiFi::getHTML() const {
    return _html;
}

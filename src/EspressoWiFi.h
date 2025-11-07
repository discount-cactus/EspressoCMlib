#ifndef ESPRESSO_WIFI_H
#define ESPRESSO_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define ESPRESSO_WIFI_MODE_STA     0   // Connect to an existing Wi-Fi network
#define ESPRESSO_WIFI_MODE_AP      1   // Create a local access point
#define ESPRESSO_WIFI_MODE_AP_STA  2   // Connect and host simultaneously

class EspressoWiFi {
public:
    EspressoWiFi(uint16_t port = 80);
    void begin(uint8_t mode, const char* ssid, const char* password);
    void end();
    void handleClient();
    void setHTML(const String& html);
    void updateHTML(const String& newHtml);
    void appendHTML(const String& htmlSegment);
    void addRoute(const String& path, std::function<void()> handler);
    //void removeRoute(const String& path);
    bool isConnected() const;
    IPAddress getIPAddress(bool apInterface = false) const;
    uint16_t getPort() const;
    String getSSID() const;
    String getHTML() const;

private:
    WebServer* _server;      // Web server instance
    uint16_t _port;          // Listening port number
    String _ssid;            // SSID for connection or AP
    String _password;        // Wi-Fi password
    String _html;            // HTML content served
    uint8_t _mode;           // Active Wi-Fi mode
    bool _initialized;       // True if server is running
};

#endif // ESPRESSO_WIFI_H

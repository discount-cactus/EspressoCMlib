//Espresso Wi-Fi Test
/*NOTES:
In Station (STA) Mode, connect to existing Wi-Fi
In Access Point (AP) Mode, the ESP32 creates its own Wi-Fi network
In Dual (AP + STA) Mode, connect and host at the same time
*/
#include <EspressoWiFi.h>

EspressoWiFi server(80);

void setup() {
    Serial.begin(115200);
    delay(1000);
    //server.begin(ESPRESSO_WIFI_MODE_STA, "MyHomeWiFi", "mypassword");     //Station Mode
    server.begin(ESPRESSO_WIFI_MODE_AP, "Espresso_Server", "12345678");   //Access Point Mode
    //server.begin(ESPRESSO_WIFI_MODE_AP_STA, "MyWiFi", "mypassword");      //Dual Mode

    // Set a simple HTML page
    server.setHTML("<html><body><h1>Welcome to EspressoWiFi!</h1><p>Server is running.</p></body></html>");

    // Print server info
    Serial.println(" ");
    Serial.println("=== Server Info ===");
    Serial.print("Connected? ");
    Serial.println(server.isConnected() ? "Yes" : "No");
    Serial.print("STA IP: ");
    Serial.println(server.getIPAddress(false));
    Serial.print("AP IP: ");
    Serial.println(server.getIPAddress(true));
    Serial.print("Port: ");
    Serial.println(server.getPort());
    Serial.print("SSID: ");
    Serial.println(server.getSSID());
    Serial.print("HTML Content: ");
    Serial.println(server.getHTML());
    Serial.println("===================");
}

void loop() {
    server.handleClient();
}

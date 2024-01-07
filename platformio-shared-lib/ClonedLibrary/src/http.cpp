#include "http.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "encoding.h"
#include "jsonnode.h"
#include "wi_fi.h"

HttpServer::HttpServer(int port, std::function<String(HttpRequest *)> callback)
{
    wifiServer = WiFiServer(port);
    requestHandle = callback;
}

void HttpServer::begin()
{
    waitForWifi();
    wifiServer.begin();
}

void HttpServer::step()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("awaiting wifi");
        waitForWifi();
    }
    WiFiClient client = wifiServer.available();
    if (client)
    {
        Serial.println("New Client.");
        currentTime = millis();
        previousTime = currentTime;

        parseRequest(&client);
    }
}

void HttpServer::parseRequest(WiFiClient *client)
{
    String currentLine = "";
    String header = "";
    while (client->connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
        currentTime = millis();
        if (client->available())
        {
            char c = client->read();
            header += c;
            if (c == '\n')
            {
                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:
                if (currentLine.length() == 0)
                {
                    HttpRequest request = HttpRequest(header, client, requestHandle);
                    if (request.asyncSend)
                    {
                        Serial.println("keeping async open");
                        return;
                    }
                    break;
                }
                else
                { // if you got a newline, then clear currentLine
                    currentLine = "";
                }
            }
            else if (c != '\r')
            {                     // if you got anything else but a carriage return character,
                currentLine += c; // add it to the end of the currentLine
            }
        }
    }

    header = "";
    // Close the connection
    Serial.println("Client disconnected.");
    Serial.println();
}
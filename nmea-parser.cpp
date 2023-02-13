#include "Arduino.h"
float lat;
float lon;
float hdop;
int sats;
bool gpsOk;
float err;
char *strings[16];
char *token = NULL;

void setup() {
    Serial2.begin(9600);  // terminal
    Serial1.begin(9600);  // gps sensor
    delay(100);
    Serial1.print("****"); // set gps sensor
    delay(100);
    while (Serial1.available()) Serial2.println(Serial1.read());
    delay(100);
    Serial1.print("*****"); // set gps sensor
    delay(100);
    while (Serial1.available()) Serial2.println(Serial1.read());
    delay(1000);
}

char receivedChars[85];

void loop() { rxGPS(); }
int ndx = 0;
void rxGPS() {
    char rc;
    while (Serial1.available()) {
        rc = Serial1.read();
        if (rc == '$') {
            ndx = 0;
        }
        if (ndx < 84) {
            receivedChars[ndx] = rc;
            ndx++;
        } else
            ndx = 0;
        receivedChars[ndx + 1] = '\0';
        if (strncmp(receivedChars, "$", 1) == 0) {
            if (strstr(receivedChars, "\r\n\0")) {
                if ((strncmp(receivedChars, "$GPGGA", 6)) == 0) {
                    getGPSdata(receivedChars, 1);
                }
                if ((strncmp(receivedChars, "$GPGST", 6)) == 0) {
                    getGPSdata(receivedChars, 2);
                }
                ndx = 0;
            }
        }
    }
}

void getGPSdata(char rx[], int message) {
    byte idx = 0;
    int ftd;
    float ntd;
    float r;
    *strchr(rx, '*') = ',';
    while (token = strsep(&rx, ",")) {
        strings[idx] = token;
        idx++;
    }
    switch (message) {
    case 1:
        gpsOk = atoi(strings[6]);
        lat = atof(strings[2]);
        if (*strings[3] == 'S') lat = -lat;
        lon = atof(strings[4]);
        if (*strings[5] == 'W') lon = -lon;
        sats = atoi(strings[7]);
        hdop = atof(strings[8]);
        ftd = ((int)lat) / 100;  // This assumes that f < 10000.
        ntd = lat - (float)(ftd * 100);
        r = (float)(ftd + ntd / 60.0);
        lat = r;
        ftd = ((int)lon) / 100;  // This assumes that f < 10000.
        ntd = lon - (float)(ftd * 100);
        r = (float)(ftd + ntd / 60.0);
        lon = r;
        Serial2.print("hdop = ");
        Serial2.print(hdop);  
        Serial2.print(" sats:");
        Serial2.println(sats);  
        Serial2.print("lat:");
        Serial2.print(lat, 6);
        Serial2.print(" lon:");
        Serial2.println(lon, 6);
        Serial2.print("gpsOK:");
        Serial2.print(gpsOk);  
        break;
    case 2:
        err = max(atof(strings[6]), atof(strings[7]));
        Serial2.print(" err: ");
        Serial2.println(err);  
        break;
    default:
        break;
    }
}

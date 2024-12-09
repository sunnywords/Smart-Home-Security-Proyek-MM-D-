#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>

using namespace std;

// Headers
string toString (double);
int toInt (string);
double toDouble (string);

int main() {
    bool pIR;
    string rFID;

    cout << "LCD = Smart Home Security, Home secured" << endl;
    while (pIR == 1) {
        if (rFID == "7361030") {
            cout << "LCD = Welcome" << endl;
            cout << "LED hijau menyala selama PIR mendeteksi adanya gerakan" << endl;
            cout << "RELAY ON = solenoid terbuka selama 5 detik" << endl;
            cout << "BUZZER OFF" << endl;
            cout << "RELAY OFF" << endl;
            cout << "LCD = Home Secured" << endl;
        } else {
            if ("LCD = Invalid Card") {
                cout << "LED merah menyala" << endl;
                cout << "RELAY OFF" << endl;
                cout << "Solenoid Lock" << endl;
                cout << "Buzzer ON" << endl;
            } else {
                cout << "LCD = Intruder Alert!" << endl;
                cout << "LED merah menyala" << endl;
                cout << "RELAY OFF" << endl;
                cout << "Solenoid Lock" << endl;
                cout << "Buzzer ON" << endl;
            }
        }
    }
    return 0;
}

// The following implements type conversion functions.
string toString (double value) { //int also
    stringstream temp;
    temp << value;
    return temp.str();
}

int toInt (string text) {
    return atoi(text.c_str());
}

double toDouble (string text) {
    return atof(text.c_str());
}

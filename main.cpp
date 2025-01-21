#include "mbed.h"
#include "hpma115/hpma115.h"

using namespace sixtron;

// On trouve ca dans PinNames.h
HPMA115 sensor(PA_9, PA_10);

int main() {
    printf("Initialisation du capteur HPMA115C0\n");


    while (true) {
        hpma115_data_t data;

        
        HPMA115::ErrorType err = sensor.read_measurement(&data);
        if (err == HPMA115::ErrorType::Ok) {
            printf("PM2.5: %d micro g/m³, PM10: %d micro g/m³\n", data.pm2_5, data.pm10);

        
            if (data.pm1_pm4_valid) {
                printf("PM1.0: %d micro g/m³, PM4.0: %d micro g/m³\n", data.pm1_0, data.pm4_0);
            }
        }

        
        ThisThread::sleep_for(2s);
    }

    sensor.stop_measurement();
    return 0;
}
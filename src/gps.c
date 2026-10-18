#include "../include/gps.h"
#include <string.h>
#include <stdlib.h>

void parse_nmea_sentence(const char *sentence, gps_data_t *gps) {
    if (!sentence || !gps) return;

    // Check if the sentence starts with $GPRMC
    if (strncmp(sentence, "$GPRMC", 6) == 0) {
        // Create a local mutable copy of the sentence for strtok
        char temp_line[82];
        strncpy(temp_line, sentence, sizeof(temp_line) - 1);
        temp_line[sizeof(temp_line) - 1] = '\0';

        char *token = strtok(temp_line, ",");
        uint8_t field_index = 0;

        while (token != NULL) {
            switch (field_index) {
                case 2: // Status: 'A' = Active / Valid Fix, 'V' = Void
                    gps->valid = (token[0] == 'A');
                    break;
                case 3: // Latitude string (e.g., 4807.038)
                    if (gps->valid) {
                        strncpy(gps->latitude, token, sizeof(gps->latitude) - 1);
                        gps->latitude[sizeof(gps->latitude) - 1] = '\0';
                    }
                    break;
                case 5: // Longitude string (e.g., 01131.000)
                    if (gps->valid) {
                        strncpy(gps->longitude, token, sizeof(gps->longitude) - 1);
                        gps->longitude[sizeof(gps->longitude) - 1] = '\0';
                    }
                    break;
                default:
                    break;
            }
            token = strtok(NULL, ",");
            field_index++;
        }
    }
}

#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char latitude[12];
    char longitude[12];
    bool valid;
} gps_data_t;

void parse_nmea_sentence(const char *sentence, gps_data_t *gps);

#endif /* GPS_H */

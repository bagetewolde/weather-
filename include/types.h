#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR,
    STATUS_TIMEOUT,
    STATUS_INVALID_DATA,
    STATUS_NOT_READY
} status_t;

typedef struct {
    int32_t temperature_centi_c;
    uint32_t humidity_milli_pct;
    uint32_t pressure_pa;
    bool valid;
    uint32_t timestamp_ms;
} bme280_data_t;

typedef struct {
    float temperature;
    float humidity;
    bool valid;
    uint32_t timestamp_ms;
} dht22_data_t;

typedef struct {
    int32_t latitude_e7;
    int32_t longitude_e7;
    int32_t altitude_mm;
    uint32_t speed_mm_s;
    bool fix_valid;
    uint32_t timestamp_ms;
} gps_data_t;

typedef struct {
    bme280_data_t bme280;
    dht22_data_t dht22;
    gps_data_t gps;
} weather_data_t;

#endif
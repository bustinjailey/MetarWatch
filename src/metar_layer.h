#ifndef METAR_LAYER_H
#define METAR_LAYER_H

#include "graph_layer.h"

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer temp_layer;
	GraphLayer graph_layer;
	uint8_t* forecast;
	uint8_t forecast_count;
	bool has_metar;
	char metar_str[4]; // start with airport code only
} MetarLayer;

void metar_layer_init(WeatherLayer* metar_layer, GPoint pos);
void metar_layer_deinit(WeatherLayer* metar_layer);
void metar_layer_set_icon(WeatherLayer* metar_layer, WeatherIcon icon);
void metar_layer_set_temperature(WeatherLayer* metar_layer, int16_t temperature);
void metar_layer_set_precipitation_forecast(WeatherLayer* metar_layer, uint8_t* forecast, uint8_t length);
void metar_layer_clear_precipitation_forecast(WeatherLayer* metar_layer);

#endif

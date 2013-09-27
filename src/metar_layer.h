#ifndef METAR_LAYER_H
#define METAR_LAYER_H

#include "graph_layer.h"

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer text_layer;
	GraphLayer graph_layer;
	uint8_t* forecast;
	uint8_t forecast_count;
	bool has_metar;
	char metar_str[5]; // start with airport code only
} MetarLayer;

void metar_layer_init(MetarLayer* metar_layer, GPoint pos);
void metar_layer_set_text(MetarLayer* metar_layer, char metar_text[]);
void metar_layer_display_errortext(MetarLayer* metar_layer, char* error_text);
void metar_layer_deinit(MetarLayer* metar_layer);

#endif

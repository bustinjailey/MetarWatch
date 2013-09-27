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
	char metar_str[50];
} MetarLayer;

void metar_layer_init(MetarLayer* metar_layer, GPoint pos);
void metar_layer_set_text(MetarLayer* metar_layer, char * text);
void metar_layer_deinit(MetarLayer* metar_layer);

#endif

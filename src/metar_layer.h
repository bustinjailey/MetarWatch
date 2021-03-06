#ifndef METAR_LAYER_H
#define METAR_LAYER_H

typedef struct {
	Layer *layer;
	TextLayer *text_layer;
	bool has_metar;
	char metar_str[90];
} MetarLayer;

MetarLayer metar_layer_init(MetarLayer* metar_layer, GRect frame);
void metar_layer_set_text(MetarLayer* metar_layer, char * text);
void maximize_font_and_frame_size(MetarLayer* metar_layer);
void metar_layer_fit_to_text(GContext* ctx, MetarLayer* layer);
void set_text_size_from_index(MetarLayer* metar_layer, int index);
void metar_layer_deinit(MetarLayer* metar_layer);

#endif

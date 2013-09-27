#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "metar_layer.h"

void metar_layer_init(MetarLayer* metar_layer, GPoint pos) {
	layer_init(&metar_layer->layer, GRect(pos.x, pos.y, 144, 68));
	// Add metar text layer
	text_layer_init(&metar_layer->text_layer, GRect(0, 0, 144, 68));
	text_layer_set_text_alignment(&metar_layer->text_layer, GTextAlignmentCenter);
	text_layer_set_font(&metar_layer->text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));
	layer_add_child(&metar_layer->layer, &metar_layer->text_layer.layer);
}

void metar_layer_set_text(MetarLayer* metar_layer, char * text) {
	memcpy(metar_layer->metar_str, text, 50);
	text_layer_set_text(&metar_layer->text_layer, metar_layer->metar_str);
}

void metar_layer_deinit(MetarLayer* metar_layer) {
}


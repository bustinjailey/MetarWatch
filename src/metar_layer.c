#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "metar_layer.h"

void metar_layer_init(MetarLayer* metar_layer, GPoint pos) {
	layer_init(&metar_layer->layer, GRect(pos.x, pos.y, 144, 68));
	// Add metar text layer
	text_layer_init(&metar_layer->metar_layer, GRect(70, 9, 64, 68));
	text_layer_set_text_alignment(&metar_layer->metar_layer, GTextAlignmentCenter);
	text_layer_set_font(&metar_layer->metar_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_20)));
	layer_add_child(&metar_layer->layer, &metar_layer->metar_layer.layer);
// Change this section to display the text
	metar_layer->has_weather_icon = false;
}

void metar_layer_set_text(MetarLayer* metar_layer, char t[]) {
	int size = sizeof(t) / sizeof(t[0]);	
	memcpy(metar_layer->metar_str, t, size + 1);
	text_layer_set_text(&metar_layer->metar_str, metar_layer->metar_str);
}

void metar_layer_display_errortext(MetarLayer* metar_layer){
	char error[] = "ERR!";
	memcpy(metar_layer->metar_str, t, 5);
	text_layer_set_text(&metar_layer->metar_str, metar_layer->metar_str);
}

void metar_layer_deinit(MetarLayer* metar_layer) {
	if(metar_layer->has_weather_icon)
		bmp_deinit_container(&metar_layer->icon_layer);
}


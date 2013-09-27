#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "metar_layer.h"

void metar_layer_init(MetarLayer* metar_layer, GPoint pos) {
	layer_init(&metar_layer->layer, GRect(pos.x, pos.y, 144, 68));
	// Add temperature layer
	text_layer_init(&metar_layer->temp_layer, GRect(70, 9, 64, 68));
	text_layer_set_text_alignment(&metar_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_font(&metar_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_20)));
	layer_add_child(&metar_layer->layer, &metar_layer->temp_layer.layer);
// Change this section to display METAR text
	metar_layer->has_weather_icon = false;
}

void metar_layer_set_temperature(MetarLayer* metar_layer, int16_t t) {
	memcpy(metar_layer->temp_str, itoa(t), 4);
	int degree_pos = strlen(metar_layer->temp_str);
	memcpy(&metar_layer->temp_str[degree_pos], "°", 3);
	text_layer_set_text(&metar_layer->temp_layer, metar_layer->temp_str);
}

void metar_layer_deinit(MetarLayer* metar_layer) {
	if(metar_layer->has_weather_icon)
		bmp_deinit_container(&metar_layer->icon_layer);
}

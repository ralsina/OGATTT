#include <vtparse.h>


void handle_print(char b);
void csi_dispatch(char b);
void parser_callback(vtparse_t *parser, vtparse_action_t action, unsigned char ch);


extern vtparse_t parser;
extern Adafruit_SSD1306 display;

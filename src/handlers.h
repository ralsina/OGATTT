#include <vtparse.h>

void init_terminal(void);

void handle_print(char b);
void csi_dispatch(char b);
void parser_callback(vtparse_t *parser, vtparse_action_t action, unsigned char ch);
void read_kbd(char *data);

extern vtparse_t parser;

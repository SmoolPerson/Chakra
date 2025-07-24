#include "cfstruct.h"

#ifndef META_H
#define META_H

void print_help();
void parse_args(int argc, char *argv[], Config *config);
Config get_default_config();

#endif
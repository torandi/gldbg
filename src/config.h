#ifndef CONFIG_H
#define CONFIG_H

#include "buffers.h"

void __load_config();
void __free_config();
void __write_buffer_config();

/* Fetches the config data for the given key */
const char * __config(const char * key);

/* Load config for the buffer */
void __configure_buffer(struct __gl_buffer_t * buffer);

extern struct __buffer_type_t __default_buffer_type;

#define INTERVAL_NEVER -2
#define INTERVAL_KEYDOWN -1

int __parse_interval(const char * str);

void lowercase(char * str);

#endif

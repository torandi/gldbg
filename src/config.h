#ifndef CONFIG_H
#define CONFIG_H

#include "buffers.h"

void __load_config();
void __write_config();
void __free_config();

/* Fetches the config data for the given key */
const char * __config(const char * key);

/* Load config for the buffer */
void __configure_buffer(struct __gl_buffer_t * buffer);

extern struct __buffer_type_t __default_buffer_type;

struct __buffer_type_t __parse_buffer_type(const char * value);

void lowercase(char * str);

#endif

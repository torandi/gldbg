#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CONFIG_FILE "gldbg.conf"
#define BUFFER_CONFIG_FILE "buffers.conf"

struct __buffer_config_t {
	GLuint buffer;
	struct __buffer_type_t type;
	const char * target;
};

static struct __buffer_config_t * __buffer_config = NULL;
static unsigned int __buffer_config_size = 0;

struct __buffer_type_t __default_buffer_type;

struct __config_entry_t {
	char key[64];
	char value[128];
};

static struct __config_entry_t * __config_table = NULL;
static unsigned int __config_entries = 0;

static char * __find_entry(const char * key);

static void __load_defaults();

static void __write_config();

static struct __buffer_type_t __parse_buffer_config(const char * type, const char * output);
static struct __buffer_type_t __parse_buffer_row(const char * value);

/* Data for parsing/writing buffer config */
struct __buffer_config_type_t {
	const char * str;
	enum __data_type_t type;
};

static const struct __buffer_config_type_t __buffer_config_types[] = {
	{ "float", FLOAT },
	{ "int", INT },
};

static const size_t __num_buffer_config_types = sizeof(__buffer_config_types) / sizeof(struct __buffer_config_type_t);

struct __buffer_config_output_type_t {
	const char * str;
	enum __output_t type;
};

static const struct __buffer_config_output_type_t __buffer_config_output_types[] = {
	{ "none", 0 },
	{ "print", OUT_PRINT },
	{ "log", OUT_LOG },
	{ "both", OUT_BOTH },
};

static const size_t __num_buffer_config_output_types = sizeof(__buffer_config_output_types) / sizeof(struct __buffer_config_output_type_t);


void __load_config() {
	__load_defaults();
	FILE * f = fopen(CONFIG_FILE, "r");

	if(f != NULL) {
		char section_buffer[64];
		char key_buffer[64];
		char value_buffer[128];
		char buffer[256];
		char * key_start = NULL;

		while(!feof(f)) {
			if(fscanf(f, " // %s ", buffer) == 1) {
			} else if(fscanf(f, " [ %[^]] ] ", section_buffer) == 1) {
				sprintf(key_buffer, "%s:", section_buffer);
				key_start = key_buffer + strlen(section_buffer) + 1;
			} else if(key_start != NULL && fscanf(f, " %[^=] = ", key_start) == 1) {
				char * ret = fgets(value_buffer, 128, f);
				if(ret != NULL) {

					/* Trim key and value ends*/

					for(int i = 0; i < strlen(key_buffer); ++i) {
						if(key_buffer[i] == 0 || isspace(key_buffer[i])) {
							key_buffer[i] = 0;
							break;
						}
					}

					for(int i = (int)strlen(value_buffer) - 1; i >= 0; --i) {
						if(value_buffer[i] != 0 && !isspace(value_buffer[i])) {
							break;
						} else {
							value_buffer[i] = 0;
						}
					}

					lowercase(key_buffer);
					lowercase(value_buffer);

					char * entry = __find_entry(key_buffer);
					if(entry != NULL) {
						memcpy(entry, value_buffer, strlen(value_buffer) + 1);
					} else {
						__gldbg_printf("Unknown key %s in config\n", key_buffer);
					}
				} else {
					__gldbg_printf("Missing value for key %s\n", key_buffer);
				}
			} else {
				if(fscanf(f, "%s", buffer) == 1) {
					__gldbg_printf("Invalid config row: %s\n", buffer);
				} //else blank row
			}
		}
		fclose(f);
	} else {
		__write_config();
	}

	f = fopen(BUFFER_CONFIG_FILE, "r");
	if( f != NULL) {
		struct __buffer_config_t config[256];
		char type[16];
		char output[16];
		unsigned int index = 0;
		while(!feof(f)) {
			struct __buffer_config_t * cfg = config + index;
			if(fscanf(f," %u %s %s %*s ", &cfg->buffer, type, output) == 3) {
				cfg->type = __parse_buffer_config(type, output);
				if(++index == 256) {
					__buffer_config = realloc(__buffer_config, sizeof(struct __buffer_config_t) * (__buffer_config_size + index));
					memcpy(__buffer_config + __buffer_config_size, config, sizeof(struct __buffer_config_t) * index);
					__buffer_config_size += index;
					index = 0;
				}
			}
		}

		if(index > 0) {
			__buffer_config = realloc(__buffer_config, sizeof(struct __buffer_config_t) * (__buffer_config_size + index));
			memcpy(__buffer_config + __buffer_config_size, config, sizeof(struct __buffer_config_t) * index);
			__buffer_config_size += index;
		}
	}

	__default_buffer_type = __parse_buffer_row(__find_entry("buffers:defaults"));
}

static void __write_config() {
	FILE * f = fopen(CONFIG_FILE, "w");
	if(f != NULL ) {
		char * section = NULL;
		int current_section_len = 0;
		int section_len = 0;
		char * key = NULL;

		for(int i=0; i<__config_entries; ++i) {
			key = strstr(__config_table[i].key, ":");
			if(key == NULL) {
				__gldbg_printf("Invalid key %s\n", __config_table[i].key);
			} else {
				section_len = (int)(key - __config_table[i].key);
				++key;
				if(section == NULL || current_section_len != section_len || strncmp(section, __config_table[i].key, (size_t)section_len) != 0) {
					current_section_len = section_len;
					section = __config_table[i].key;
					fprintf(f, "[%.*s]\n", section_len, section);
				}
				fprintf(f, "\t%s = %s\n", key, __config_table[i].value);
			}
		}
		fclose(f);
		__gldbg_printf("Wrote config to %s\n", CONFIG_FILE);
	} else {
		__gldbg_printf("Failed to open %s for writing\n", CONFIG_FILE);
	}
}

void __write_buffer_config() {
	FILE * f = fopen(BUFFER_CONFIG_FILE, "w");
	if(f != NULL ) {
		for(int i=0;i < __buffer_config_size; ++i) {
			const char * type, *output;

			for(size_t j=0; j<__num_buffer_config_types; ++j) {
				if(__buffer_config[i].type.data_type == __buffer_config_types[j].type) {
					type = __buffer_config_types[j].str;
					break;
				}
			}

			for(size_t j=0; j<__num_buffer_config_output_types; ++j) {
				if(__buffer_config[i].type.output == __buffer_config_output_types[j].type) {
					output = __buffer_config_output_types[j].str;
					break;
				}
			}

			fprintf(f, "%3u\t\t%s%d\t%s\t%s\n", __buffer_config[i].buffer, type, __buffer_config[i].type.group_size, output,__buffer_config[i].target);
		}
		fclose(f);
		__gldbg_printf("Wrote config to %s\n", BUFFER_CONFIG_FILE);
	} else {
		__gldbg_printf("Failed to open %s for writing\n", BUFFER_CONFIG_FILE);
	}
}

void __free_config() {
	if(__config_table != NULL) free(__config_table);
	if(__buffer_config != NULL) free(__buffer_config);
}

const char * __config(const char * key) {
	const char * entry = (const char*)__find_entry(key);
	if(entry == NULL) {
		__gldbg_printf("Internal error: Unknown config key %s\n", key);
	}
	return (const char *) entry;
}

static void __load_defaults() {
	struct __config_entry_t config[] = {
		{ "keyboard:print_buffers", "ctrl shift t" },
		{ "buffers:defaults", "float4 both" },
		{ "buffers:log_interval", "20" },
		{ "buffers:print_interval", "keydown" },
	};

	__config_entries = sizeof(config) / sizeof(struct __config_entry_t);

	__config_table = malloc(sizeof(config));

	memcpy(__config_table, config, sizeof(config));
}

static char * __find_entry(const char * key) {
	for(int i=0; i<__config_entries; ++i) {
		if(strcmp(__config_table[i].key, key) == 0) {
			return __config_table[i].value;
		}
	}
	return NULL;
}

static struct __buffer_type_t __parse_buffer_row(const char * value) {
	char type[16];
	char output[16];
	sscanf(value, "%s %s", type, output);

	return __parse_buffer_config(type, output);
}

static struct __buffer_type_t __parse_buffer_config(const char * type, const char * output) {

	struct __buffer_type_t buffer_type = {
		.data_type = 0,
		.group_size = 0,
		.output = -1,
	};

	const char * num = NULL;

	for(int i=0; i < __num_buffer_config_types; ++i) {
		if(strncmp(type, __buffer_config_types[i].str, strlen(__buffer_config_types[i].str)) == 0) {
			buffer_type.data_type = __buffer_config_types[i].type;
			num = type + strlen(__buffer_config_types[i].str);
			break;
		}
	}

	if(buffer_type.data_type == 0) {
		buffer_type.data_type = FLOAT;
		buffer_type.group_size = 4;
		__gldbg_printf("Warning! Unknown data type %s\n", type);
	} else {
		buffer_type.group_size = atoi(num);
		if(buffer_type.group_size == 0) {
			__gldbg_printf("Warning! Invalid group size in data type %s\n", type);
			buffer_type.group_size = 4;
		}
	}


	for(int i=0; i<__num_buffer_config_output_types; ++i) {
		if(strcmp(output, __buffer_config_output_types[i].str) == 0) {
			buffer_type.output = __buffer_config_output_types[i].type;
			break;
		}
	}

	if(buffer_type.output == -1) {
		__gldbg_printf("Unknown output mode %s.\n", output);
		buffer_type.output = OUT_BOTH;
	}

	return buffer_type;
}

void __configure_buffer(struct __gl_buffer_t * buffer) {
	for(int i=0; i<__buffer_config_size; ++i) {
		if(buffer->name == __buffer_config[i].buffer) {
			__buffer_config[i].target = __target_names[__target_index(buffer->target)];
			buffer->type = __buffer_config[i].type;
			return;
		}
	}
	__buffer_config = realloc(__buffer_config, sizeof(struct __buffer_config_t) * (++__buffer_config_size));

	__buffer_config[__buffer_config_size - 1].buffer = buffer->name;
	__buffer_config[__buffer_config_size - 1].type = buffer->type;
	__buffer_config[__buffer_config_size - 1].target = __target_names[__target_index(buffer->target)];
}

int __parse_interval(const char * str) {
	if(strcmp(str, "never") == 0) return INTERVAL_NEVER;
	if(strcmp(str, "keydown") == 0) return INTERVAL_KEYDOWN;
	return atoi(str);
}

void lowercase(char * str) {
	for(int i=0; i< strlen(str); ++i) {
		str[i] = (char)tolower(str[i]);
	}
}

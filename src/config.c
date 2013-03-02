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

struct __buffer_type_t __default_buffer_type;

struct __config_entry_t {
	char key[64];
	char value[128];
};

static struct __config_entry_t * __config_table = NULL;
static unsigned int __config_entries = 0;

static char * __find_entry(const char * key);

static void __load_defaults();

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
				__gldbg_printf("Begin section %s\n", section_buffer);
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

					__gldbg_printf("'%s' = '%s'\n", key_buffer, value_buffer);

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
	}

	__default_buffer_type = __parse_buffer_type(__find_entry("buffers:defaults"));
}

void __write_config() {

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

struct __buffer_type_t __parse_buffer_type(const char * value) {
	char type[16];
	char output[16];

	struct type_t {
		const char * str;
		enum __data_type_t type;
	};

	struct type_t types[] = {
		{ "float", FLOAT },
		{ "int", INT },
	};

	struct __buffer_type_t buffer_type = {
		.data_type = 0,
		.group_size = 0,
		.output = -1,
	};

	const char * num = NULL;

	sscanf(value, "%s %s", type, output);

	lowercase(type);
	lowercase(output);

	for(int i=0; i < sizeof(types) / sizeof(struct type_t); ++i) {
		if(strncmp(type, types[i].str, strlen(types[i].str)) == 0) {
			buffer_type.data_type = types[i].type;
			num = type + strlen(types[i].str);
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

	struct output_type_t {
		const char * str;
		enum __buffer_output_t type;
	};

	struct output_type_t output_types[] = {
		{ "none", 0 },
		{ "print", OUT_PRINT },
		{ "log", OUT_LOG },
		{ "both", OUT_BOTH },
	};

	for(int i=0; i<sizeof(output_types) / sizeof(struct output_type_t); ++i) {
		if(strcmp(output, output_types[i].str) == 0) {
			buffer_type.output = output_types[i].type;
			break;
		}
	}

	if(buffer_type.output == -1) {
		__gldbg_printf("Unknown output mode %s.\n", output);
		buffer_type.output = OUT_BOTH;
	}

	return buffer_type;
}

void lowercase(char * str) {
	for(int i=0; i< strlen(str); ++i) {
		str[i] = (char)tolower(str[i]);
	}
}

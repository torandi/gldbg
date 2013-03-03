#include "glinject.h"
#include "buffers.h"

#include <string.h>

GL_BIND_BUFFER_FUNC __real_glBindBuffer = NULL;

/* Functions here must be in the order defined by the enum in glinject.h */
struct __gl_func_t __functions[] = {
	{ "glGenBuffers", (void*)__glGenBuffers , (void*)__glGenBuffersARB, NULL, },
	{ "glDeleteBuffers", (void*)__glDeleteBuffers , (void*)__glDeleteBuffersARB, NULL, },
	{ "glBindBuffer", (void*)__glBindBuffer , (void*)__glBindBufferARB, NULL, NULL, (void*)&__real_glBindBuffer },
	{ "glBufferData", (void*)__glBufferData , (void*)__glBufferDataARB, NULL,},
};


static void * __glXGetProcAddressINT (GL_X_GET_PROC_ADDRESS_FUNC real_func, const GLubyte * procName) {
	const char * proc_name = (const char*)procName;


	if(strncmp("ARB", proc_name + strlen(proc_name) - 3, 3) == 0) {
		/* ARB */
		for(int i=0; i< sizeof(__functions) / sizeof(struct __gl_func_t); ++i) {
			if(strncmp(__functions[i].name, proc_name, strlen(proc_name) - 3) == 0) {
				if(__functions[i].real_func_arb == NULL) {
#if DEBUG
					__gldbg_printf("Hooking into %s\n", (const char*) proc_name);
#endif
					__functions[i].real_func_arb = real_func(procName);
					if(__functions[i].internal_func != NULL && *(__functions[i].internal_func) == NULL) {
						*__functions[i].internal_func = __functions[i].real_func_arb;
					}
					return __functions[i].func;
				}
			}
		}
	} else {
		/* Non ARB */
		for(int i=0; i< sizeof(__functions) / sizeof(struct __gl_func_t); ++i) {
			if(strcmp(__functions[i].name, proc_name) == 0) {
				if(__functions[i].real_func == NULL) {
#if DEBUG
					__gldbg_printf("Hooking into %s\n", (const char*) proc_name);
#endif
					__functions[i].real_func = real_func(procName);
					if(__functions[i].internal_func != NULL && *(__functions[i].internal_func) == NULL) {
						*__functions[i].internal_func = __functions[i].real_func;
					}
					return __functions[i].func;
				}
			}
		}
	}
	
	return real_func(procName);
}

EXPORT void * glXGetProcAddress(const GLubyte * procName) {
	__gldbg_init();

	return __glXGetProcAddressINT(__real_glXGetProcAddress, procName);
}

EXPORT void * glXGetProcAddressARB(const GLubyte * procName) {
	__gldbg_init();

	return __glXGetProcAddressINT(__real_glXGetProcAddressARB, procName);
}


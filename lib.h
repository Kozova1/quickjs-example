#ifndef __LIB_H_
#define __LIB_H_

#include "quickjs/quickjs.h"

#ifdef JS_SHARED_LIBRARY
#define JS_INIT_MODULE js_init_module
#else
#define JS_INIT_MODULE js_init_module_klib
#endif
JSModuleDef *JS_INIT_MODULE(JSContext*, const char*);

#endif // __LIB_H_

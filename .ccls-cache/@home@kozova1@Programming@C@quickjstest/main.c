#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "QuickJS/quickjs.h"
#include "lib.h"

const char* tagtos(long long tag) {
    switch(tag) {
        case JS_TAG_INT:               return "int";
        case JS_TAG_BOOL:              return "bool";
        case JS_TAG_NULL:              return "null";
        case JS_TAG_UNDEFINED:         return "undefined";
        case JS_TAG_CATCH_OFFSET:      return "catch offset";
        case JS_TAG_EXCEPTION:         return "exception";
        case JS_TAG_FLOAT64:           return "float64";
        case JS_TAG_MODULE:            return "module";
        case JS_TAG_OBJECT:            return "object";
        case JS_TAG_STRING:            return "string";
        case JS_TAG_FIRST:             return "first";
        case JS_TAG_BIG_INT:           return "big_int";
        case JS_TAG_BIG_FLOAT:         return "big_float";
        case JS_TAG_SYMBOL:            return "symbol";
        case JS_TAG_FUNCTION_BYTECODE: return "function bytecode";
        default:                       return "unknown type!";
    }
}


int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("usage: main file.js");
        return 1;
    }
    if (access(argv[1], F_OK) == -1) {
        printf("file %s doesn't exist or is not readable\n", argv[1]);
        return 1;
    }
    struct stat *fstat = malloc(sizeof(struct stat));
    if (!fstat) {
        printf("line %d : malloc failed\n", __LINE__-2);
        return 1;
    }
    if (stat(argv[1], fstat) != 0) {
        printf("line %d : %m\n", __LINE__-1, errno);
        free(fstat);
        return 1;
    }
    int if_size = fstat->st_size;
    free(fstat);
    if (if_size == 0) {
        printf("file %s is empty!\n", argv[1]);
        return 1;
    }
    char* if_contents = malloc(if_size);
    if (!if_contents) {
        printf("line %d : malloc failed\n", __LINE__-2);
        return 1;
    }
    FILE* in_file = fopen(argv[1], "r");
    if (!in_file) {
        printf("line %d : %m\n", __LINE__-2, errno);
        return 1;
    }
    int size_read = fread(if_contents, 1, if_size, in_file);
    if (fclose(in_file) == EOF) {
        printf("line %d : failed to close file\n", __LINE__-1);
        return -1;
    }
    if (size_read != if_size) {
        printf(
            "line %d : expected %d bytes but read only %d instead!\n",
            __LINE__-8,
            if_size,
            size_read
        );
        return -1;
    }
    // Now we FINALLY know that everything went OK.

    JSRuntime* runtime = JS_NewRuntime();
    if (!runtime) {
        printf("line %d : JS_NewRuntime returned NULL\n", __LINE__-2);
        free(if_contents);
        return 1;
    }
    JSContext* ctx = JS_NewContext(runtime);
    if (!ctx) {
        printf("line %d : JS_NewContext returned NULL\n", __LINE__-2);
        free(runtime);
        free(if_contents);
        return 1;
    }
    JS_AddModuleExport(ctx, JS_INIT_MODULE(ctx, "klib"), "klib");
    int is_module = 0;
    if (strstr(argv[1], ".mjs")) {
        is_module = JS_EVAL_TYPE_MODULE;
    }
    JSValue result = JS_Eval(ctx, if_contents, if_size, argv[1], JS_EVAL_FLAG_STRICT | is_module);
    if (JS_IsException(result)) {
        printf("JS err : %s\n", JS_ToCString(ctx, JS_GetException(ctx)));
    } else if (JS_IsFunction(ctx, result)) {
        JSValue res = JS_Call(ctx, result, result, 0, NULL);
        puts(JS_ToCString(ctx, result));
        puts(JS_ToCString(ctx, res));
    } else {
        printf("type : %s (%ld)\n", tagtos(result.tag), result.tag);
        printf("val  : %s\n", JS_ToCString(ctx, result));
    }

    JS_RunGC(runtime);

    free(ctx);
    free(runtime);
    free(if_contents);
    return 0;
}

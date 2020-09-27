#include <stdio.h>
#include "lib.h"

// example function for tic80's print()
static JSValue js_print(JSContext* ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv) {
    int x = 0, y = 0, color = 15, fixed = 0, scale = 1, smallfont = 0;
    if (argc >= 8 || argc <= 0) {
        return JS_EXCEPTION;
    }
    if (argc >= 2) {
        if (JS_ToInt32(ctx, &x, argv[1]))
            return JS_EXCEPTION;
    }
    if (argc >= 3) {
        if (JS_ToInt32(ctx, &y, argv[2]))
            return JS_EXCEPTION;
    }
    if (argc >= 4) {
        if (JS_ToInt32(ctx, &color, argv[3]))
            return JS_EXCEPTION;
    }
    if (argc >= 5) {
        fixed = JS_ToBool(ctx, argv[4]);
    }
    if (argc >= 6) {
        if (JS_ToInt32(ctx, &scale, argv[5]))
            return JS_EXCEPTION;
    }
    if (argc == 7) {
        smallfont = JS_ToBool(ctx, argv[6]);
    }

    printf("[tic] \"%s\"  ---  x:%d y:%d col:%d fixed:%d scale:%d smallfont:%d\n", JS_ToCString(ctx, argv[0]), x, y, color, fixed, scale, smallfont);
    return JS_NewInt32(ctx, 32); // width of text
}
// example of the tic80 reset()
static JSValue js_reset(JSContext* ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv) {
    printf("[tic] called reset() with %d args\n", argc);
    return JS_UNDEFINED;
}

// list of exported functions, the string is how they'll appear in the module
static const JSCFunctionListEntry js_tic_funcs[] = {
    JS_CFUNC_DEF("print", 7, js_print ),
    JS_CFUNC_DEF("reset", 0, js_reset ),
};

// initializes the module with the export functions list and it's length
static int js_tic_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, js_tic_funcs,
                                  2);
}


// this is what we use later as the module itself.
JSModuleDef *JS_INIT_MODULE(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_tic_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_tic_funcs, 2);
    return m;
}

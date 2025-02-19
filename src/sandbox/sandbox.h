#ifndef SANDBOX_H
#define SANDBOX_H

#include <stddef.h>
#include <lua.h>

typedef struct ts_Sandbox ts_Sandbox;

ts_Sandbox* ts_sandbox_create();
void        ts_sandbox_free(ts_Sandbox* sb);

ts_Sandbox* ts_sandbox_unserialize(lua_State* L, char* error_buf, size_t error_sz);
ts_Sandbox* ts_sandbox_unserialize_from_string(const char* str, char* error_buf, size_t error_sz);
void        ts_sandbox_serialize(ts_Sandbox* sb, char* buf, size_t buf_sz);

#endif //SANDBOX_H

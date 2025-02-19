#include "wire.h"

#include "util/serialize.h"

int ts_wire_serialize(ts_Wire const* wire, int, char* buf, size_t buf_sz)
{
    return snprintf(buf, buf_sz, "\"%c%c\"", wire->width, wire->layer);
}

ts_Response ts_wire_unserialize(ts_Wire* wire, lua_State* L)
{

}

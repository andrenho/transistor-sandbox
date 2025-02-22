#ifndef POS_DS_H
#define POS_DS_H

#include "position.h"

typedef struct { ts_PositionHash key; bool value; } ts_PosSet;

#define psetlen                       hmlen
#define psetput(collection, pos)      { ts_PositionHash __p = ts_pos_hash(pos); hmput((collection), __p, true); }
#define psetcontains(collection, pos) (hmgeti((collection), ts_pos_hash(pos)) >= 0)
#define psetdel(collection, pos)      hmdel((collection), ts_pos_hash(pos))
#define psetget(collection, idx)      (ts_pos_unhash(collection[idx].key))
#define psetfirst(collection)         (psetget(collection, 0))

#endif //POS_DS_H

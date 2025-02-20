#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdio.h>

#define SR_INIT(s)             int current_idx = vspace ? snprintf(buf, buf_sz, "%*c" s "\n", vspace, ' ') : snprintf(buf, buf_sz, s "\n");
#define SR_INIT_INLINE(s)      int current_idx = snprintf(buf, buf_sz, s "\n");
#define SR_ADD_INLINE(s, ...)  current_idx += snprintf(&buf[current_idx], buf_sz - current_idx, s, ##__VA_ARGS__)
#define SR_CONT_INLINE(s, ...) current_idx += vspace ? snprintf(&buf[current_idx], buf_sz - current_idx, "%*c" s, vspace, ' ', ##__VA_ARGS__) : snprintf(&buf[current_idx], buf_sz - current_idx, s, ##__VA_ARGS__)
#define SR_CONT(s, ...)        SR_CONT_INLINE(s, ##__VA_ARGS__); current_idx += snprintf(&buf[current_idx],  buf_sz - current_idx, "\n");
#define SR_FINI_INLINE(s, ...) SR_CONT_INLINE(s, ##__VA_ARGS__); return current_idx
#define SR_FINI(s, ...)        SR_CONT(s, ##__VA_ARGS__); return current_idx

#define SR_CALL(f, par, v)     current_idx += f(par, vspace + v, &buf[current_idx], buf_sz - current_idx)
#define SR_CALL_INLINE(f, par) current_idx += f(par, 0, &buf[current_idx], buf_sz - current_idx)

#endif //SERIALIZE_H

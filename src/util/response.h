#ifndef RESPONSE_H
#define RESPONSE_H

typedef enum {
    TS_OK = 0,
    TS_DESERIALIZATION_ERROR = -1,
    TS_CANNOT_PLACE = -2,
} ts_Response;

#endif //RESPONSE_H

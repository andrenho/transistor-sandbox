#include "transistor-sandbox.h"

#include <stdio.h>

int main()
{
    ts_Sandbox sb;
    ts_sandbox_init(&sb);

    ts_board_add_wire(&sb.boards[0], &(ts_Position) { TS_S, 1, 1 }, &(ts_Wire) { TS_W1, TS_TOP });

    char buf[16 * 1024];
    ts_sandbox_serialize(&sb, 0, buf, sizeof buf);
    printf("%s\n", buf);

    return 0;
}

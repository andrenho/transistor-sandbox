#include "simulation.h"

#include <errno.h>
#include <stb_ds.h>
#include <sched.h>
#include <sys/time.h>

#include "compiler/compiler.h"
#include "sandbox/sandbox.h"

//
// execution
//

static ts_Result ts_simulation_single_step(ts_Simulation* sim)
{
    // execute components
    for (int i = 0; i < arrlen(sim->sandbox->boards); ++i) {
        ts_Board* board = &sim->sandbox->boards[i];
        for (int j = 0; j < hmlen(board->components); ++j) {
            ts_Component* component = board->components[j].value;
            if (component->def->simulate)
                component->def->simulate(component);       // (note: modify component)
        }
    }

    // update connection and input pin values
    for (int i = 0; i < arrlen(sim->connections); ++i) {

        ts_Connection* connection = &sim->connections[i];
        uint8_t value = 0;

        // calculate value from output pins
        for (int j = 0; j < arrlen(connection->pins); ++j) {
            ts_Pin* pin = &connection->pins[j];
            if (pin->component->def->pins[pin->pin_no].type == TS_OUTPUT)
                value |= pin->component->pins[pin->pin_no];
        }

        connection->value = value;

        // set input pins
        for (int j = 0; j < arrlen(connection->pins); ++j) {
            ts_Pin* pin = &connection->pins[j];
            if (pin->component->def->pins[pin->pin_no].type == TS_INPUT)
                pin->component->pins[pin->pin_no] = value;     // (note: modify component)
        }

    }

    return TS_OK;
}

ts_Result ts_simulation_run(ts_Simulation* sim, size_t run_for_us)
{
    ts_Result r = TS_OK;

    if (!sim->multithreaded) {
        struct timeval t;
        gettimeofday(&t, NULL);

        long end_time = t.tv_sec * 1000000 + t.tv_usec + run_for_us;
        for (;;) {
            if ((r = ts_simulation_single_step(sim)) != TS_OK)
                break;

            gettimeofday(&t, NULL);
            long current_time = t.tv_sec * 1000000 + t.tv_usec + run_for_us;
            if (current_time > end_time)
                break;
        }
    }

    return r;
}

static void* ts_simulation_run_thread(void* psim)
{
    ts_Simulation* sim = psim;

    while (sim->thread_running) {
        ts_simulation_single_step(sim);
        if (!sim->heavy)
            sched_yield();
    }

    return NULL;
}

//
// initialize
//

ts_Result ts_simulation_init(ts_Simulation* sim, bool multithreaded, bool heavy, ts_Sandbox* sandbox)
{
    sim->connections = NULL;
    sim->multithreaded = multithreaded;
    sim->heavy = heavy;
    sim->sandbox = sandbox;
    if (multithreaded) {
        sim->thread_running = true;
        if (pthread_create(&sim->thread, NULL, ts_simulation_run_thread, sim) != 0)
            return ts_error(sandbox, TS_SYSTEM_ERROR, "Could not create simulation thread: %s", strerror(errno));
    }
    return TS_OK;
}

ts_Result ts_simulation_finalize(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        sim->thread_running = false;
        pthread_join(sim->thread, NULL);
    }

    for (int i = 0; i < arrlen(sim->connections); ++i)
        ts_connection_finalize(&sim->connections[i]);
    arrfree(sim->connections);
    return TS_OK;
}

//
// start/stop
//

ts_Result ts_simulation_start(ts_Simulation* sim, ts_Sandbox const* sb)
{
    ts_simulation_stop(sim);
    /*
    sim->connections = ts_compiler_compile(sb);
    sim->sandbox = sb;
    */
    sim->connections = ts_compiler_compile(sim->sandbox);
    if (sim->multithreaded) {
        // TODO - start execution thread
    }
    return TS_OK;
}

ts_Result ts_simulation_stop(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        // TODO - stop execution thread
    }
    ts_simulation_finalize(sim);
    return TS_OK;
}

//
// pause/unpause
//

ts_Result ts_simulation_pause(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        // TODO
    }
    return TS_OK;
}

ts_Result ts_simulation_unpause(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        // TODO
    }
    return TS_OK;
}

//
// information
//

size_t ts_simulation_wires(ts_Simulation* sim, ts_Position* positions, uint8_t* data, size_t sz)
{
    ts_simulation_pause(sim);

    size_t count = 0;

    for (int i = 0; i < arrlen(sim->connections); ++i) {
        for (int j = 0; j < arrlen(sim->connections[i].wires); ++j) {
            if (count >= sz)
                return count;
            positions[count] = sim->connections[i].wires[j];
            data[count] = sim->connections[i].value;
            ++count;
        }
    }

    ts_simulation_unpause(sim);

    return count;
}


#include "simulation.h"

#include <errno.h>
#include <stb_ds.h>
#include <sched.h>
#include <stdio.h>
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

    ++sim->steps;

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
            long current_time = t.tv_sec * 1000000 + t.tv_usec;
            if (current_time > end_time)
                break;
        }
    }

    return r;
}

//
// thread
//

static void* ts_simulation_run_thread(void* psim)
{
    ts_Simulation* sim = psim;

    while (sim->thread_running) {

        // pause thread
        pthread_mutex_lock(&sim->thread_lock);
        while (sim->thread_paused)
            pthread_cond_wait(&sim->thread_cond, &sim->thread_lock);

        // execute
        ts_simulation_single_step(sim);

        // unpause
        pthread_mutex_unlock(&sim->thread_lock);

        // give the CPU a break
        if (!sim->heavy)
            sched_yield();
    }

    return NULL;
}

static void start_thread(ts_Simulation* sim)
{
    sim->thread_running = true;
    pthread_create(&sim->thread, NULL, ts_simulation_run_thread, sim);
}

static void end_thread(ts_Simulation* sim)
{
    sim->thread_running = false;
    if (sim->thread != 0)
        pthread_join(sim->thread, NULL);
    sim->thread = 0;
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
    sim->steps = 0;

    sim->thread_paused = false;
    if (multithreaded) {
        pthread_mutex_init(&sim->thread_lock, NULL);
        pthread_cond_init(&sim->thread_cond, NULL);
    }

    return TS_OK;
}

ts_Result ts_simulation_finalize(ts_Simulation* sim)
{
    if (sim->multithreaded)
        end_thread(sim);

    for (int i = 0; i < arrlen(sim->connections); ++i)
        ts_connection_finalize(&sim->connections[i]);
    arrfree(sim->connections);
    return TS_OK;
}

//
// start/stop
//

ts_Result ts_simulation_start(ts_Simulation* sim)
{
    sim->connections = ts_compiler_compile(sim->sandbox);
    sim->thread_paused = false;
    if (sim->multithreaded)
        start_thread(sim);
    return TS_OK;
}

ts_Result ts_simulation_end(ts_Simulation* sim)
{
    if (sim->multithreaded)
        end_thread(sim);
    ts_simulation_finalize(sim);
    return TS_OK;
}

//
// pause/unpause
//

ts_Result ts_simulation_pause(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        pthread_mutex_lock(&sim->thread_lock);
        sim->thread_paused = true;
    }
    return TS_OK;
}

ts_Result ts_simulation_unpause(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        sim->thread_paused = false;
        pthread_cond_signal(&sim->thread_cond);
        pthread_mutex_unlock(&sim->thread_lock);
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

size_t ts_simulation_steps(ts_Simulation* sim)
{
    ts_simulation_pause(sim);
    size_t steps = sim->steps;
    ts_simulation_unpause(sim);
    return steps;
}

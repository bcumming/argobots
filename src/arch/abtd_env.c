/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include "abti.h"
#include <unistd.h>

#define ABTD_KEY_TABLE_DEFAULT_SIZE     4
#define ABTD_THREAD_DEFAULT_STACKSIZE   16384
#define ABTD_SCHED_DEFAULT_STACKSIZE    (4*1024*1024)
#define ABTD_SCHED_EVENT_FREQ           50


void ABTD_env_init(ABTI_global *p_global)
{
    char *env;

    /* Get the number of available cores in the system */
    p_global->num_cores = sysconf(_SC_NPROCESSORS_ONLN);

    /* By default, we use the CPU affinity */
    p_global->set_affinity = ABT_TRUE;
    env = getenv("ABT_ENV_SET_AFFINITY");
    if (env != NULL) {
        if (strcmp(env, "0") == 0 || strcmp(env, "NO") == 0 ||
            strcmp(env, "no") == 0 || strcmp(env, "No") == 0) {
            p_global->set_affinity = ABT_FALSE;
        }
    }
    if (p_global->set_affinity == ABT_TRUE) {
        ABTD_affinity_init();
    }

#ifdef ABT_CONFIG_USE_DEBUG_LOG
    /* If the debug logging is set in configure, logging is turned on by
     * default. */
    p_global->use_logging = ABT_TRUE;
    p_global->use_debug = ABT_TRUE;
#else
    /* Otherwise, logging is not turned on by default. */
    p_global->use_logging = ABT_FALSE;
    p_global->use_debug = ABT_FALSE;
#endif
    env = getenv("ABT_ENV_USE_LOG");
    if (env != NULL) {
        if (strcmp(env, "0") == 0 || strcmp(env, "NO") == 0 ||
            strcmp(env, "no") == 0 || strcmp(env, "No") == 0) {
            p_global->use_logging = ABT_FALSE;
        } else {
            p_global->use_logging = ABT_TRUE;
        }
    }
    env = getenv("ABT_ENV_USE_DEBUG");
    if (env != NULL) {
        if (strcmp(env, "0") == 0 || strcmp(env, "NO") == 0 ||
            strcmp(env, "no") == 0 || strcmp(env, "No") == 0) {
            p_global->use_debug = ABT_FALSE;
        } else {
            p_global->use_debug = ABT_TRUE;
        }
    }

    /* Maximum size of the internal ES array */
    env = getenv("ABT_ENV_MAX_NUM_XSTREAMS");
    if (env != NULL) {
        p_global->max_xstreams = atoi(env);
    } else {
        p_global->max_xstreams = p_global->num_cores;
    }

    /* Default key table size */
    env = getenv("ABT_ENV_KEY_TABLE_SIZE");
    if (env != NULL) {
        p_global->key_table_size = (int)atoi(env);
    } else {
        p_global->key_table_size = ABTD_KEY_TABLE_DEFAULT_SIZE;
    }

    /* Default stack size for ULT */
    env = getenv("ABT_ENV_THREAD_STACKSIZE");
    if (env != NULL) {
        p_global->thread_stacksize = (size_t)atol(env);
        ABTI_ASSERT(p_global->thread_stacksize >= 512);
    } else {
        p_global->thread_stacksize = ABTD_THREAD_DEFAULT_STACKSIZE;
    }

    /* Default stack size for scheduler */
    env = getenv("ABT_ENV_SCHED_STACKSIZE");
    if (env != NULL) {
        p_global->sched_stacksize = (size_t)atol(env);
        ABTI_ASSERT(p_global->sched_stacksize >= 512);
    } else {
        p_global->sched_stacksize = ABTD_SCHED_DEFAULT_STACKSIZE;
    }

    /* Default frequency for event checking by the scheduler */
    env = getenv("ABT_ENV_SCHED_EVENT_FREQ");
    if (env != NULL) {
        p_global->sched_event_freq = (uint32_t)atol(env);
        ABTI_ASSERT(p_global->sched_event_freq >= 1);
    } else {
        p_global->sched_event_freq = ABTD_SCHED_EVENT_FREQ;
    }

#ifdef ABT_CONFIG_HANDLE_POWER_EVENT
    char *p_host = "localhost";
    int port = 60439;

    /* Hostname for power management daemon */
    env = getenv("ABT_ENV_POWER_EVENT_HOSTNAME");
    if (env != NULL) {
        p_host = env;
    }

    /* Port number for power management daemon */
    env = getenv("ABT_ENV_POWER_EVENT_PORT");
    if (env != NULL) {
        port = atoi(env);
    }

    ABTI_event_connect_power(p_host, port);
#endif

    /* Init timer */
    ABTD_time_init();
}


#ifndef INC_phold_h
#define INC_phold_h

#include <ross.h>

	/*
	 * PHOLD Types
	 */

struct phold_state_tag;
typedef struct phold_state_tag phold_state;
struct phold_message_tag;
typedef struct phold_message_tag phold_message;

struct phold_state_tag
{
	long int	 dummy_state;
};

struct phold_message_tag
{
	long int	 dummy_data;
};

	/*
	 * PHOLD Globals
	 */
tw_stime lookahead = 1.0;
static unsigned int stagger = 0;
static unsigned int offset_lpid = 0;
static tw_stime mult = 1.4;
static tw_stime percent_remote = 0.25;
static unsigned int ttl_lps = 0;
static unsigned int nlp_per_pe = 8;
static int g_phold_start_events = 1;
static int optimistic_memory = 100;

// rate for timestamp exponential distribution
static tw_stime mean = 1.0;

static char run_id[1024] = "undefined";

#endif
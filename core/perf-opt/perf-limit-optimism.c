#include "ross.h"

static int limit_opt = LIMIT_OFF;
static pe_data last_check = {0};
int g_perf_disable_opt = 0;
static tw_stime max_diff_sim = 0.0;

static const tw_optdef perf_options[] = {
    TWOPT_GROUP("ROSS Performance Optimizations"),
    TWOPT_UINT("disable-opt", g_perf_disable_opt, "disable performance optimizations when set to 1"),
    TWOPT_END()
};

const tw_optdef *perf_opts(void)
{
	return perf_options;
}

void perf_init()
{
    int i;

    last_check.nevent_processed = tw_calloc(TW_LOC, "performance optimizations", sizeof(tw_stat), g_tw_nkp);
    last_check.e_rbs = tw_calloc(TW_LOC, "performance optimizations", sizeof(tw_stat), g_tw_nkp);
    for (i = 0; i < g_tw_nkp; i++)
    {
        last_check.nevent_processed[i] = 0;
        last_check.e_rbs[i] = 0;
    }
}

double perf_efficiency_check(tw_pe *pe)
{
    //tw_statistics s;
    //bzero(&s, sizeof(s));
    //tw_get_stats(pe, &s);

    int i;
    tw_kp *kp;
    tw_stat nevent_delta;
    tw_stat e_rbs_delta;
    //double efficiency = 0;
    //double lowest_eff = DBL_MAX;
    tw_stime vt_diff = 0.0;
    tw_stime max_diff = 0.0;

    for (i = 0; i < g_tw_nkp; i++)
    {
        kp = tw_getkp(i);
        vt_diff = kp->last_time - pe->GVT;

        if (vt_diff > max_diff)
            max_diff = vt_diff;

        //nevent_delta = kp->s_nevent_processed - last_check.nevent_processed[i];
        //e_rbs_delta = kp->s_e_rbs - last_check.e_rbs[i];
        //if (nevent_delta - e_rbs_delta != 0)
        //    efficiency = 100.0 * (1 - ((double)e_rbs_delta/(double)(nevent_delta - e_rbs_delta)));
        //else
        //    efficiency = 0;

        //if (efficiency < lowest_eff)
        //    lowest_eff = efficiency;

        //last_check.nevent_processed[i] = kp->s_nevent_processed;
        //last_check.e_rbs[i] = kp->s_e_rbs;

    }

    vt_diff = max_diff;
    tw_stime ratio = vt_diff/max_diff_sim;
    //efficiency = lowest_eff;

    if (limit_opt == LIMIT_OFF || limit_opt == LIMIT_RECOVER)
    {
        //if (efficiency < 0.0) // set flag to limit optimism
        if (vt_diff > max_diff_sim)
            limit_opt = LIMIT_ON;
    }
    else if (limit_opt == LIMIT_ON)
    {
        //if (efficiency > 0)
        if (vt_diff <= max_diff_sim)
            limit_opt = LIMIT_RECOVER;
    }
    //return efficiency;
    if (vt_diff > max_diff_sim)
        max_diff_sim = vt_diff;

    return ratio;
}

void perf_adjust_optimism(tw_pe *pe)
{
    if (g_perf_disable_opt)
        return;

    //double efficiency = perf_efficiency_check(pe);
    tw_stime diff_ratio = perf_efficiency_check(pe);
    //int inc_amount = 1000 * efficiency;
    int inc_amount = 1000 * (1-diff_ratio);
    //double dec_amt = 1.0 - (abs(efficiency) / (abs(efficiency) + 1000.0));
    double dec_amt = diff_ratio;
    if (dec_amt < 0.05)
        dec_amt = 0.05;
    else if (dec_amt > 0.95)
        dec_amt = 0.95;

    if (limit_opt == LIMIT_ON)
    { // use multiplicative decrease to lower g_st_max_opt_lookahead
        if (g_tw_max_opt_lookahead >= 1000)
            g_tw_max_opt_lookahead *= dec_amt;
        //printf("efficiency = %f, dec_amt = %f\n", efficiency, dec_amt);
        //printf("PE %ld decreasing opt lookahead to %llu\n", g_tw_mynode, g_tw_max_opt_lookahead);
    }
    else if (limit_opt == LIMIT_RECOVER)
    { // use additive increase to slowly allow for more optimism
        if (ULLONG_MAX - g_tw_max_opt_lookahead < inc_amount) // avoid overflow
            g_tw_max_opt_lookahead = ULLONG_MAX;
        else 
            g_tw_max_opt_lookahead += inc_amount;
        //printf("PE %ld increasing opt lookahead to %llu\n", g_tw_mynode, g_tw_max_opt_lookahead);
    }
}


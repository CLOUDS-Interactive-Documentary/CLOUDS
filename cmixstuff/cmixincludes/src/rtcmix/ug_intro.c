#include	"ugens.h"
#include <ug_intro.h>

extern void rt_ug_intro();

void
ug_intro()
{
   rt_ug_intro();	// introduce functions owned by RTcmix object
/*
 *  This cute macro (from ugens.h) makes the necessary declarations
 *  and adds the function to ug_list
 */
   UG_INTRO_HANDLE_RETURN("maketable", maketable);
   UG_INTRO_DOUBLE_RETURN("tablelen", tablelen);
   UG_INTRO_DOUBLE_RETURN("tablemin", tablemin);
   UG_INTRO_DOUBLE_RETURN("tablemax", tablemax);
   UG_INTRO_HANDLE_RETURN("copytable", copytable);
   UG_INTRO_DOUBLE_RETURN("samptable", samptable);
   UG_INTRO_DOUBLE_RETURN("dumptable", dumptable);
   UG_INTRO_DOUBLE_RETURN("plottable", plottable);
   UG_INTRO_HANDLE_RETURN("mul", opmul);
   UG_INTRO_HANDLE_RETURN("add", opadd);
   UG_INTRO_HANDLE_RETURN("div", opdiv);
   UG_INTRO_HANDLE_RETURN("sub", opsub);
   UG_INTRO_HANDLE_RETURN("modtable", modtable);
   UG_INTRO_HANDLE_RETURN("makeconnection", makeconnection);
   UG_INTRO_HANDLE_RETURN("makeLFO", makeLFO);
   UG_INTRO_HANDLE_RETURN("makerandom", makerandom);
   UG_INTRO_HANDLE_RETURN("makefilter", makefilter);
   UG_INTRO_HANDLE_RETURN("makeconverter", makeconverter);
   UG_INTRO_HANDLE_RETURN("makemonitor", makemonitor);
	UG_INTRO_HANDLE_RETURN("makeinstrument", makeinstrument);
	UG_INTRO_DOUBLE_RETURN("filedur", filedur);
	UG_INTRO_DOUBLE_RETURN("filechans", filechans);
	UG_INTRO_DOUBLE_RETURN("filesr", filesr);
	UG_INTRO_DOUBLE_RETURN("filepeak", filepeak);
	UG_INTRO_DOUBLE_RETURN("filerms", filerms);
	UG_INTRO_DOUBLE_RETURN("filedc", filedc);
	UG_INTRO_DOUBLE_RETURN("bus_exists", bus_exists);
	UG_INTRO_DOUBLE_RETURN("bus_link", bus_link);
	UG_INTRO("makegen", makegen);
	UG_INTRO("open",m_open);
	UG_INTRO("peakoff",peak_off);
	UG_INTRO("punch",punch_on);
	UG_INTRO("sfclean",m_clean);
	UG_INTRO("sfprint",sfprint);
	UG_INTRO("system",m_system);
	UG_INTRO("sfcopy",sfcopy);
	UG_INTRO("input",m_input);
	UG_INTRO("output",m_output);
	UG_INTRO("cpspch",m_cpspch);
	UG_INTRO("pchcps",m_pchcps);
 	UG_INTRO("pchoct",m_pchoct);
 	UG_INTRO("octpch",m_octpch);
 	UG_INTRO("octlet",m_octlet);
 	UG_INTRO("cpslet",m_cpslet);
 	UG_INTRO("pchlet",m_pchlet);
 	UG_INTRO("pchadd",m_pchadd);
	UG_INTRO("translen",m_translen);
 	UG_INTRO("dbamp",m_dbamp);
	UG_INTRO("octcps",m_octcps);
	UG_INTRO("cpsoct",m_cpsoct);
	UG_INTRO("random",m_random);
	UG_INTRO("rand",m_rand);
	UG_INTRO("srand",m_srand);
	UG_INTRO("fplot",fplot);
	UG_INTRO("fdump",fdump);
	UG_INTRO("tb",m_time_beat);
	UG_INTRO("bt",m_beat_time);
	UG_INTRO("tbase",tbase);
	UG_INTRO("tempo",tempo);
	UG_INTRO("trunc",m_trunc);
	UG_INTRO("ampdb",m_ampdb);
	UG_INTRO("boost",m_boost);
	UG_INTRO("resetamp",resetamp);
	UG_INTRO("sr",m_sr);
	UG_INTRO("chans",m_chans);
	UG_INTRO("class",m_class);
	UG_INTRO("dur",m_dur);
	UG_INTRO("peak",m_peak);
	UG_INTRO("left_peak",m_left);
	UG_INTRO("right_peak",m_right);
	UG_INTRO("load_array",m_load_array);
	UG_INTRO("get_array",m_get_array);
	UG_INTRO("get_sum",m_get_sum);
	UG_INTRO("mod",m_mod);
	UG_INTRO("put_array",m_put_array);
	UG_INTRO("get_size",m_get_size);
	UG_INTRO("max",m_max);
	UG_INTRO("min",m_min);
	UG_INTRO("exit",m_exit);
	UG_INTRO("info",m_info);
 	UG_INTRO("infile", m_infile);
 	UG_INTRO("sampfunc",m_sampfunc);
 	UG_INTRO("sampfunci",m_sampfunci);
 	UG_INTRO("getpch",m_getpch);
 	UG_INTRO("getamp",m_getamp);
 	UG_INTRO("stringify",m_stringify);
	UG_INTRO("log",m_log);
	UG_INTRO("ln",m_ln);
	UG_INTRO("pow",m_pow);
	UG_INTRO("round",m_round);
	UG_INTRO("print",m_print);
	UG_INTRO("wrap",m_wrap);
	UG_INTRO("abs",m_abs);
	UG_INTRO("f_arg",f_arg); /* to return float from command line */
	UG_INTRO("i_arg",i_arg); /* to return int from command line */
	UG_INTRO("s_arg",s_arg); /* to return string from command line */
	UG_INTRO("n_arg",n_arg); /* to return num args from command line */
	UG_INTRO("print_on",m_print_is_on); /* to turn on printing*/
	UG_INTRO("print_off",m_print_is_off); /* to turn off printing*/
	UG_INTRO("str_num",str_num); /* string,num,strin,num, etc print out */
	UG_INTRO("get_spray",m_get_spray);
	UG_INTRO("spray_init",m_spray_init);
	UG_INTRO("pchmidi",m_pchmidi);
	UG_INTRO("cpsmidi",m_cpsmidi);
	UG_INTRO("octmidi",m_octmidi);
	UG_INTRO("midipch", m_midipch);
	UG_INTRO("setline_size",m_setline_size);
	UG_INTRO("setline",m_setline);
	UG_INTRO("reset",m_reset);
	UG_INTRO("control_rate",m_reset); /* because "reset" is a perlfunc */
	UG_INTRO("load",m_load); /* allows loading of dynamic libraries */
	UG_INTRO_DOUBLE_RETURN("pickrand", m_pickrand);
	UG_INTRO_DOUBLE_RETURN("pickwrand", m_pickwrand);
	UG_INTRO_DOUBLE_RETURN("get_time", get_time);
	UG_INTRO("irand", m_irand);
	UG_INTRO("trand", m_trand);
//	UG_INTRO("grand", m_grand);
	UG_INTRO("setexp", m_setexp);
	UG_INTRO("addgens", m_addgens);
	UG_INTRO("multgens", m_multgens);
	UG_INTRO("copygen", m_copygen);
	UG_INTRO("offsetgen", m_offsetgen);
	UG_INTRO("scalegen", m_scalegen);
	UG_INTRO("invertgen", m_invertgen);
	UG_INTRO("reversegen", m_reversegen);
	UG_INTRO("shiftgen", m_shiftgen);
	UG_INTRO("quantizegen", m_quantizegen);
}


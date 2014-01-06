/* print_off(); */
/* print_on(5) */
/* set_option("exit_on_error=0") */

srand();
reset(11025);

amp_declick = maketable("line", 4000, 0.0, 0.0, 0.001, 1., 0.999, 1., 1.0, 0.);
amp_trapezoid = maketable("line", 1000, 0.0, 0.0, 50.0, 1.0, 200.0, 1., 250.0, 0.0);
amp_sharp = maketable("line", 1000, 0.0, 0.0, 0.01, 1.0, 3.0, 0.0);
amp_sharpadsr = maketable("line", 4000, 0.0, 0.0, 0.01, 1.0, 0.5, 0.7, 2., 0.7, 3.0, 0.0);
amp_sharphold = maketable("line", 4000, 0.0, 0.0, 0.01, 1.0, 25, 1., 30, 0.0);
amp_triangle = maketable("line", 1000, 0.0, 0.0, 1.0, 1.0, 2.0, 0.0);
hanning = maketable("window", 4000, "hanning")
hamming = maketable("window", 4000, "hamming")

wf_puresine = maketable("wave", 1000, 1.0);
wf_dampedsine = maketable("wave", 1000, 1.0, 0.1, 0.01, 0.001, 0.0001);
wf_organ = maketable("wave", 1000, 1.0, 0.7, 0.4, 0.3, 0.2, 0.1, 0.05, 0.02);
wf_slowwaves = maketable("wave", 1000, 1.0, 0.3, 0.2, 0.1, 0.05);
wf_slowwaveshi = maketable("wave", 1000, 1.0, 0.1, 0.3, 0.05, 0.15, 0.025);
wf_tri = maketable("wave", 1000, "tri30");
wf_saw = maketable("wave", 1000, "sawup30")
wf_square = maketable("wave", 1000, "square30")
wf_buzz = maketable("wave", 1000, "buzz30")

vel_strike = maketable("line", 1000, 0.0, 1.0, 1.0, 0.0, 10., 0.);
ws_xfer = maketable("linebrk", 1000, -0.5, 300, -0.5, 200, 0, 200, 0.5, 300, 0.5)

transplist1 = maketable("literal", "nonorm", 0, 0, .05, .07, .07, .12, .19)
transplist2 = maketable("literal", "nonorm", 0, 0, 0, 0, .12, .12, .24)
transplist3 = maketable("literal", "nonorm", 0, 0, 0, 0, 0.01, 0.02)

ramp_01 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0)
ramp_12 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 2.0)
ramp_010 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 2, 0.0)
ramp_121 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 2.0, 2, 1.0)
ramp_10 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0)
ramp_21 = maketable("line", "nonorm", 1000, 0, 2.0, 1, 1.0)
ramp_101 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 2, 1.0)
ramp_212 = maketable("line", "nonorm", 1000, 0, 2.0, 1, 1.0, 2, 2.0)

e_DECLICK = maketable("line", "nonorm", 1000, 0, 0., 1, 1.0, 998, 1.0, 1000, 0.0)

e_CONSTANT = maketable("line", "nonorm", 1000, 0, 1., 1, 1.)

e_FADEUP = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0)
e_FADEDOWN = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0)

e_FADEINFASTEST = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 100, 1.0)
e_FADEINFAST = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 10, 1.0)

e_FADEOUTFASTEST = maketable("line", "nonorm", 1000, 0, 1.0, 99, 1.0, 100, 0.0)
e_FADEOUTFAST = maketable("line", "nonorm", 1000, 0, 1.0, 9, 1.0, 10, 0.0)

e_FADEINOUTFAST = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 9, 1.0, 10, 0.0)
e_FADEINOUTFASTEST = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 99, 1.0, 100, 0.0)

e_MIDDLEIN25 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 0.0, 3, 1.0, 5, 1.0, 7, 0.0, 8, 0.0)
e_MIDDLEIN33 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 2, 0.0)
e_MIDDLEIN50 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 2, 1.0, 3, 0.0)
e_MIDDLEIN60 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 3, 1.0, 4, 0.0)
e_MIDDLEIN66 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 4, 1.0, 5, 0.0)
e_MIDDLEIN75 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 6, 1.0, 7, 0.0)

e_MIDDLEOUT25 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 1.0, 3, 0.0, 5, 0.0, 7, 1.0, 8, 1.0)
e_MIDDLEOUT33 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 2, 1.0)
e_MIDDLEOUT50 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 2, 0.0, 3, 1.0)
e_MIDDLEOUT60 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 3, 0.0, 4, 1.0)
e_MIDDLEOUT66 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 4, 0.0, 5, 1.0)
e_MIDDLEOUT75 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 6, 0.0, 7, 1.0)

e_FADEINBY25 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 2, 1.0, 3, 1.0)
e_FADEINBY33 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 1.0, 2, 1.0)
e_FADEINBY50 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 0.0, 2, 1.0, 3, 1.0)
e_FADEINBY66 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 0.0, 2, 1.0)
e_FADEINBY75 = maketable("line", "nonorm", 1000, 0, 0.0, 1, 0.0, 2, 0.0, 3, 1.0)

e_FADEOUTBY25 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 2, 0.0, 3, 0.0)
e_FADEOUTBY33 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 0.0, 2, 0.0)
e_FADEOUTBY50 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 1.0, 2, 0.0, 3, 0.0)
e_FADEOUTBY66 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 1.0, 2, 0.0)
e_FADEOUTBY75 = maketable("line", "nonorm", 1000, 0, 1.0, 1, 1.0, 2, 1.0, 3, 0.0)
 
bus_config("NOISE", "aux 8-9 out");
bus_config("FILTERBANK", "aux 8-9 in", "aux 0-1 out");
bus_config("SPLITTER", "aux 0-1 in", "aux 2-3 out", "aux 4-5 out", "aux 6-7 out")
bus_config("MIX", "aux 2-3 in", "out 0-1")
bus_config("GVERB", "aux 4-5 in", "out 0-1")
bus_config("PANECHO", "aux 6-7 in", "aux 4-5 out");

/* print_off(); */
srand();
reset(44100);

amp_declick = maketable("line", 1000, 0.0, 0.0, 0.001, 1., 0.999, 1., 1.0, 0.);
amp_trapezoid = maketable("line", 1000, 0.0, 0.0, 50.0, 1.0, 200.0, 1., 250.0, 0.0);
amp_sharp = maketable("line", 1000, 0.0, 0.0, 0.01, 1.0, 3.0, 0.0);
amp_sharpadsr = maketable("line", 1000, 0.0, 0.0, 0.01, 1.0, 0.5, 0.7, 2., 0.7, 3.0, 0.0);
amp_sharphold = maketable("line", 1000, 0.0, 0.0, 0.01, 1.0, 2.5, 1., 3.0, 0.0);
amp_triangle = maketable("line", 1000, 0.0, 0.0, 1.0, 1.0, 2.0, 0.0);

wf_organ = maketable("wave", 1000, 1.0, 0.7, 0.4, 0.3, 0.2, 0.1, 0.05, 0.02);
wf_slowwaves = maketable("wave", 1000, 1.0, 0.3, 0.2, 0.1, 0.05);
wf_slowwaveshi = maketable("wave", 1000, 1.0, 0.1, 0.3, 0.05, 0.15, 0.025);

vel_strike = maketable("line", 1000, 0.0, 1.0, 1.0, 0.0, 10., 0.);

bus_config("WAVETABLE", "aux 0-1 out");
bus_config("NOISE", "aux 8-9 out");
bus_config("FILTERBANK", "aux 8-9 in", "aux 0-1 out");
bus_config("STEREO", "in 0", "aux 0-1 out");
bus_config("MMODALBAR", "aux 0-1 out");
bus_config("MBLOWBOTL", "aux 0-1 out");
bus_config("MMESH2D", "aux 0-1 out");
bus_config("MBANDEDWG", "aux 0-1 out");
bus_config("STRUM2", "aux 0-1 out");
bus_config("SPLITTER", "aux 0-1 in", "aux 2-3 out", "aux 4-5 out", "aux 6-7 out")
bus_config("MIX", "aux 2-3 in", "out 0-1")
bus_config("GVERB", "aux 4-5 in", "out 0-1")
bus_config("PANECHO", "aux 6-7 in", "aux 4-5 out");

bus_config("SPLITTER", "aux 0-1 in", "aux 2-3 out", "aux 4-5 out", "aux 6-7 out")
bus_config("MIX", "aux 2-3 in", "out 0-1")
bus_config("GVERB", "aux 4-5 in", "out 0-1")
bus_config("PANECHO", "aux 6-7 in", "aux 4-5 out");
bus_config("WAVETABLE", "aux 0-1 out")
bus_config("WAVESHAPE", "aux 0-1 out")
bus_config("GRANSYNTH", "aux 0-1 out")
bus_config("MBANDEDWG", "aux 0-1 out")
bus_config("MBLOWBOTL", "aux 0-1 out")
bus_config("NOISE", "aux 8-9 out");
bus_config("FILTERBANK", "aux 8-9 in", "aux 0-1 out");
bus_config("STRUM2", "aux 0-1 out")
bus_config("MMODALBAR", "aux 0-1 out")
bus_config("MMESH2D", "aux 0-1 out")
bus_config("STEREO", "in 0", "aux 0-1 out")
bus_config("TRANS3", "in 0", "aux 0-1 out")

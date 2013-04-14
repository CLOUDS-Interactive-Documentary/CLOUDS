rtsetparams(44100, 2, 256)
load("WAVETABLE")

dur = 60

wave = maketable("wave", 2000, 1)

freq = makeconnection("mouse", "x", min=200, max=2000, dflt=500, lag=50,
			"freq", "Hz", 0)

amp = makeconnection("mouse", "y", min=0, max=90, dflt=min, lag=50,
			"amp", "dB", 2)
amp = makeconverter(amp, "ampdb")

pan = makeconnection("mouse", "x", min=1, max=0, dflt=0.5, lag=50, "pan")

WAVETABLE(0, dur, amp, freq, pan, wave)


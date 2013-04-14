rtsetparams(44100, 2, 256)
load("WAVETABLE")

dur = 120

amp = makeconnection("mouse", "y", min=0, max=90, dflt=min, lag=50,
			"amp", "dB", 2)
amp = makeconverter(amp, "ampdb")
amp = makemonitor(amp, "display", "amp")

freq = makerandom("linear", rfreq=3, min=200, max=2000)
freq = makemonitor(freq, "display", "freq", "Hz", 0)

pan = makeconnection("mouse", "x", min=1, max=0, dflt=.5, lag=50, "pan")
pan = makemonitor(pan, "display", "pan")

wave = maketable("wave", 2000, 1)

WAVETABLE(0, dur, amp, freq, pan, wave)


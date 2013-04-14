// a loocher canon
rtsetparams(44100, 2)
load("TRANS")
rtinput("../../../snd/loocher.aiff")
dur = DUR()

// Express dynamic transposition in terms of linear octaves, then convert
// to octave.pc before passing to TRANS

high = octpch(0.05)
low = octpch(-0.05)
transp = maketable("line", "nonorm", 1000, 0,0, 1,low, 3,high)
transp = makeconverter(transp, "pchoct")

// shows oct.pc output
transp = makemonitor(transp, "display", "transp1", "pch")

amp = maketable("line", 1000, 0,0, .1,1, dur-.1,1, dur,0) * 0.5
pan = maketable("line", 1000, 0,0, 1,1)   // pan right to left

TRANS(0, 0, dur, amp, transp, inchan=0, pan)

transp = maketable("line", "nonorm", 1000, 0,0, 1,high, 3,low)
transp = makeconverter(transp, "pchoct")
transp = makemonitor(transp, "display", "transp2", "pch", 2)
TRANS(dur/4, 0, dur, amp, transp, inchan=0, pan)

transp = maketable("line", "nonorm", 1000, 0,1, 1,low)
transp = makeconverter(transp, "pchoct")
transp = makemonitor(transp, "display", "transp3", "pch", 3)
TRANS(dur/2, 0, dur, amp, transp, inchan=0, pan)

transp = maketable("line", "nonorm", 1000, 0,low, 1,high)
transp = makeconverter(transp, "pchoct")
transp = makemonitor(transp, "display", "transp4", "pch", 6)
TRANS(dur/1.5, 0, dur, amp, transp, inchan=0, pan)


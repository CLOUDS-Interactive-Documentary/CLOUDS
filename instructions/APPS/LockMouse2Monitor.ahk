; MouseTrap2Mon by Drugwash
; confines the mouse cursor to chosen monitor only
; Ctrl+Alt+F9 shows options dialog in NoTray mode

#persistent
SetBatchLines, -1
CoordMode, Mouse, Screen
;*****************************************
appname = MouseTrap2Mon		; application name
version = 1.2					; version number
release = July 13, 2009			; release date
type = public					; release type (internal / public)
iconlocal = MT2M.ico			; external icon for uncompiled script
debug = 0					; debug switch (1 = active)
; *****************************************
mylink = drugwash@gmail.com
ahklink = http://www.autohotkey.com
ffflink = http://www.famfamfam.com
abtwin = About %appname%
inifile := A_ScriptDir "\" appname ".ini"
;************* Custom tray menu *************
notray = 1
if ! notray
	Menu, Tray, Icon, %iconlocal%
Menu, Tray, NoStandard
Menu, Tray, Add, Settings, options
Menu, Tray, Default, Settings
Menu, Tray, Add, About, about
Menu, Tray, Add
Menu, Tray, Add, Exit, exit
if debug
	Menu, Tray, Add, Reload, Reload
; *****************************************
SysGet, a, 80 ; Check for multi-display environment
if a < 2
	{
	m=1
	MsgBox, 0x40040, %appname% information,
(
This application is designed to work
only in a multi-display environment.
We detected a single display here.

The application will now exit.
)
	ExitApp
	}
pass=0 ; pass-through switch
t=0 ; Enable/Disable toggle switch
; SysGet, p, MonitorPrimary ; not used (yet)
Gui, Add, Picture, xm ym+16 vm1 gms, M1.ico
Gui, Add, Picture, x+10 yp vm2 gms, M2.ico
Gui, Add, Button,xm y+5 w80, Disable
Gui, Add, Button,x+10 yp w80, Exit
Gui, Add, Text, xm ym h14, Click the monitor to trap the mouse in:
if FileExist(inifile)
	{
	IniRead, m, %inifile%, Settings, Monitor, 1
	IniRead, s, %inifile%, Settings, Sub, t1
	gosub msi
	}
else
	Gui, Show,, %appname%
hCurs := DllCall("LoadCursor", "UInt", NULL, "Int", 32649, "UInt") ;IDC_HAND
OnMessage(0x200, "WM_MOUSEMOVE")
OnExit, exit
return

; case 1: primary 1, selected 1 -> m=1, side=rRight, op=>
; case 2: primary 1, selected 2 -> m=2, side=rLeft, op=<
; case 3: primary 2, selected 1 -> m=1, side=rRight, op=>
; case 4: primary 2, selected 2 -> m=2, side=rLeft, op=<
ms:
m := SubStr(A_GuiControl, 2)
msi:
if s
	SetTimer, %s%, Off
SysGet, r, Monitor, %m%
side := ( m = 1) ? rRight : rLeft
;s := ( m = 1) ? "t1" : "t2"
s := "t1"
off := (m = 1) ? -5 : 5
if t
	gosub ButtonDisable
else
	SetTimer, %s%, 10
GuiClose:
GuiEscape:
Gui, Hide
return

t1:
MouseGetPos, x, y
if (x < 1920 && !pass)
	gosub chk
else if (x > 1920 && pass)
	pass = 0
return

t2:
MouseGetPos, x, y
if (x < side && !pass)
	gosub chk
else if (x > side && pass)
	pass = 0
return

chk:
if GetKeyState("Ctrl", "P")
	pass = 1
else
	DllCall("SetCursorPos", Int, 1925, Int, y)
return

^!F9::
options:
Gui, Show,, %appname%
return

ButtonDisable:
t := !t
GuiControl,, Button1, % t ? "Enable" : "Disable"
SetTimer, %s%, % t ? "Off" : "On"
Gui, Hide
return

Reload:
Reload
exit:
ButtonExit:
IniWrite, %m%, %inifile%, Settings, Monitor
IniWrite, %s%, %inifile%, Settings, Sub
OnMessage(0x200,"")
DllCall("DestroyCursor", "Uint", hCurs)
ExitApp

;************** ABOUT box *****************
about:
Gui 1:+Disabled
Gui, 1:Hide
Gui, 2:+Owner1 -MinimizeBox
if ! A_IsCompiled
	Gui, 2:Add, Picture, x69 y10 w32 h-1, %iconlocal%
else
	Gui, 2:Add, Picture, x69 y10 w32 h-1 Icon1, %A_ScriptName%
Gui, 2:Font, Bold
Gui, 2:Add, Text, x5 y+10 w160 Center, %appname% v%version%
Gui, 2:Font
Gui, 2:Add, Text, xp y+2 wp Center, by Drugwash
Gui, 2:Add, Text, xp y+2 wp Center gmail0 cBlue, %mylink%
Gui, 2:Add, Text, xp y+10 wp Center, Released %release%
Gui, 2:Add, Text, xp y+2 wp Center, (%type% version)
Gui, 2:Add, Text, xp y+10 wp Center, This product is open-source,
Gui, 2:Add, Text, xp y+2 wp Center, developed in AutoHotkey
Gui, 2:Font,CBlue Underline
Gui, 2:Add, Text, xp y+2 wp Center glink0, %ahklink%
Gui, 2:Font
Gui, 2:Add, Text, xp y+5 wp Center, Icon from the Silk package at
Gui, 2:Font,CBlue Underline
Gui, 2:Add, Text, xp y+2 wp Center glink1, %ffflink%
Gui, 2:Font
Gui, 2:Add, Button, xp+40 y+20 w80 gdismiss Default, &OK
Gui, 2:Show,, %abtwin%
return

dismiss:
Gui, 1:-Disabled
Gui, 2:Destroy
;Gui, 1:Show
return

mail0:
mail = mailto:%mylink%
Run, %mail%, UseErrorLevel
return

link0:
Run, %ahklink%,, UseErrorLevel
return

link1:
Run, %ffflink%,, UseErrorLevel
return

WM_MOUSEMOVE(wParam, lParam)
{
Global hCurs, abtwin, appname, m, side, off, pass, t
WinGetTitle, wind
MouseGetPos, x, y, winid, ctrl
if wind in %abtwin%
	if ctrl in Static4,Static9,Static11
		DllCall("SetCursor", "UInt", hCurs)
if wind in %appname%
	if ctrl in Static1,Static2
		DllCall("SetCursor", "UInt", hCurs)
return
}
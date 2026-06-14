; CLOUDS LOCAL Windows installer (Inno Setup 6)
;
; All-in-one install: binaries + CloudsData + CloudsDataMedia (as CloudsData\CloudsMedia).
; Unlike the USB installer, no thumb drive is required at runtime.
;
; Before compiling, run the staging script from this folder:
;   stage-local-release.cmd
;   stage-local-release.cmd -UseDevData
;
; That populates:
;   ..\..\LOCAL_BINARIES\
;   ..\..\CloudsData_LOCAL\          (includes CloudsMedia\ from CloudsDataMedia)
;
; Compile:
;   "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" clouds-installer-local.iss
;
; The LOCAL installer exceeds Windows' ~4 GB single-exe limit, so disk spanning is
; enabled. Distribute the entire output folder together:
;   CLOUDS-LOCAL-Setup.exe
;   CLOUDS-LOCAL-Setup-1.bin
;   CLOUDS-LOCAL-Setup-2.bin  (etc.)
;
; VC++ 2012 x64 runtime: place vcredist_x64.exe in Builds\extras\ before compiling

#define MyAppName "CLOUDS"
#define MyAppVersion "1.2.0"
#define MyAppPublisher "Serious Computational Cinema LLC"
#define MyAppExeName "CLOUDS.exe"
#define BinariesDir "..\..\LOCAL_BINARIES"
#ifndef CloudsDataDir
  #define CloudsDataDir "..\..\CloudsData_LOCAL"
#endif

[Setup]
AppId={{A7E2B4C1-9D3F-4E8A-B562-1C4F9E6D0A23}
AppName={#MyAppName} (Local)
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf32}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=..\..\output
OutputBaseFilename=CLOUDS-LOCAL-Setup
Compression=lzma2/ultra64
SolidCompression=yes
DiskSpanning=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x86 x64compatible
ArchitecturesInstallIn64BitMode=
WizardStyle=modern
; Custom wizard images must be BMP or PNG (not JPEG) at Inno's expected sizes.
; The legacy InstallShield Theme\*.jpg files cause "bitmap corrupted" at runtime.
; Omit WizardImageFile/WizardSmallImageFile to use Inno's built-in modern images.
UninstallDisplayIcon={app}\{#MyAppExeName}
DisableProgramGroupPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut for CLOUDS"; GroupDescription: "Additional shortcuts:"
Name: "openal"; Description: "Install &OpenAL audio driver (recommended)"; GroupDescription: "Additional options:"; Flags: checkedonce

[Files]
Source: "{#BinariesDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#CloudsDataDir}\*"; DestDir: "{app}\CloudsData"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\..\CLOUDS_Windows_OculusSetup.pdf"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

#include "clouds-vcredist.inc"

[Dirs]
Name: "{app}\CloudsData\fml"; Permissions: users-full
Name: "{app}\CloudsData\runs"; Permissions: users-full
Name: "{app}\CloudsData\quality"; Permissions: users-full
Name: "{app}\CloudsData\vhx"; Permissions: users-full

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\CLOUDS OCULUS"; Filename: "{app}\CLOUDS_OCULUS.exe"
Name: "{group}\Oculus Display Setup"; Filename: "{app}\CLOUDS_Windows_OculusSetup.pdf"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\oalinst.exe"; StatusMsg: "Installing OpenAL..."; Tasks: openal; Flags: waituntilterminated skipifsilent; Check: FileExists(ExpandConstant('{app}\oalinst.exe'))
Filename: "{app}\CLOUDS_Windows_OculusSetup.pdf"; Description: "View Oculus display setup guide"; Flags: postinstall shellexec skipifsilent unchecked
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: postinstall nowait skipifsilent unchecked

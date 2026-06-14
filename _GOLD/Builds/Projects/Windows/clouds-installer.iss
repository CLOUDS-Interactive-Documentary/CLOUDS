; CLOUDS USB Windows installer (Inno Setup 6)
;
; Replaces the legacy InstallShield project at USB\CLOUDS.ise
;
; Before compiling, run the staging script from this folder:
;   stage-usb-release.cmd
;   stage-usb-release.cmd -UseDevData
;
; That populates:
;   ..\..\USB_BINARIES\
;   ..\..\CloudsData\
;
; Compile:
;   "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" clouds-installer.iss
;
; Override staging paths for local dev builds:
;   ISCC.exe /DCloudsDataDir=..\..\..\CloudsData clouds-installer.iss

#define MyAppName "CLOUDS"
#define MyAppVersion "1.2.0"
#define MyAppPublisher "Serious Computational Cinema LLC"
#define MyAppExeName "CLOUDS.exe"
#define BinariesDir "..\..\USB_BINARIES"
#ifndef CloudsDataDir
  #define CloudsDataDir "..\..\CloudsData"
#endif

[Setup]
AppId={{D34864A9-A62E-4CE0-83B5-BA8118278193}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf32}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=..\..\output
OutputBaseFilename=CLOUDS-USB-Setup
Compression=lzma2/ultra64
SolidCompression=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x86 x64compatible
ArchitecturesInstallIn64BitMode=
WizardStyle=modern
; Custom wizard images must be BMP or PNG (not JPEG) at Inno's expected sizes.
; The legacy InstallShield Theme\*.jpg files cause "bitmap corrupted" at runtime.
UninstallDisplayIcon={app}\{#MyAppExeName}
DisableProgramGroupPage=no
LicenseFile=
InfoBeforeFile=

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut for CLOUDS"; GroupDescription: "Additional shortcuts:"
Name: "openal"; Description: "Install &OpenAL audio driver (recommended)"; GroupDescription: "Additional options:"; Flags: checkedonce

[Files]
; Binaries and runtime dependencies (legacy USB_BINARIES staging folder)
Source: "{#BinariesDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; Application data tree (legacy dynamic link target from InstallShield)
Source: "{#CloudsDataDir}\*"; DestDir: "{app}\CloudsData"; Flags: ignoreversion recursesubdirs createallsubdirs

; Oculus display setup guide (also copied if present in USB_BINARIES)
Source: "..\..\..\CLOUDS_Windows_OculusSetup.pdf"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

[Dirs]
; Writable data folders (chmod 777 equivalent from release checklist)
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

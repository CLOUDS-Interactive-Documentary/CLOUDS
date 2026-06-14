; Visual C++ 2012 Update 4 x64 redistributable (matches PlatformToolset v110 /MD builds)
; Place vcredist_x64.exe in Builds\extras\ before compiling (see extras\README.txt)

#ifndef VCRedistSource
  #define VCRedistSource "..\..\extras\vcredist_x64.exe"
#endif

[Files]
Source: "{#VCRedistSource}"; DestDir: "{tmp}"; DestName: "vcredist_x64.exe"; Flags: deleteafterinstall; Check: VCRedistNeedsInstall

[Run]
Filename: "{tmp}\vcredist_x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Microsoft Visual C++ 2012 Runtime (x64)..."; Check: VCRedistNeedsInstall; Flags: waituntilterminated

[Code]
function VCRedistNeedsInstall: Boolean;
var
  Version: String;
begin
  if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\11.0\VC\Runtimes\x64', 'Installed', Version) then
    Result := (Version <> '1')
  else if RegQueryStringValue(HKLM, 'SOFTWARE\WOW6432Node\Microsoft\VisualStudio\11.0\VC\Runtimes\x64', 'Installed', Version) then
    Result := (Version <> '1')
  else
    Result := True;
end;

Optional installer prerequisites
================================

Place redistributable installers here before compiling clouds-installer-local.iss.

Visual C++ 2012 Update 4 (x64) — REQUIRED for CLOUDS builds
-------------------------------------------------------------
CLOUDS is built with PlatformToolset v110 and /MD (MultiThreadedDLL).

Save the x64 redistributable as:

  Builds\extras\vcredist_x64.exe

(Not vcredist_x86.exe — CLOUDS is a 64-bit build.)

If you only have the x86 download, get vcredist_x64.exe from the same page.
stage-local-release.ps1 will also copy from VisualStudio\VS2012\vcRedist_x64.exe
if extras\vcredist_x64.exe is missing.

OpenAL — optional (also searched from bin/ and instructions/APPS/)
-------------------------------------------------------------------
  oalinst.exe

Release DLL architecture
------------------------
CLOUDS is built as x64. The Visual Studio post-build step copies DLLs from
export\vs64. Do not copy addons\ofxTimeline\libs\openal\export\vs64\OpenAL32.dll
into bin — that folder contains a 32-bit DLL despite the vs64 path name, and
shipping it next to CLOUDS.exe causes startup error 0xc000007b on clean machines.

stage-*-release.ps1 filters bin\ to x64 release DLLs only and restores x64
OpenAL32.dll from export\vs64 when needed.

CloudsDataIgnored merge
-----------------------
On dev machines, GetCloudsDataPath(true) reads from CloudsDataIgnored (fonts,
large VS assets, tunnel.mp3, etc.). On an installed Windows build both data paths
resolve to CloudsData, so stage-*-release.ps1 merges the entire
apps\CLOUDS\CloudsDataIgnored tree into staged CloudsData before compiling.

Remove-ReleaseArtifacts then strips dev-only runtime folders (fml, runs,
vhx\tokens) from the staged payload.

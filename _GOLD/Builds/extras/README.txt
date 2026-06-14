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

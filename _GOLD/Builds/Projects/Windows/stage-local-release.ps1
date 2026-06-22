# Stages CLOUDS LOCAL installer inputs into _GOLD/Builds for clouds-installer-local.iss
#
# Like the USB staging script, but also bundles CloudsDataMedia into CloudsData\CloudsMedia\
# so video assets install under Program Files instead of requiring a thumb drive.
#
# Usage:
#   .\stage-local-release.ps1
#   .\stage-local-release.ps1 -UseDevData
#   .\stage-local-release.ps1 -SkipDataScript
#   .\stage-local-release.ps1 -BinDir "D:\path\to\bin"
#
# Prerequisites:
#   - Build CLOUDS Release|x64 and Release_Oculus|x64 in Visual Studio
#   - CloudsDataMedia present at apps\CLOUDS\CloudsDataMedia

[CmdletBinding()]
param(
    [switch]$SkipDataScript,
    [switch]$UseDevData,
    [string]$BinDir = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    Write-Host ""
    Write-Host "==> $Message" -ForegroundColor Cyan
}

function Ensure-EmptyDir([string]$Path) {
    if (Test-Path $Path) {
        Remove-Item -LiteralPath $Path -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
}

function Copy-IfExists([string]$Source, [string]$Destination) {
    if (Test-Path $Source) {
        Copy-Item -LiteralPath $Source -Destination $Destination -Force
        return $true
    }
    return $false
}

function Remove-ReleaseArtifacts([string]$CloudsDataRoot) {
    $pathsToRemove = @(
        (Join-Path $CloudsDataRoot "runs"),
        (Join-Path $CloudsDataRoot "fml"),
        (Join-Path $CloudsDataRoot "vhx\tokens"),
        (Join-Path $CloudsDataRoot "vhx\Tokens")
    )

    foreach ($path in $pathsToRemove) {
        if (Test-Path $path) {
            Write-Host "  removing $path"
            Remove-Item -LiteralPath $path -Recurse -Force
        }
    }

    $linksDir = Join-Path $CloudsDataRoot "links"
    if (Test-Path $linksDir) {
        Get-ChildItem -LiteralPath $linksDir -Recurse -File -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match 'backup' } |
            ForEach-Object {
                Write-Host "  removing link backup $($_.FullName)"
                Remove-Item -LiteralPath $_.FullName -Force
            }
    }
}

function Invoke-CopyCloudsDataScript([string]$ScriptPath, [string]$WorkingDirectory) {
    $bash = $null
    foreach ($candidate in @(
        (Get-Command bash -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Source),
        "C:\Program Files\Git\bin\bash.exe",
        "C:\Program Files (x86)\Git\bin\bash.exe"
    )) {
        if ($candidate -and (Test-Path $candidate)) {
            $bash = $candidate
            break
        }
    }

    if (-not $bash) {
        throw "bash not found. Install Git for Windows or pass -UseDevData / -SkipDataScript."
    }

    Write-Host "  running $ScriptPath"
    & $bash -lc "cd '$($WorkingDirectory -replace '\\','/')' && ./copy_clouds_data.sh"
    if ($LASTEXITCODE -ne 0) {
        throw "copy_clouds_data.sh failed with exit code $LASTEXITCODE"
    }
}

function Invoke-RobocopyMirror([string]$Source, [string]$Destination) {
    Ensure-EmptyDir $Destination
    robocopy $Source $Destination /MIR /NFL /NDL /NJH /NJS /NC /NS | Out-Null
    if ($LASTEXITCODE -ge 8) {
        throw "robocopy failed copying '$Source' to '$Destination' (exit code $LASTEXITCODE)"
    }
}

. (Join-Path $PSScriptRoot "stage-release-binaries-common.ps1")

$scriptDir = $PSScriptRoot
$buildsRoot = (Resolve-Path (Join-Path $scriptDir "..\..")).Path
$goldRoot = (Resolve-Path (Join-Path $scriptDir "..\..\..")).Path
$appsClouds = (Resolve-Path (Join-Path $scriptDir "..\..\..\..")).Path
$appsRoot = Split-Path $appsClouds -Parent
$openFrameworksRoot = (Resolve-Path (Join-Path $appsRoot "..")).Path

if (-not $BinDir) {
    $BinDir = Join-Path $appsClouds "CLOUDS\bin"
}

$localBinariesDir = Join-Path $buildsRoot "LOCAL_BINARIES"
$cloudsDataDest = Join-Path $buildsRoot "CloudsData_LOCAL"
$packagedDataDir = Join-Path $appsRoot "CloudsData"
$devDataDir = Join-Path $appsClouds "CloudsData"
$cloudsMediaSource = Join-Path $appsClouds "CloudsDataMedia"
$copyDataScript = Join-Path $appsClouds "scripts\copy_clouds_data.sh"
$oculusPdf = Join-Path $goldRoot "CLOUDS_Windows_OculusSetup.pdf"

Write-Host "CLOUDS LOCAL release staging"
Write-Host "  builds : $buildsRoot"
Write-Host "  bin    : $BinDir"

Write-Step "Staging LOCAL_BINARIES"
if (-not (Test-Path $BinDir)) {
    throw "Build output not found at '$BinDir'. Build CLOUDS Release|x64 and Release_Oculus|x64 first."
}

$requiredExes = @("CLOUDS.exe", "CLOUDS_OCULUS.exe")
$missingExes = $requiredExes | Where-Object { -not (Test-Path (Join-Path $BinDir $_)) }
if ($missingExes) {
    throw "Missing build outputs in '$BinDir': $($missingExes -join ', '). Build both Release and Release_Oculus configurations."
}

Ensure-EmptyDir $localBinariesDir

foreach ($exe in $requiredExes) {
    $exePath = Join-Path $BinDir $exe
    Assert-ReleaseExeIs64 $exePath
    Copy-Item -LiteralPath $exePath -Destination $localBinariesDir
    Write-Host "  copied $exe"
}

Copy-ReleaseDllsFromBin -BinDir $BinDir -DestinationDir $localBinariesDir -OpenFrameworksRoot $openFrameworksRoot

if (Copy-IfExists $oculusPdf (Join-Path $localBinariesDir "CLOUDS_Windows_OculusSetup.pdf")) {
    Write-Host "  copied CLOUDS_Windows_OculusSetup.pdf"
}

$oalinstCandidates = @(
    (Join-Path $BinDir "oalinst.exe"),
    (Join-Path $buildsRoot "extras\oalinst.exe"),
    (Join-Path $appsClouds "instructions\APPS\oalinst.exe")
)
foreach ($candidate in $oalinstCandidates) {
    if (Copy-IfExists $candidate (Join-Path $localBinariesDir "oalinst.exe")) {
        Write-Host "  copied oalinst.exe from $candidate"
        break
    }
}

Write-Step "Staging CloudsData"
if ($UseDevData) {
    if (-not (Test-Path $devDataDir)) {
        throw "Dev CloudsData not found at '$devDataDir'"
    }
    Write-Host "  mirroring dev data from $devDataDir"
    Invoke-RobocopyMirror $devDataDir $cloudsDataDest
}
else {
    if (-not $SkipDataScript) {
        if (-not (Test-Path $copyDataScript)) {
            throw "copy_clouds_data.sh not found at '$copyDataScript'. Regenerate it from Visual System Manager or pass -SkipDataScript / -UseDevData."
        }
        Invoke-CopyCloudsDataScript $copyDataScript (Split-Path $copyDataScript -Parent)
    }

    if (-not (Test-Path $packagedDataDir)) {
        throw "Packaged CloudsData not found at '$packagedDataDir'. Run copy_clouds_data.sh or pass -UseDevData."
    }

    Write-Host "  mirroring packaged data from $packagedDataDir"
    Invoke-RobocopyMirror $packagedDataDir $cloudsDataDest
}

Merge-CloudsDataIgnoredIntoRelease -CloudsDataDest $cloudsDataDest -AppsClouds $appsClouds

Write-Step "Applying release exclusions"
Remove-ReleaseArtifacts $cloudsDataDest

Write-Step "Staging CloudsDataMedia -> CloudsData\CloudsMedia"
if (-not (Test-Path $cloudsMediaSource)) {
    throw "CloudsDataMedia not found at '$cloudsMediaSource'. Required for the LOCAL installer."
}

$cloudsMediaDest = Join-Path $cloudsDataDest "CloudsMedia"
Write-Host "  mirroring $cloudsMediaSource"
Write-Host "         to $cloudsMediaDest"
Invoke-RobocopyMirror $cloudsMediaSource $cloudsMediaDest

Write-Step "Ensuring VC++ redist in extras"
$extrasDir = Join-Path $buildsRoot "extras"
$vcRedistDest = Join-Path $extrasDir "vcredist_x64.exe"
$cloudsRoot = (Resolve-Path (Join-Path $buildsRoot "..\..\..\..\..\..")).Path

if (-not (Test-Path $vcRedistDest)) {
    $candidates = @(
        (Join-Path $extrasDir "vcRedist_x64.exe"),
        (Join-Path $cloudsRoot "VisualStudio\VS2012\vcRedist_x64.exe")
    )
    $copied = $false
    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            Copy-Item -LiteralPath $candidate -Destination $vcRedistDest -Force
            Write-Host "  copied vcredist_x64.exe from $candidate"
            $copied = $true
            break
        }
    }
    if (-not $copied) {
        Write-Host "  WARNING: vcredist_x64.exe not found in extras or VisualStudio\VS2012." -ForegroundColor Yellow
        Write-Host "           Download vcredist_x64.exe into Builds\extras\ before compiling." -ForegroundColor Yellow
        Write-Host "           See Builds\extras\README.txt" -ForegroundColor Yellow
    }
} else {
    Write-Host "  found vcredist_x64.exe"
}

Write-Step "Done"
Write-Host "  LOCAL_BINARIES : $localBinariesDir"
Write-Host "  CloudsData     : $cloudsDataDest"
Write-Host "  CloudsMedia    : $cloudsMediaDest"
Write-Host ""
Write-Host "Next: compile clouds-installer-local.iss with Inno Setup 6" -ForegroundColor Green
Write-Host "Note: LOCAL output spans multiple files (Setup.exe + Setup-*.bin)." -ForegroundColor Yellow
Write-Host "      Distribute the whole Builds\output folder together." -ForegroundColor Yellow

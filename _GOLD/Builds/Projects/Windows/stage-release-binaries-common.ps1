# Shared helpers for stage-usb-release.ps1 and stage-local-release.ps1

function Get-PeArchitecture([string]$Path) {
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 0x40) {
        return $null
    }

    $peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
    if ($peOffset -lt 0 -or ($peOffset + 6) -ge $bytes.Length) {
        return $null
    }

    $machine = [BitConverter]::ToUInt16($bytes, $peOffset + 4)
    switch ($machine) {
        0x8664 { return "x64" }
        0x014C { return "x86" }
        default { return "unknown" }
    }
}

function Test-ReleaseDll([System.IO.FileInfo]$Dll) {
    if ($Dll.Name -match '(?i)(^glut32\.dll$|D\.dll$)') {
        return $false
    }

    $arch = Get-PeArchitecture $Dll.FullName
    return ($arch -eq "x64")
}

function Copy-ReleaseDllsFromBin([string]$BinDir, [string]$DestinationDir, [string]$OpenFrameworksRoot) {
    $skipped = @()
    Get-ChildItem -LiteralPath $BinDir -File -Filter "*.dll" | ForEach-Object {
        if (Test-ReleaseDll $_) {
            Copy-Item -LiteralPath $_.FullName -Destination $DestinationDir
            Write-Host "  copied $($_.Name)"
        }
        else {
            $arch = Get-PeArchitecture $_.FullName
            $skipped += "$($_.Name) ($arch)"
        }
    }

    $openAlSource = Join-Path $OpenFrameworksRoot "export\vs64\OpenAL32.dll"
    if (-not (Test-Path (Join-Path $DestinationDir "OpenAL32.dll"))) {
        if (Test-Path $openAlSource) {
            $openAlArch = Get-PeArchitecture $openAlSource
            if ($openAlArch -ne "x64") {
                throw "Expected x64 OpenAL32.dll at '$openAlSource' but found $openAlArch."
            }
            Copy-Item -LiteralPath $openAlSource -Destination $DestinationDir
            Write-Host "  copied OpenAL32.dll from export\vs64"
        }
        else {
            throw "OpenAL32.dll is required but was not found in '$BinDir' or '$openAlSource'."
        }
    }

    if ($skipped.Count -gt 0) {
        Write-Host "  skipped non-release DLLs: $($skipped -join ', ')" -ForegroundColor Yellow
    }
}

function Assert-ReleaseExeIs64([string]$ExePath) {
    $arch = Get-PeArchitecture $ExePath
    if ($arch -ne "x64") {
        throw "Expected x64 executable at '$ExePath' but found $arch."
    }
}

function Merge-CloudsDataIgnoredIntoRelease([string]$CloudsDataDest, [string]$AppsClouds) {
    $ignoredRoot = Join-Path $AppsClouds "CloudsDataIgnored"
    if (-not (Test-Path $ignoredRoot)) {
        throw "CloudsDataIgnored not found at '$ignoredRoot'. Required for release staging."
    }

    Write-Host "  merging CloudsDataIgnored into staged CloudsData"

    Get-ChildItem -LiteralPath $ignoredRoot | ForEach-Object {
        $destPath = Join-Path $CloudsDataDest $_.Name
        if ($_.PSIsContainer) {
            if (-not (Test-Path $destPath)) {
                New-Item -ItemType Directory -Path $destPath -Force | Out-Null
            }

            robocopy $_.FullName $destPath /E /NFL /NDL /NJH /NJS /NC /NS | Out-Null
            if ($LASTEXITCODE -ge 8) {
                throw "robocopy failed merging '$($_.Name)' from CloudsDataIgnored (exit code $LASTEXITCODE)"
            }

            Write-Host "    merged $($_.Name)/"
        }
        else {
            Copy-Item -LiteralPath $_.FullName -Destination $destPath -Force
            Write-Host "    merged $($_.Name)"
        }
    }

    $requiredFont = Join-Path $CloudsDataDest "flaunt\RenderStadium.bin"
    if (-not (Test-Path $requiredFont)) {
        throw @"
Missing encrypted fonts at '$requiredFont'.
CLOUDS loads Blender-MEDIUM.ttf from CloudsData\flaunt\ at startup; without these .bin files the app crashes on launch.
Ensure apps\CLOUDS\CloudsDataIgnored\flaunt exists before staging.
"@
    }
}

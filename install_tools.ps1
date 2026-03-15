# Define paths
$devToolsPath = "F:\DevelopmentTools"
$projectPath = "C:\Users\Samiul\Desktop\FinalProject"
$winlibsUrl = "https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0-16.0.6-11.0.1-msvcrt-r1/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64msvcrt-11.0.1-r1.zip"
$sfmlUrl = "https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip"

# Create directory
if (!(Test-Path $devToolsPath)) {
    New-Item -ItemType Directory -Path $devToolsPath | Out-Null
}

Write-Host "Step 1/4: Downloading GCC 13.2.0 (WinLibs)... This may take a few minutes."
$winlibsZip = "$devToolsPath\winlibs.zip"
Invoke-WebRequest -Uri $winlibsUrl -OutFile $winlibsZip

Write-Host "Step 2/4: Extracting GCC..."
Expand-Archive -Path $winlibsZip -DestinationPath $devToolsPath -Force
# Rename/Move if needed, but WinLibs usually extracts to 'mingw64'
# Result should be F:\DevelopmentTools\mingw64\bin\g++.exe

Write-Host "Step 3/4: Downloading SFML 2.6.1 (64-bit)..."
$sfmlZip = "$devToolsPath\sfml.zip"
Invoke-WebRequest -Uri $sfmlUrl -OutFile $sfmlZip

Write-Host "Step 4/4: Setting up SFML..."
$sfmlExtractPath = "$devToolsPath\SFML_Temp"
Expand-Archive -Path $sfmlZip -DestinationPath $sfmlExtractPath -Force

# Find the inner folder (e.g., SFML-2.6.1-...)
$sfmlRoot = Get-ChildItem -Path $sfmlExtractPath -Directory | Select-Object -First 1
$sfmlRootPath = $sfmlRoot.FullName

# Copy Libs to Project
Write-Host "Copying libraries to project..."
Copy-Item -Path "$sfmlRootPath\lib\*" -Destination "$projectPath\lib" -Force

# Copy DLLs to Project Root
Write-Host "Copying DLLs to project root..."
Copy-Item -Path "$sfmlRootPath\bin\*.dll" -Destination "$projectPath" -Force

# Cleanup
Remove-Item $winlibsZip
Remove-Item $sfmlZip
Remove-Item $sfmlExtractPath -Recurse -Force

Write-Host "Installation Complete! GCC 13.2.0 and SFML 64-bit are ready."

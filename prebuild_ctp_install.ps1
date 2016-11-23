function GetUninstallString($productName) {
    $x64items = @(Get-ChildItem "HKLM:SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall")
    ($x64items + @(Get-ChildItem "HKLM:SOFTWARE\wow6432node\Microsoft\Windows\CurrentVersion\Uninstall") `
        | ForEach-object { Get-ItemProperty Microsoft.PowerShell.Core\Registry::$_ } `
        | Where-Object { $_.DisplayName -and $_.DisplayName.Contains($productName) } `
        | Select UninstallString).UninstallString
}

Write-Host "Uninstalling Microsoft Visual C++ 2013 Redistributable (x64)..."
"$(GetUninstallString "Microsoft Visual C++ 2013 Redistributable (x64)") /quiet" | out-file "$env:temp\uninstall.cmd" -Encoding ASCII
& $env:temp\uninstall.cmd

Write-Host "Uninstalling Microsoft Visual C++ 2013 Redistributable (x86)..."
"$(GetUninstallString "Microsoft Visual C++ 2013 Redistributable (x86)") /quiet" | out-file "$env:temp\uninstall.cmd" -Encoding ASCII
& $env:temp\uninstall.cmd

Write-Host "Downloading Visual C++ Compiler November 2013 CTP..." -ForegroundColor Cyan
$exePath = "$($env:TEMP)\vc_CompilerCTP.Nov2013.exe"
(New-Object Net.WebClient).DownloadFile('https://download.microsoft.com/download/3/C/2/3C271B79-6354-4B66-9014-C6CEBC14C5C4/vc_CompilerCTP.Nov2013.exe', $exePath)

Write-Host "Installing..."
cmd /c start /wait $exePath /install /quiet /norestart /log c:\users\appveyor\install-log.txt

Write-Host "Installed" -ForegroundColor green


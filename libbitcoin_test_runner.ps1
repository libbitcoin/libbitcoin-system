$BIN_PATH = "c:\git\libbitcoin\bin"
$TEST_EXE_NAME = "libbitcoin-test.exe"
$TEST_ARGS = "--run_test=* --show_progress=no --detect_memory_leak=0 --report_level=no --build_info=yes" 

Write-Host "Locating test executables..." -ForegroundColor Cyan 

$DISCOVERED_EXES = @(Get-ChildItem -Path $BIN_PATH  -recurse | Where-Object { $_.Name -eq $TEST_EXE_NAME }) 

If ($DISCOVERED_EXES.Count -ne 1) {
  Write-Host "Failure, invalid count of test elements" -ForegroundColor Red
  exit 1
}

Write-Host "Found single instance: " $DISCOVERED_EXES.Name -ForegroundColor Cyan

$DISCOVERED_EXES.FullName | ForEach-Object {
  Write-Host "Executing $_ $TEST_ARGS" -ForegroundColor Cyan
  try {
    Invoke-Expression "$_ $TEST_ARGS"
  }
  catch {
    $error = $_
    Write-Host "Failure executing tests: " $error -ForegroundColor Red
    exit $error
  }
}

Write-Host "Complete." -ForegroundColor Green


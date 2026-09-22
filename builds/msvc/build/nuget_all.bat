@ECHO OFF
ECHO Downloading libbitcoin vs2026 dependencies from NuGet
CALL nuget.exe install ..\vs2026\libbitcoin-system\packages.config
CALL nuget.exe install ..\vs2026\libbitcoin-system-examples\packages.config
CALL nuget.exe install ..\vs2026\libbitcoin-system-test\packages.config

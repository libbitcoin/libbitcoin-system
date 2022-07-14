@ECHO OFF
ECHO Downloading libbitcoin vs2022 dependencies from NuGet
CALL nuget.exe install ..\vs2022\libbitcoin-system\packages.config
CALL nuget.exe install ..\vs2022\libbitcoin-system-examples\packages.config
CALL nuget.exe install ..\vs2022\libbitcoin-system-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2019 dependencies from NuGet
CALL nuget.exe install ..\vs2019\libbitcoin-system\packages.config
CALL nuget.exe install ..\vs2019\libbitcoin-system-examples\packages.config
CALL nuget.exe install ..\vs2019\libbitcoin-system-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2017 dependencies from NuGet
CALL nuget.exe install ..\vs2017\libbitcoin-system\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-system-examples\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-system-test\packages.config

@ECHO OFF
ECHO Downloading libbitcoin vs2017 dependencies from NuGet
CALL nuget.exe install ..\vs2017\libbitcoin\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-examples\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2015 dependencies from NuGet
CALL nuget.exe install ..\vs2015\libbitcoin\packages.config
CALL nuget.exe install ..\vs2015\libbitcoin-examples\packages.config
CALL nuget.exe install ..\vs2015\libbitcoin-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2013 dependencies from NuGet
CALL nuget.exe install ..\vs2013\libbitcoin\packages.config
CALL nuget.exe install ..\vs2013\libbitcoin-examples\packages.config
CALL nuget.exe install ..\vs2013\libbitcoin-test\packages.config

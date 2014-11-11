@ECHO OFF
ECHO.
ECHO Downloading libbitcoin dependencies from NuGet
CALL nuget.exe install ..\vs2013\libbitcoin\packages.config
CALL nuget.exe install ..\vs2013\libbitcoin-test\packages.config
ECHO.
CALL buildbase.bat ..\vs2013\libbitcoin.sln 12
ECHO.
PAUSE
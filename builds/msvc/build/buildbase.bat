@ECHO OFF
REM Usage: [buildbase.bat ..\vs2013\mysolution.sln 12]

SET solution=%1
SET version=%2
SET log=build_%version%.log
SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat
SET environment="%programfiles(x86)%\%tools%"
IF NOT EXIST %environment% SET environment="%programfiles%\%tools%"
IF NOT EXIST %environment% GOTO no_tools

ECHO Building: %solution%

CALL %environment% x86 > nul
ECHO Platform=x86

ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error

CALL %environment% x86_amd64 > nul
ECHO Platform=x64

ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=x64 %solution% >> %log%
IF errorlevel 1 GOTO error

ECHO Complete: %solution%
GOTO end

:error
ECHO *** ERROR, build terminated early, see: %log%
GOTO end

:no_tools
ECHO *** ERROR, build tools not found: %tools%

:end


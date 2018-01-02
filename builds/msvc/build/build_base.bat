@ECHO OFF
REM Usage: [buildbase.bat vs2017 libbitcoin "Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"]
REM Usage: [buildbase.bat vs2015 libbitcoin "Microsoft Visual Studio 14.0\VC"]
REM Usage: [buildbase.bat vs2013 libbitcoin "Microsoft Visual Studio 12.0\VC"]

SET studio=%1
SET project=%2
SET version=%~3

SET log=%studio%.log
SET solution=..\%studio%\%project%.sln
SET tools=%version%\vcvarsall.bat
SET environment=%programfiles(x86)%\%tools%

IF NOT EXIST "%environment%" SET environment=%programfiles%\%tools%
IF NOT EXIST "%environment%" GOTO no_tools

ECHO Building: %solution%

CALL "%environment%" x86 > nul
ECHO Platform=x86

ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %solution% >> %log%
IF errorlevel 1 GOTO error

CALL "%environment%" x86_amd64 > nul
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


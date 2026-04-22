@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions
REM ###########################################################################
REM #  Copyright (c) 2014-2026 libbitcoin-system developers (see COPYING).
REM #
REM #         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
REM #
REM ###########################################################################

REM Script managing the build of libbitcoin-system.
REM
REM Script options:
REM --build-config config       Build configuration.
REM --build-platform platform   Build platform.
REM --build-version version     Build MSVC version.
REM --build-mode mode           Determines action on target.
REM                               Default: Rebuild
REM --build-symbols mode        Determines treatment of symbols.
REM                               Values: { default, disabled, public-only }
REM --build-src-dir path        Location for sources.
REM                               Default: !CD!
REM --build-full-repositories   Sync full github repositories.
REM                               Default: git clone --depth 1 --single-branch
REM --build-skip-tests          Skip test compilation and execution.
REM --build-use-local-src       Use existing sources in relevant paths.
REM --verbose                   Display verbose script output.
REM --help, -h                  Display usage, overriding script execution.
REM

if "!MSBUILD_EXE!" == "" (
    set "MSBUILD_EXE=msbuild"
)

if "!NUGET_EXE!" == "" (
    set "NUGET_EXE=nuget"
)

if "!libbitcoin_system_OWNER!" == "" (
    set "libbitcoin_system_OWNER=libbitcoin"
)
if "!libbitcoin_system_TAG!" == "" (
    set "libbitcoin_system_TAG=master"
)

:main
    call :parse_input %*

    if "!DISPLAY_VERBOSE!" == "yes" (
        call :msg_heading "Pre-sanitized state"
        call :display_build_variables
        call :display_toolchain_variables
        call :display_constants
    )

    if "!DISPLAY_HELP!" == "yes" (
        call :help
        exit /b 0
    )

    if "!BUILD_SRC_DIR!" == "" (
        set "BUILD_SRC_DIR=!CD!"
        call :msg_warn "No source directory specified, using '!BUILD_SRC_DIR!'."
    ) else (
        call :push_directory "!BUILD_SRC_DIR!"
        if %ERRORLEVEL% neq 0 (
            exit /b %ERRORLEVEL%
        )
        set "BUILD_SRC_DIR=!CD!"
        call :msg_verbose "Resolving potential relative path '!BUILD_SRC_DIR!'."
        call :pop_directory
        if %ERRORLEVEL% neq 0 (
            exit /b %ERRORLEVEL%
        )
    )

    if "!NUGET_PKG_PATH!" == "" (
        set "NUGET_PKG_PATH=!BUILD_SRC_DIR!\.nuget\packages"
    )

    if not exist "!NUGET_PKG_PATH!" (
        call :msg_warn "NUGET_PKG_PATH does not exist, creating..."
        call :create_directory_force "!NUGET_PKG_PATH!"
        call :msg_success "NUGET_PKG_PATH: '%NUGET_PKG_PATH%' created successfully."
        if %ERRORLEVEL% neq 0 (
            exit /b %ERRORLEVEL%
        )
    ) else (
        call :msg_warn "NUGET_PKG_PATH exists, reusing..."
    )

    if "!BUILD_CONFIG!" == "" (
        call :msg_error "Build configuration required."
        call :help
        exit /b 1
    )

    if "!BUILD_VERSION!" == "" (
        set "BUILD_VERSION=vs2022"
        call :msg_warn "Build msvc version not provided, defaulting to '!BUILD_VERSION!'."
    )

    if "!BUILD_PLATFORM!" == "" (
        set "BUILD_PLATFORM=x64"
        call :msg_warn "Build platform not provided, defaulting to '!BUILD_PLATFORM!'."
    )

    if "!MSBUILD_VERBOSE!" == "" (
        if "!DISPLAY_VERBOSE!" == "yes" (
            set "MSBUILD_VERBOSE=diagnostic"
        ) else (
            set "MSBUILD_VERBOSE=minimal"
        )
    )

    if "!BUILD_SYMBOLS!" == "" (
        call :msg_verbose "BUILD_SYMBOLS not defined or empty, defaulting to 'default' behavior."
    ) else if "!BUILD_SYMBOLS!" == "default" (
        call :msg_verbose "BUILD_SYMBOLS 'default'."
    ) else if "!BUILD_SYMBOLS!" == "disabled" (
        call :msg_verbose "BUILD_SYMBOLS 'disabled'."
    ) else if "!BUILD_SYMBOLS!" == "public-only" (
        call :msg_verbose "BUILD_SYMBOLS 'public-only'."
    ) else (
        call :msg_error "BUILD_SYMBOLS value '!BUILD_SYMBOLS!' unrecognized."
        call :help
        exit /b 1
    )

    call :msg_heading "Configuration"
    call :display_build_variables

    call :msg_heading "Toolchain Configuration Parameters"
    call :display_toolchain_variables

    if "!DISPLAY_VERBOSE!" == "yes" (
        call :msg_heading "State"
        call :display_constants
    )

    call :push_directory "!BUILD_SRC_DIR!"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )

    call :source_github "!libbitcoin_system_OWNER!" "libbitcoin-system" "!libbitcoin_system_TAG!"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    call :build_msbuild "libbitcoin-system" "builds\msvc\%proj_version%" "libbitcoin-system"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    call :build_msbuild "libbitcoin-system" "builds\msvc\%proj_version%" "libbitcoin-system-examples"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    if not "!BUILD_SKIP_TESTS!" == "yes" (
        call :build_msbuild "libbitcoin-system" "builds\msvc\%proj_version%" "libbitcoin-system-test"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    )

    call :pop_directory
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )

    call :msg_success "Build process completed successfully."
    if not defined CI (
        pause
    )

    exit /b 0

:parse_input
    if "%~1" == "" (
        goto :end_parse_input
    ) else if "%~1" == "--build-config" (
        set "BUILD_CONFIG=%~2"
        shift
    ) else if "%~1" == "--build-platform" (
        set "BUILD_PLATFORM=%~2"
        shift
    ) else if "%~1" == "--build-version" (
        set "BUILD_VERSION=%~2"
        shift
    ) else if "%~1" == "--build-mode" (
        set "BUILD_MODE=%~2"
        shift
    ) else if "%~1" == "--build-symbols" (
        set "BUILD_SYMBOLS=%~2"
        shift
    ) else if "%~1" == "--build-src-dir" (
        set "BUILD_SRC_DIR=%~2"
        shift
    ) else if "%~1" == "--build-full-repositories" (
        set "BUILD_FULL_REPOSITORIES=yes"
    ) else if "%~1" == "--build-skip-tests" (
        set "BUILD_SKIP_TESTS=yes"
    ) else if "%~1" == "--build-use-local-src" (
        set "BUILD_USE_LOCAL_SRC=yes"
    ) else if "%~1" == "--verbose" (
        set "DISPLAY_VERBOSE=yes"
    ) else if "%~1" == "--help" (
        set "DISPLAY_HELP=yes"
    ) else if "%~1" == "-h" (
        set "DISPLAY_HELP=yes"
    ) else (
        if "!UNHANDLED_ARGS!" == "" (
            set "UNHANDLED_ARGS=%1"
        ) else (
            set "UNHANDLED_ARGS=!UNHANDLED_ARGS! %1"
        )
    )

    shift
    goto :parse_input

:end_parse_input
    exit /b 0


:source_github
    set "OWNER=%~1"
    set "REPOSITORY=%~2"
    set "TAG=%~3"
    set "CLONE_OPTIONS=--depth 1 --single-branch"

    if "!BUILD_FULL_REPOSITORIES!" == "yes" (
        set "CLONE_OPTIONS="
    )

    call :msg_heading "Preparing to aquire %OWNER%/%REPOSITORY%/%TAG%"
    call :push_directory "!BUILD_SRC_DIR!"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )

    if exist "%REPOSITORY%" (
        if "!BUILD_USE_LOCAL_SRC!" == "yes" (
            call :msg_warn "Reusing existing '%REPOSITORY%'..."
            call :pop_directory
            exit /b %ERRORLEVEL%
        )

        call :msg_warn "Encountered existing '%REPOSITORY%' directory, removing..."
        call :remove_directory_force "%REPOSITORY%"
        if %ERRORLEVEL% neq 0 (
            exit /b %ERRORLEVEL%
        )
    )

    call :msg "Cloning %OWNER%/%REPOSITORY%/%TAG%..."
    call git clone !CLONE_OPTIONS! --branch %TAG% "https://github.com/%OWNER%/%REPOSITORY%"
    if %ERRORLEVEL% neq 0 (
        call :msg_error "Clone of '%OWNER%/%REPOSITORY%/%TAG%' failed."
        call :pop_directory
        exit /b 1
    )

    call :pop_directory
    exit /b %ERRORLEVEL%

:build_msbuild
    set "PROJECT=%~1"
    set "RELATIVE_PATH=%~2"
    set "TARGET=%~3"

    if not "%TARGET%" == "" (
        if not "!BUILD_MODE!" == "" (
            set "TARGET_ARG=/target:%TARGET%:!BUILD_MODE!"
        ) else (
            set "TARGET_ARG=/target:%TARGET%"
        )
    ) else (
        if not "!BUILD_MODE!" == "" (
            set "TARGET_ARG=/target:!BUILD_MODE!"
        )
    )

    set "SYMBOLS_ARG="
    if "!BUILD_SYMBOLS!" == "disabled" (
        set "SYMBOLS_ARG=/p:LinkerOptions=/PDBSTRIPPED "
    ) else if "!BUILD_SYMBOLS!" == "public-only" (
        set "SYMBOLS_ARG=/p:DebugSymbols=false "
    )

    call :push_directory "!BUILD_SRC_DIR!\%PROJECT%\%RELATIVE_PATH%\!BUILD_VERSION!"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )

    call :nuget_restore "%PROJECT%"
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )

    !MSBUILD_EXE! /verbosity:!MSBUILD_VERBOSE! !SYMBOLS_ARG!/p:Platform=!BUILD_PLATFORM! /p:Configuration=!BUILD_CONFIG! /p:PreferredToolArchitecture=x64 %TARGET_ARG% %PROJECT%.sln /p:PreBuildEventUseInBuild=false /p:PostBuildEventUseInBuild=false !UNHANDLED_ARGS!

    if %ERRORLEVEL% neq 0 (
        call :msg_error "!MSBUILD_EXE! /verbosity:!MSBUILD_VERBOSE! !SYMBOLS_ARG!/p:Platform=!BUILD_PLATFORM! /p:Configuration=!BUILD_CONFIG! /p:PreferredToolArchitecture=x64 %TARGET_ARG% %PROJECT%.sln /p:PreBuildEventUseInBuild=false /p:PostBuildEventUseInBuild=false !UNHANDLED_ARGS!"
        call :pop_directory
        exit /b 1
    )

    call :msg_success "'%PROJECT%' built successfully."
    call :pop_directory
    exit /b %ERRORLEVEL%

:nuget_restore
    call :msg_verbose "nuget restore %~1.sln to !NUGET_PKG_PATH!"
    !NUGET_EXE! restore "%~1.sln" -OutputDirectory !NUGET_PKG_PATH!
    if %ERRORLEVEL% neq 0 (
        call :msg_error "nuget restore failed."
        exit /b 1
    )

    call :msg_success "nuget restoration for %~1 complete."
    exit /b %ERRORLEVEL%

:display_build_variables
    call :msg "BUILD_CONFIG                    : !BUILD_CONFIG!"
    call :msg "BUILD_PLATFORM                  : !BUILD_PLATFORM!"
    call :msg "BUILD_VERSION                   : !BUILD_VERSION!"
    call :msg "BUILD_SRC_DIR                   : !BUILD_SRC_DIR!"
    call :msg "BUILD_FULL_REPOSITORIES         : !BUILD_FULL_REPOSITORIES!"
    call :msg "BUILD_USE_LOCAL_SRC             : !BUILD_USE_LOCAL_SRC!"
    call :msg "BUILD_MODE                      : !BUILD_MODE!"
    call :msg "BUILD_SYMBOLS                   : !BUILD_SYMBOLS!"
    call :msg "DISPLAY_VERBOSE                 : !DISPLAY_VERBOSE!"
    call :msg "DISPLAY_HELP                    : !DISPLAY_HELP!"
    exit /b %ERRORLEVEL%


:display_toolchain_variables
    call :msg "MSBUILD_EXE                     : !MSBUILD_EXE!"
    call :msg "NUGET_PKG_PATH                  : !NUGET_PKG_PATH!"
    exit /b %ERRORLEVEL%

:display_constants
    call :msg "libbitcoin_system_OWNER         : !libbitcoin_system_OWNER!"
    call :msg "libbitcoin_system_TAG           : !libbitcoin_system_TAG!"
    exit /b %ERRORLEVEL%

:help
    call :msg "Script managing the build of libbitcoin-system."
    call :msg ""
    call :msg "Script options:"
    call :msg "--build-config config       Build configuration."
    call :msg "--build-platform platform   Build platform."
    call :msg "--build-version version     Build MSVC version."
    call :msg "--build-mode mode           Determines action on target."
    call :msg "                              Default: Rebuild"
    call :msg "--build-symbols mode        Determines treatment of symbols."
    call :msg "                              Values: { default, disabled, public-only }"
    call :msg "--build-src-dir path        Location for sources."
    call :msg "                              Default: !CD!"
    call :msg "--build-full-repositories   Sync full github repositories."
    call :msg "                              Default: git clone --depth 1 --single-branch"
    call :msg "--build-skip-tests          Skip test compilation and execution."
    call :msg "--build-use-local-src       Use existing sources in relevant paths."
    call :msg "--verbose                   Display verbose script output."
    call :msg "--help, -h                  Display usage, overriding script execution."
    exit /b %ERRORLEVEL%

:create_directory
    set "CD_DIRECTORY=%~1"
    set "CD_MODE=%~2"

    if exist "!CD_DIRECTORY!" (
        if "!CD_MODE!" == "-f" (
            call :msg_warn "Reinitializing '%CD_DIRECTORY%'..."
            rmdir /S /Q "!CD_DIRECTORY!"
            if %ERRORLEVEL% neq 0 (
                exit /b %ERRORLEVEL%
            )

            mkdir "!CD_DIRECTORY!"
        ) else (
            call :msg_warn "Reusing existing '!CD_DIRECTORY!'..."
        )
    ) else (
        call :msg "Initializing '!CD_DIRECTORY!'..."
    )
    exit /b %ERRORLEVEL%

:create_directory_force
    call :create_directory "%~1" -f
    exit /b %ERRORLEVEL%

:pop_directory
    call :msg_verbose "*** move  pre: '!CD!'"
    popd
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    call :msg_verbose "*** move post: '!CD!'"
    exit /b %ERRORLEVEL%

:push_directory
    call :msg_verbose "*** move  pre: '!CD!'"
    pushd %1
    if %ERRORLEVEL% neq 0 (
        exit /b %ERRORLEVEL%
    )
    call :msg_verbose "*** move post: '!CD!'"
    exit /b %ERRORLEVEL%

:remove_directory_force
    call :msg_verbose "*** removing: '%~1'"
    rmdir /S /Q "%~1"
    exit /b %ERRORLEVEL%

:msg_heading
    call :msg "***************************************************************************"
    call :msg "%~1"
    call :msg "***************************************************************************"
    exit /b %ERRORLEVEL%

:msg
    if "%~1" == "" (
        echo.
    ) else (
        echo %~1
    )
    exit /b %ERRORLEVEL%

:msg_empty
    echo.
    exit /b %ERRORLEVEL%

:msg_verbose
    if "!DISPLAY_VERBOSE!" == "yes" (
        echo [96m%~1[0m
    )
    exit /b %ERRORLEVEL%

:msg_success
    echo [92m%~1[0m
    exit /b %ERRORLEVEL%

:msg_warn
    echo [93m%~1[0m
    exit /b %ERRORLEVEL%

:msg_error
    echo [91m%~1[0m
    exit /b %ERRORLEVEL%

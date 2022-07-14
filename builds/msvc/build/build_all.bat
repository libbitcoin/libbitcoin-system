@ECHO OFF
CALL nuget_all.bat
ECHO.
CALL build_base.bat vs2022 libbitcoin-system "Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
CALL build_base.bat vs2019 libbitcoin-system "Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
CALL build_base.bat vs2017 libbitcoin-system "Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
REM CALL build_base.bat vs2015 libbitcoin-system "Microsoft Visual Studio 14.0\VC"
REM CALL build_base.bat vs2013 libbitcoin-system "Microsoft Visual Studio 12.0\VC"
PAUSE

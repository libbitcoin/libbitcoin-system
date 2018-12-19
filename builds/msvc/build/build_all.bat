@ECHO OFF
CALL nuget_all.bat
ECHO.
CALL build_base.bat vs2017 libbitcoin-system "Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
CALL build_base.bat vs2015 libbitcoin-system "Microsoft Visual Studio 14.0\VC"
CALL build_base.bat vs2013 libbitcoin-system "Microsoft Visual Studio 12.0\VC"
PAUSE

@echo off


echo.
echo ===== Deleting build and binary folders...
pushd ..\..\
if exist _build echo Deleting: _build
if exist _build call rmdir /S /q _build
if exist _bin echo Deleting: _bin
if exist _bin call rmdir /S /q _bin
popd


echo.
echo ===== Deleting Visual Studio files...
echo todo...


echo.
echo ===== Done.
pause

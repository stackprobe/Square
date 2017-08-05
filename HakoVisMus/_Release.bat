C:\Factory\Tools\RDMD.exe /RC out

COPY /B HakoVisMus.exe out\マウスカーソル表示パッチ.exe
COPY /B Readme.txt out\お読み下さい！.txt

C:\Factory\SubTools\EmbedConfig.exe --factory-dir-disabled out\マウスカーソル表示パッチ.exe

C:\Factory\SubTools\zip.exe /O out HakoVisMus

PAUSE

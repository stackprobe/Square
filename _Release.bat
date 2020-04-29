C:\Factory\Tools\RDMD.exe /RC out

SET RAWKEY=68a89abc4ca13003ce678362c38838cf

C:\Factory\SubTools\makeCluster.exe Picture.txt     out\Picture.dat     %RAWKEY% 110000000
C:\Factory\SubTools\makeCluster.exe Music.txt       out\Music.dat       %RAWKEY% 120000000
C:\Factory\SubTools\makeCluster.exe SoundEffect.txt out\SoundEffect.dat %RAWKEY% 130000000
C:\Factory\SubTools\makeCluster.exe Etcetera.txt    out\Etcetera.dat    %RAWKEY% 140000000

MD out\Map
FOR /L %%C IN (1, 1, 9) DO COPY /B Map\Dummy.bmp out\Map\000%%C.bmp

MD out\Replay
COPY /B res\_Dummy_.txt out\Replay

COPY /B Square\Release\Square.exe out\Hako.exe
COPY /B AUTHORS out
C:\Factory\Tools\xcp.exe doc out
ren out\Manual.txt ƒ}ƒjƒ…ƒAƒ‹.txt

out\Hako.exe /L
IF ERRORLEVEL 1 START ?_LOG_ENABLED

C:\Factory\SubTools\zip.exe /G out Hako
C:\Factory\Tools\summd5.exe /M out

IF NOT "%1" == "/-P" PAUSE

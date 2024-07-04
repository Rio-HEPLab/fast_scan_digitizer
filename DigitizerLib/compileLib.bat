gcc -c fft.c -o fft.o
ar rcs libfft.a fft.o

gcc -c flash.c -o flash.o -I"C:\Program Files\CAEN\Digitizers\Library\include" -I"C:\Program Files\CAEN\Comm\include"
ar rcs libflash.a flash.o

gcc -c keyb.c -o keyb.o -I"C:\Program Files\CAEN\Digitizers\Library\include"
ar rcs libkeyb.a keyb.o

gcc -c spi.c -o spi.o -I"C:\Program Files\CAEN\Digitizers\Library\include" -I"C:\Program Files\CAEN\Comm\include"
ar rcs libspi.a spi.o

gcc -c WDconfig.c -o WDconfig.o -I"C:\Program Files\CAEN\Digitizers\Library\include"
ar rcs libWDconfig.a WDconfig.o

gcc -c X742CorrectionRoutines.c -o X742CorrectionRoutines.o -I"C:\Program Files\CAEN\Digitizers\Library\include"
ar rcs libX742CorrectionRoutines.a X742CorrectionRoutines.o

gcc -c -DBUILD_MY_DLL DigitizerLib.c -I"C:\Program Files\CAEN\Digitizers\Library\include"

gcc -shared -o DigitizerLib.dll *.o  -L"C:\Program Files\CAEN\Digitizers\Library\lib\x86_64" -lCAENDigitizer -L"C:\Program Files\CAEN\Comm\lib\x86_64" -lCAENComm

del *.a 
del *.o 
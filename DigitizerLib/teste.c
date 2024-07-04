#include <stdio.h>
#include "DigitizerLib.h"

int main()
{   int amp = 0;

    /*if(ConfigureDigitizer(argc, argv)){
        goto QuitProgram;
    }*/

    if(ConfigureDigitizer())
        goto QuitProgram;

    amp = getMinValue();
    if(amp<0){
        goto QuitProgram;
    }
    else{
        printf("\nO valor de amplitude encontrado foi de: %d\n", amp);
    }
    getchar();
QuitProgram:
    CloseDigitizer();

    return 0;
}
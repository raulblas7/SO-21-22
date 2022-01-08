#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>



//Anass Carreau Allagui 
//Raul Blas Ruiz
//Parte B practica 4
//Hemos hecho un programa que depende de los segundos del sistema
//Si hay menos de 20 segundos parpadea la primera luz
//Si hay entre 20 y  40 segundos parpadea la segunda luz y la primera se queda fija 
//Si hay más de 40 segundos parpadea la tercera luz y las otras dos se quedan fijas
//Si llega a 0 segundos parpadean todas las luces a la vez indicando que hemos alcanzado el minuto 

//Metodo que simula el parpadeo al llegar a un minuto 
void minuteWaiting(int leds)
{
    int i = 0;
    for (; i < 3; i++)
    {
        write(leds, "", 0);
        usleep(100000);

        char *buff = "123";
        write(leds, buff, strlen(buff));
        usleep(100000);
    }
    write(leds, "", 0);
}
//Metodo que simula el parpadero de la luz de la ultima posicion 
void waiting(char *buff, int leds)
{
    int s = strlen(buff) - 1;
    if (s != 0)
    {
        char buffAux[s];
        strncpy(buffAux, buff, s);
        write(leds, buffAux, s);
    }
    else
    {
        write(leds, "", 0);
    }

    usleep(100000);

    write(leds, buff, strlen(buff));
    usleep(100000);
}

//Convierte el numero de segundos en un patron de luces 
void secondsToLeds(int sec, int leds)
{

    if (sec == 0)
    {
        minuteWaiting(leds);
    }
    else if (sec < 20)
    {
        waiting("1", leds);
    }
    else if (sec < 40)
    {
        waiting("12", leds);
    }
    else
    {
        waiting("123", leds);
    }
}

int main()
{
    //Abrir archivo especial 
    int leds = open("/dev/leds", O_WRONLY | O_CREAT | O_TRUNC);
    if (leds == -1)
    {
        printf("No se ha encontrado el archivo");
        return -1;
    }

    time_t now;
    struct tm *tm;
    int lastSec = -1;
    //Encender todas las luces
    write(leds, "123", 3);
    while (1)
    {
        now = time(0);
        tm = localtime(&now);
        //Si cambia el segundo parpadean las luces
        if (lastSec != tm->tm_sec)
        {
            printf("%02d\n", tm->tm_sec);
            secondsToLeds(tm->tm_sec, leds);
            lastSec = tm->tm_sec;
        }
    }
    return 0;
}

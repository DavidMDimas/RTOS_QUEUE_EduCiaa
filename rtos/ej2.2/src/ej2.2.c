#include "board.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "ciaaIO.h"
#include "main.h"

SemaphoreHandle_t semaforo;

static void initHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, false);
    ciaaIOInit();
}

/* esta tarea va a destellar un LED
 * ahora el LED me lo pasan por parametro
 *
 * el parametro de una tarea es void *
 * este parametro se puede castear como un puntero
 * a otro tipo o bien cualquier entero de igual o
 * menor tamaño que el parametro (32 bits)
 *
 */
void tarea1(void * parametro)
{
	/* recordar tener especial cuidado con las
	 * referencias a punteros
	 * los punteros a void (void *) siempre deben
	 * ser casteados a otro tipo, en este caso int
	 */
	int led = (int)parametro;

	while (1) {
		/* espero el semáforo indefinidamente */
		xSemaphoreTake(semaforo, portMAX_DELAY);
		/* destello el LED */
		Board_LED_Set(led, 1);
		vTaskDelay(1000);
		Board_LED_Set(led, 0);
	}
}

/* esta tarea va a esperar un botón y va a liberar
 * el semáforo
 */
void tarea2(void * p)
{
	while (1) {
		/* espero que apachurre un botón */
		while (ciaaReadInput(1) == 1) {
			vTaskDelay(5);
		}
		/* espero que suelte el botón */
		while (ciaaReadInput(1) == 0) {
			vTaskDelay(5);
		}
		/* libero semáforo (pongo en verde) */
		xSemaphoreGive(semaforo);
	}
}

int main(void)
{
	/* inicialización de hardware */
	initHardware();

	/* creación de tareas */

	/* instancio la tarea 1 tres veces,
	 * pasando por parametro a cada instancia
	 * el número de led
	 *
	 * recordar castear el parametro a void *
	 */
	/* primer instancia, LED 0 */
	xTaskCreate(tarea1, "tarea1", 512, (void *)0, 1, 0);
	/* segunda instancia, LED 1 */
	xTaskCreate(tarea1, "tarea1", 512, (void *)1, 1, 0);
	/* segunda instancia, LED 2 */
	xTaskCreate(tarea1, "tarea1", 512, (void *)2, 1, 0);

	xTaskCreate(tarea2, "tarea2", 512, 0, 1, 0);

	/* creación del semáforo */
	/* NOTA: al crear el semáforo, éste se encuentra
	 * en VERDE (liberado)
	 */
	semaforo = xSemaphoreCreateBinary();

	/* iniciar el sistema operativo */
	vTaskStartScheduler();

	/* si llegamos a este punto es porque
	 * el sistema operativo no se pudo iniciar
	 * (problema)
	 */
	while (1) {

	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/

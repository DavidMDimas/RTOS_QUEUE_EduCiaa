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

/* esta tarea va a destellar un LED */
void tarea1(void * parametro)
{
	while (1) {
		/* espero el semáforo por 1 segundo
		 * si salgo de xSemaphoreTake porque se
		 * libera el semáforo, la función devuelve true
		 * si salgo porque pasa el tiempo de espera
		 * la función devuelve false
		 */
		if (xSemaphoreTake(semaforo, 1000) == pdTRUE) {
			/* el semáforo fue liberado desde la tarea
			 * del pulsador, prendo LED verde
			 */
			Board_LED_Set(1, 1);
			/* apago LED rojo */
			Board_LED_Set(0, 0);
		}
		else {
			/* en este caso se cumplió el tiempo de
			 * espera, prendo LED rojo
			 */
			Board_LED_Set(0, 1);
			/* apago LED verde */
			Board_LED_Set(1, 0);
		}
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
	xTaskCreate(tarea1, "tarea1", 512, 0, 1, 0);
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

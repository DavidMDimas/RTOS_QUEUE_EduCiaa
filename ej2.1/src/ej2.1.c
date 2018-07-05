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
		/* espero el semáforo indefinidamente */
		xSemaphoreTake(semaforo, portMAX_DELAY);
		/* destello el LED */
		Board_LED_Set(2, 1);
		vTaskDelay(1000);
		Board_LED_Set(2, 0);
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

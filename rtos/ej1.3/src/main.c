/*==================[inclusions]=============================================*/

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "ciaaIO.h"
#include "main.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, false);
    ciaaIOInit();
}

/*==================[external functions definition]==========================*/

/* esta tarea va a destellar un LED */
void tarea1(void * parametro)
{
	int tiempo_encendido = 100;

	portTickType ticks = xTaskGetTickCount();

	while (1) {
		Board_LED_Set(2, 1);
		vTaskDelay(tiempo_encendido);
		Board_LED_Set(2, 0);

		vTaskDelayUntil(&ticks, 1000);

		tiempo_encendido += 100;
		if (tiempo_encendido == 1000) {
			tiempo_encendido = 100;
		}
	}
}

int main(void)
{
	/* inicialización de hardware */
	initHardware();

	/* creación de tareas */
	xTaskCreate(tarea1, "tarea1", 512, 0, 1, 0);

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

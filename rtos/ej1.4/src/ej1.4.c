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

static int tiempo_pulsado;
static int tiempo_encendido = 500;

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

/* esta función va a ser llamada por el RTOS
 * cada 1 milisegundo
 */
void vApplicationTickHook(void)
{
	tiempo_pulsado++;
}


/* esta tarea va a destellar un LED */
void tarea1(void * parametro)
{
	portTickType ticks = xTaskGetTickCount();

	while (1) {
		Board_LED_Set(2, 1);
		vTaskDelay(tiempo_encendido);
		Board_LED_Set(2, 0);

		vTaskDelayUntil(&ticks, 1000);
	}
}

void tarea2(void * p)
{
	while (1) {
		if (ciaaReadInput(0) == 0) {
			tiempo_pulsado = 0;
			while (ciaaReadInput(0) == 0) {
				vTaskDelay(5);
			}
			tiempo_encendido = tiempo_pulsado;
		}
		else {
			vTaskDelay(5);
		}
	}
}

int main(void)
{
	/* inicialización de hardware */
	initHardware();

	/* creación de tareas */
	xTaskCreate(tarea1, "tarea1", 512, 0, 1, 0);
	xTaskCreate(tarea2, "tarea2", 512, 0, 1, 0);

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

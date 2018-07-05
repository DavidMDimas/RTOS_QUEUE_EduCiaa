#include "board.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "ciaaIO.h"
#include "main.h"

QueueHandle_t cola;
int contador;

static void initHardware(void)
{
    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, false);
    ciaaIOInit();
}

void vApplicationTickHook(void)
{
	contador++;
}

/* esta tarea va a destellar un LED
 * el tiempo de destello es recibido a traves
 * de una cola */
void tarea1(void * parametro)
{
	/* variable para guardar el tiempo recibido */
	int tiempo;

	while (1) {
		/* espero indefinidamente a que llegue un dato
		 * a traves de la cola
		 *
		 * la funcion xQueueReceive bloquea a la tarea
		 * mientras la cola esté vacía
		 *
		 * recordar pasar el dato a leer POR REFERENCIA
		 */
		xQueueReceive(cola, & tiempo, portMAX_DELAY);
		/* enciendo el LED durante el tiempo medido */
		Board_LED_Set(2, 1);
		vTaskDelay(tiempo);
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

		/* medimos tiempo: contador en cero */
		contador = 0;

		/* espero que suelte el botón */
		while (ciaaReadInput(1) == 0) {
			vTaskDelay(5);
		}

		/* enviamos tiempo medido por cola
		 * recordar pasar dato POR REFERENCIA
		 * en este caso timeout va en cero ya que
		 * no quiero bloquear esta tarea
		 */
		xQueueSend(cola, &contador, 0);
	}
}

int main(void)
{
	/* inicialización de hardware */
	initHardware();

	/* creación de tareas */
	xTaskCreate(tarea1, "tarea1", 512, 0, 1, 0);
	xTaskCreate(tarea2, "tarea2", 512, 0, 1, 0);

	/* creación de la cola:
	 * creamos una cola de 10 variables de tipo int
	 */
	cola = xQueueCreate(10, sizeof(int));

	/* ejemplo: si quisiera crear una cola de 20
	 * variables tipo float sería así:
	 *
	 * cola = xQueueCreate(20, sizeof(float));
	 */

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

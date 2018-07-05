#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "ciaaIO.h"
#include "main.h"
#include "semphr.h"
#include "queue.h"

/* tipo de estructura para parametros */
typedef struct {
	int contador;
	int boton;
	QueueHandle_t cola;
} tipoParametros;

/* declaro parametros para las tareas */
tipoParametros paramT1, paramT2;

static void initHardware(void)
{
	SystemCoreClockUpdate();

	Board_Init();

	Board_LED_Set(0, false);
	ciaaIOInit();
}

void vApplicationTickHook( void )
{
	/*
	 * en este caso accedemos al contador
	 * dentro de la estructura directamente
	 * ya que las estructuras son globales,
	 * por eso se usa el operador punto '.'
	 */
	paramT1.contador++;
	paramT2.contador++;
}

void tarea_led(void *parametro)
{
	/*variable para guardar el dato*/
	int tiempo=500;
	int tiempo2=500;

	while (1){
		xQueueReceive(paramT1.cola, &tiempo, 0);
		/*ENCIENDO EL LED DURANTE EL TIEMPO MEDIDO*/
		Board_LED_Set(2,1);
		vTaskDelay(tiempo);
		xQueueReceive(paramT2.cola, &tiempo2, 0);
		Board_LED_Set(2,0);
		vTaskDelay(tiempo2);
	}
}

void tarea_boton(void *parametro)
{
	tipoParametros * p = (tipoParametros *)parametro;

	while (1){
		/*espero que presione el boton*/
		/* usamos el operador flecha '->' ya que estamos
		 * accediendo a la estructura a traves de un puntero
		 */
		while (ciaaReadInput(p->boton)==1) {
			vTaskDelay(5);
		}

		p->contador =0;

		/*espero que suelte el boton*/
		while(ciaaReadInput(p->boton)==0) {
			vTaskDelay(5);
		}

		/*mando la cola*/
		xQueueSend(p->cola, &(p->contador),0);
	}
}

int main(void)
{

	initHardware();

	/*crear tareas*/

	xTaskCreate(tarea_led,"tarea_led",512,0,1,0);
	xTaskCreate(tarea_boton,"tarea_boton1",512,(void *)&paramT1,1,0);
	xTaskCreate(tarea_boton,"tarea_boton2",512,(void *)&paramT2,1,0);

	/*creo el la cola*/
	paramT1.cola = xQueueCreate(10,sizeof(int));
	paramT2.cola = xQueueCreate(10,sizeof(int));

	/* inicializo los botones */
	paramT1.boton = 0;
	paramT2.boton = 1;

	/*iniciar sistema operativo*/
	vTaskStartScheduler();

	while (1) {
	}
}

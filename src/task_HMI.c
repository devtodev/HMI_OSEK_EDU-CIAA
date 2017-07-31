/*============================================================================
 * Autor: Carlos Miguens
 * Fecha: Jul 30 2017
 *===========================================================================*/

#include "sapi.h"     // <= Biblioteca sAPI
#include "os.h"       // <= freeOSEK

#define HMI_OFF 			-1
#define HMI_MAIN_MENU 		0
#define HMI_SHOW_OP1 		1
#define HMI_SHOW_OP2 		2
#define HMI_STREAMING_DATA  9


int stateHMI;
uartMap_t MyUART = UART_USB;
int UART_SPEED = 115200;

int streamingData = 0;

void *(*dataStreaming)();

char *testString() { return "hola!\0"; }
int testInteger() { return 12345; }
char inputBuffer[10];

void hmi_streamData(void * *pFunc, int start, int cycle)
{
	dataStreaming = (void *) pFunc;
	stateHMI = HMI_STREAMING_DATA;
	SetRelAlarm(ActivateHumanMachineTask, start, cycle);
}


void showMenu(int option)
{
	stateHMI = option;
	ActivateTask(HumanMachineTask);
}

void task_HMI(void)
{
	int selectedOption;
	if (!streamingData) uartWriteString(MyUART, "\n\r\n\r\n\r\0");
	switch(stateHMI)
	{
		case HMI_OFF:
			WaitEvent(EventUART_RX);
			showMenu(HMI_MAIN_MENU);
		break;
		case HMI_MAIN_MENU:
			uartWriteString(MyUART, "Human Machine Interface example\n\r\0");
			uartWriteString(MyUART, "1) Option 1\n\r\0");
			uartWriteString(MyUART, "2) Option 2\n\r\0");
			uartWriteString(MyUART, "Choose your option: \n\r\0");
			WaitEvent(EventUART_RX);
			selectedOption = inputBuffer[0] - '0';
			if ((selectedOption == HMI_SHOW_OP1) ||
			    (selectedOption == HMI_SHOW_OP2))
			{
					showMenu(selectedOption);
			} else {
					showMenu(HMI_MAIN_MENU);
			}

			break;
		case HMI_SHOW_OP1:
			uartWriteString(MyUART, "Human Machine Interface example\n\r\0");
			uartWriteString(MyUART, "1) Show data1\n\r\0");
			uartWriteString(MyUART, "0) Main Menu\n\r\0");
			uartWriteString(MyUART, "Choose your option: \n\r\0");
			WaitEvent(EventUART_RX);
			if (inputBuffer[0] == '1')
			{
				int period = -1;
				while ((period < 0) || (period > 1000))
				{
					uartWriteString(MyUART, "Choose the refresh period (1-1000ms) or 0 to main menu: \0");
					WaitEvent(EventUART_RX);
					period = atoi(inputBuffer);
					uartWriteString(MyUART, "\n\r\0");
				}
				if (period == 0)
					showMenu(HMI_MAIN_MENU);
				else {
					hmi_streamData(testString, 0, 10);
				}

			}
			else
				showMenu(HMI_MAIN_MENU);

			break;
		case HMI_SHOW_OP2:
			break;
		case HMI_STREAMING_DATA:
			uartWriteString(MyUART, dataStreaming());
			uartWriteString(MyUART, "\n\r\0");
			break;

	}

	TerminateTask();
}


void task_HMI_init(void)
{
	uartConfig( MyUART, UART_SPEED );
	stateHMI = HMI_OFF;
}

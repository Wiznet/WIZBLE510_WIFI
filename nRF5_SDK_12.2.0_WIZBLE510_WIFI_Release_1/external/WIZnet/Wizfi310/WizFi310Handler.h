#ifndef WIZFI250HANDLER_H_
#define WIZFI250HANDLER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "app_error.h"
#include "app_uart.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "app_util.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "boards.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "softdevice_handler.h"
#include "ble_advdata.h"
#include "ble_nus_c.h"
#include "SEGGER_RTT.h"
#include "nrf_drv_uart.h"
#include "string.h"

#include "WizFi310_Config.h"

#define APP_TIMER_PRESCALER     0                               /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 8                               /**< Size of timer operation queues. */

#define UART_TX_BUF_SIZE        256                             /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        256                             /**< UART RX buffer size. */

typedef enum{
	RET_OK=0,
	RET_NOK,
  RET_FAIL
}state_ret_t;

typedef enum{
  WIZFI310_AT,
  WIZFI310_LEAVE,
  WIZFI310_NET,
  WIZFI310_SET,
  WIZFI310_SEC,
	WIZFI310_JOIN,
  WIZFI310_CON,
  WIZFI310_IDLE,
  WIZFI310_EXIT_DATAMODE
}wizfi310_state_t;

typedef struct{
	uint8_t socket;
  uint8_t remote_ip[4];
  uint32_t remote_port;
  uint32_t send_size;
  uint32_t timeout;
  uint8_t *find1;
  uint8_t *find2;
}wizfi310_t;

typedef struct{
  bool sending_flag;
	bool ok_flag;
  bool error_flag;
  bool timeout_flag;
  bool con_flag;
}wizfi310_flag_t;

//void wizfi310_fsm(wizfi310_state_t *wizfi310_state);
void word_compare(void);
state_ret_t send_and_check_command(char* send_command, char* str_find1, char* str_find2, uint32_t timeout);
void wizfi310_timers_init(void);
void wizfi310_timers_start(void);

#endif /* WIZFI250HANDLER_H_ */

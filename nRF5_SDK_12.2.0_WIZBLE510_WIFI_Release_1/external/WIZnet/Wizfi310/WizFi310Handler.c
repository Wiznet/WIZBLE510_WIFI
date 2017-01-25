
#include "WizFi310Handler.h"

#define TIMEOUT_INTERVAL      APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#define EXIT_TIMEOUT_INTERVAL      APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#define COMPARE_INTERVAL      APP_TIMER_TICKS(1, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */
#define FSM_INTERVAL      APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)  /**< Battery level measurement interval (ticks). */

APP_TIMER_DEF(m_timeout_timer_id);
APP_TIMER_DEF(m_compare_timer_id);
APP_TIMER_DEF(m_fsm_timer_id);

wizfi310_t wizfi310;
wizfi310_flag_t wizfi310_flag;
wizfi310_state_t Wizfi310_state;;
extern uint8_t data_array[];

static void set_send_flag (void)
{
  wizfi310_flag.ok_flag = false;
  wizfi310_flag.timeout_flag = false;
  wizfi310_flag.error_flag = false;
  wizfi310_flag.sending_flag = true;
}

static void set_ok_flag (void)
{
  wizfi310_flag.ok_flag = true;
  wizfi310_flag.timeout_flag = false;
  wizfi310_flag.error_flag = false;
  wizfi310_flag.sending_flag = false;
}

static void set_error_flag (void)
{
  wizfi310_flag.ok_flag = false;
  wizfi310_flag.timeout_flag = false;
  wizfi310_flag.error_flag = true;
  wizfi310_flag.sending_flag = false;
}

static void set_timeout_flag (void)
{
  wizfi310_flag.ok_flag = false;
  wizfi310_flag.timeout_flag = true;
  wizfi310_flag.error_flag = false;
  wizfi310_flag.sending_flag = false;
}

static void clear_flag (void)
{
  wizfi310_flag.ok_flag = false;
  wizfi310_flag.timeout_flag = false;
  wizfi310_flag.error_flag = false;
  wizfi310_flag.sending_flag = false;
  wizfi310_flag.con_flag = false;
}

void wizfi310_fsm_handler(void * p_context)
{
  char cmd[256];

  switch (Wizfi310_state) 
  {
    case WIZFI310_EXIT_DATAMODE: 
       if (wizfi310_flag.ok_flag || wizfi310_flag.timeout_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_AT;
        SEGGER_RTT_WriteString(0, "EXIT_DATAMODE [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        send_and_check_command("+++", "[OK]", NULL, 2000);
      }
      break; 
    
    case WIZFI310_AT: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_LEAVE;
        SEGGER_RTT_WriteString(0, "AT [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        send_and_check_command("AT\r\n", "[OK]", NULL, 3000);
      }
      break; 
    
    case WIZFI310_LEAVE: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_NET;
        SEGGER_RTT_WriteString(0, "AT+WLEAVE [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        send_and_check_command("AT+WLEAVE\r\n", "[OK]", NULL, 3000);
      }
      break; 
      
    case WIZFI310_NET: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_SET;
        SEGGER_RTT_WriteString(0, "AT+WNET [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        send_and_check_command("AT+WNET=1\r\n", "[OK]", NULL, 3000);
      }
      break; 
    
    case WIZFI310_SET: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_SEC;
        SEGGER_RTT_WriteString(0, "AT+WSET [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        sprintf(cmd,"AT+WSET=0,%s\r\n",SSID);      
        send_and_check_command(cmd, "[OK]", NULL, 3000);
      }
      break; 
      
    case WIZFI310_SEC: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_JOIN;
        SEGGER_RTT_WriteString(0, "AT+WSEC [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        sprintf(cmd,"AT+WSEC=0,,%s\r\n",PASSWORD);
        send_and_check_command(cmd, "[OK]", NULL, 3000);
      }
      break; 
      
    case WIZFI310_JOIN: 
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        Wizfi310_state = WIZFI310_CON;
        SEGGER_RTT_WriteString(0, "AT+WJOIN [OK]\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        send_and_check_command("AT+WJOIN\r\n", "[OK]", NULL, 10000);
      }
      break; 
      
    case WIZFI310_CON:
      if (wizfi310_flag.ok_flag)
      {
        clear_flag();
        wizfi310_flag.con_flag = true;
        Wizfi310_state = WIZFI310_IDLE;
        SEGGER_RTT_WriteString(0, "AT+SCON [OK]\r\n");
        printf("WIZFI310 Test\r\n");
      }
      else if (wizfi310_flag.sending_flag == false)
      {
        set_send_flag();
        sprintf(cmd,"AT+SCON=O,TCN,%s,%s,,1\r\n", SERVER_IP,SERVER_PORT);
        send_and_check_command(cmd, "[CONNECT", NULL, 10000);
      }
      break;
    case WIZFI310_IDLE:
      break;
  }
  if (wizfi310_flag.error_flag)
  {
    //SEGGER_RTT_WriteString(0, "ERROR or Timeout\r\n");
    clear_flag();;
  }
}

static void atcmd_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    set_timeout_flag();
}

static void word_compare_handler(void * p_context)
{
  uint8_t find1=0, find2=0;
  
  UNUSED_PARAMETER(p_context);

 
  if (wizfi310.find1 == NULL)
    find1 = 1;
  if (wizfi310.find2 == NULL)
    find2 = 1;
  
  if (find1 == 0)
  {
    if (strstr((char *)data_array, (char *)wizfi310.find1) != NULL)
      find1 = 1;
  }
  
  if (find2 == 0)
  {
    if (strstr((char *)data_array, (char *)wizfi310.find2) != NULL)
      find2 = 1;
  }
  
#if WIZFI310_RSP_PRINT
  SEGGER_RTT_WriteString(0, (char *)data_array);
#endif  
  
  if (strstr((char *)data_array, "ERROR") != NULL)
  {
    app_timer_stop(m_timeout_timer_id);
    //memset(data_array, 0x00, UART_RX_BUF_SIZE);
    //Fifo_Buf_Flush(&g_fifo_buf);
    set_error_flag();
  }

  if (find1 && find2)
  {
    app_timer_stop(m_timeout_timer_id);
    set_ok_flag();
  }
  memset(data_array, 0x00, UART_RX_BUF_SIZE);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
void wizfi310_timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.

    // Create timers.
    err_code = app_timer_create(&m_timeout_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                atcmd_timeout_handler);
    APP_ERROR_CHECK(err_code);
  
    err_code = app_timer_create(&m_compare_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                word_compare_handler);
    APP_ERROR_CHECK(err_code);
  
    err_code = app_timer_create(&m_fsm_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                wizfi310_fsm_handler);
    APP_ERROR_CHECK(err_code);
}

void wizfi310_timers_start(void)
{
  app_timer_start(m_fsm_timer_id, FSM_INTERVAL, NULL);
}


void word_compare(void)
{
  app_timer_start(m_compare_timer_id, COMPARE_INTERVAL, NULL);
}

state_ret_t send_and_check_command(char* send_command, char* str_find1, char* str_find2, uint32_t timeout)
{ 
  wizfi310.find1 = (uint8_t *)str_find1;
  wizfi310.find2 = (uint8_t *)str_find2;
  
  app_timer_start (m_timeout_timer_id, APP_TIMER_TICKS(timeout, APP_TIMER_PRESCALER), NULL);
  
  printf("%s", send_command);
  return RET_OK; 
}



void WizFi310_Restart(void)
{

 #if 0
					GPIO_SetBits(GPIOC, GPIO_Pin_1);		// HIGH
					delay_ms(10);
					GPIO_ResetBits(GPIOC, GPIO_Pin_1);		// LOW
					delay_ms(10);
					GPIO_SetBits(GPIOC, GPIO_Pin_1);		// HIGH

					delay_ms(1000);
#endif
}

void WizFi310_FactoryReset (void)
{
//					int i;
//					GPIO_SetBits(GPIOC, GPIO_Pin_10);	// HIGH
//					delay_ms(200);

//	for(i=0;i<3;i++)
//	{
//					GPIO_ResetBits(GPIOC, GPIO_Pin_10);// LOW
//					delay_ms(200);
//					GPIO_SetBits(GPIOC, GPIO_Pin_10);// HIGH
//					delay_ms(200);
//	}
}


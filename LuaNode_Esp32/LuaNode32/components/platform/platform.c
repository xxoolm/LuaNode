// Platform-dependent functions

#include "platform.h"
#include "c_stdio.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "rom/gpio.h"
#include "mygpio.h"
#include "user_config.h"
#include "rom/uart.h"
#include "rom/spi_flash.h"
#include "rom/ets_sys.h"
#include "soc/soc.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal-i2c.h"
#include "extras/soc_ext.h"
#include "platform_partition.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
// Platform specific includes

#include "rom.h"
#include "gpio16.h"
#include "i2c_master.h"
#include "spi_api.h"
#include "pin_map.h"
#include <stdio.h>
#include <stdlib.h>

i2c_t *i2c;

uint16_t flash_safe_get_sec_num(void);

#define FLASH_SEC_NUM		(flash_safe_get_sec_num())
#define SYS_PARAM_SEC_NUM	4
#define SYS_PARAM_SEC_START	(FLASH_SEC_NUM - SYS_PARAM_SEC_NUM)

#define DATA_LENGTH          512  /*!<Data buffer length for test buffer*/
#define I2C_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */
#define I2C_SLAVE_TX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN  (2*DATA_LENGTH) /*!<I2C slave rx buffer size */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
#define ACK_VAL    0x0         /*!< I2C ack value */
#define NACK_VAL   0x1         /*!< I2C nack value */

//static void pwms_init();

/*int platform_init()
{
  // Setup PWMs
  pwms_init();

  cmn_platform_init();
  // All done
  return PLATFORM_OK;
}*/

// ****************************************************************************
// KEY_LED functions
uint8_t platform_key_led( uint8_t level){
  uint8_t temp;
  gpio16_output_set(1);   // set to high first, for reading key low level
  gpio16_input_conf();
  temp = gpio16_input_get();
  gpio16_output_conf();
  gpio16_output_set(level);
  return temp;
}

// ****************************************************************************
// GPIO functions
#ifdef GPIO_INTERRUPT_ENABLE
extern void lua_gpio_unref(unsigned pin);
#endif


int platform_gpio_mode( unsigned pin, unsigned mode )
{
  // NODE_DBG("Function platform_gpio_mode() is called. pin_mux:%d, func:%d\n",pin_mux[pin],pin_func[pin]);
  pinMode(pin, mode);
  return 1;
}


int platform_gpio_write( unsigned pin, unsigned level )
{
  // NODE_DBG("Function platform_gpio_write() is called. pin:%d, level:%d\n",GPIO_ID_PIN(pin_num[pin]),level);
  /*if (pin >= NUM_GPIO)
    return -1;
  if(pin == 0){
    gpio16_output_conf();
    gpio16_output_set(level);
    return 1;
  }

  GPIO_OUTPUT_SET(GPIO_ID_PIN(pin_num[pin]), level);
  GPIO_OUTPUT(GPIO_ID_PIN(pin_num[pin]), level);*/
  digitalWrite(pin, level);
}

int platform_gpio_read( unsigned pin )
{
  // NODE_DBG("Function platform_gpio_read() is called. pin:%d\n",GPIO_ID_PIN(pin_num[pin]));
  /*if (pin >= NUM_GPIO)
    return -1;

  if(pin == 0){
    // gpio16_input_conf();
    return 0x1 & gpio16_input_get();
  }

  // GPIO_DIS_OUTPUT(pin_num[pin]);
  return 0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin_num[pin]));*/
  return digitalRead(pin);
}

#ifdef GPIO_INTERRUPT_ENABLE
static void platform_gpio_intr_dispatcher( platform_gpio_intr_handler_fn_t cb){
  uint8 i, level;
  uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
  for (i = 0; i < GPIO_PIN_NUM; i++) {
    if (pin_int_type[i] && (gpio_status & BIT(pin_num[i])) ) {
      //disable interrupt
      gpio_pin_intr_state_set(GPIO_ID_PIN(pin_num[i]), GPIO_PIN_INTR_DISABLE);
      //clear interrupt status
      GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(pin_num[i]));
      level = 0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin_num[i]));
      if(cb){
        cb(i, level);
      }
      gpio_pin_intr_state_set(GPIO_ID_PIN(pin_num[i]), pin_int_type[i]);
    }
  }
}

void platform_gpio_init( platform_gpio_intr_handler_fn_t cb )
{
  //ETS_GPIO_INTR_ATTACH(platform_gpio_intr_dispatcher, cb);
  //ETS_GPIO_INTR_ATTACH(cb, NULL);
}

int platform_gpio_intr_init( unsigned pin, GPIO_INT_TYPE type )
{
  if (pin >= NUM_GPIO)
    return -1;
  ETS_GPIO_INTR_DISABLE();
  //clear interrupt status
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(pin_num[pin]));
  pin_int_type[pin] = type;
  //enable interrupt
  gpio_pin_intr_state_set(GPIO_ID_PIN(pin_num[pin]), type);
  ETS_GPIO_INTR_ENABLE();
}
#endif

// ****************************************************************************
// UART
// TODO: Support timeouts.

// UartDev is defined and initialized in rom code.
//extern UartDevice UartDev;
uint32_t platform_uart_setup( unsigned id, uint32_t baud, int databits, int parity, int stopbits )
{
	UartDevice UartDev;
  switch( baud )
  {
    case BIT_RATE_9600:
    case BIT_RATE_19200:
    case BIT_RATE_38400:
    case BIT_RATE_57600:
    case BIT_RATE_115200:
    case BIT_RATE_230400:
    case BIT_RATE_460800:
    case BIT_RATE_921600:
      UartDev.baut_rate = baud;
      break;
    default:
      UartDev.baut_rate = BIT_RATE_9600;
      break;
  }

  switch( databits )
  {
    case 5:
      UartDev.data_bits = FIVE_BITS;
      break;
    case 6:
      UartDev.data_bits = SIX_BITS;
      break;
    case 7:
      UartDev.data_bits = SEVEN_BITS;
      break;
    case 8:
      UartDev.data_bits = EIGHT_BITS;
      break;
    default:
      UartDev.data_bits = EIGHT_BITS;
      break;
  }

  switch (stopbits)
  {
    case ONE_STOP_BIT:
      UartDev.stop_bits = ONE_STOP_BIT;
      break;
    case TWO_STOP_BIT:
      UartDev.stop_bits = TWO_STOP_BIT;
      break;
    default:
      UartDev.stop_bits = ONE_STOP_BIT;
      break;
  }

  switch (parity)
  {
    case EVEN_BITS:
      UartDev.parity = EVEN_BITS;
      break;
    case ODD_BITS:
      UartDev.parity = ODD_BITS;
      break;
    default:
      UartDev.parity = NONE_BITS;
      break;
  }

  //uart_config(id, &UartDev);

  return baud;
}

// if set=1, then alternate serial output pins are used. (15=rx, 13=tx)
void platform_uart_alt( int set )
{
    //uart0_alt( set );
    return;
}


// Send: version with and without mux
void platform_uart_send( unsigned id, u8 data ) 
{
  uart_tx_one_char(data);
  //printf("%c", data);
}

// ****************************************************************************
// PWMs

static uint16_t pwms_duty[NUM_PWM] = {0};

/*static void pwms_init()
{
  int i;
  for(i=0;i<NUM_PWM;i++){
    pwms_duty[i] = DUTY(0);
  }
  pwm_init(500, NULL);
  // NODE_DBG("Function pwms_init() is called.\n");
}*/

// Return the PWM clock
// NOTE: Can't find a function to query for the period set for the timer,
// therefore using the struct.
// This may require adjustment if driver libraries are updated.
uint32_t platform_pwm_get_clock( void )
{
  // NODE_DBG("Function platform_pwm_get_clock() is called.\n");
  uint32_t freq = ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
  return (uint32_t)freq;
}

// Set the PWM clock
uint32_t platform_pwm_set_clock( unsigned pin, uint32_t clock )
{
  // NODE_DBG("Function platform_pwm_set_clock() is called.\n");
  ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, clock);
  return (uint32_t)clock;
}

uint32_t platform_pwm_get_duty( unsigned channel )
{
  // NODE_DBG("Function platform_pwm_get_duty() is called.\n");
  int duty = ledc_get_duty(LEDC_HIGH_SPEED_MODE, channel);
  if(duty) {
	return duty;
  }
  return 0;
}

// Set the PWM duty
uint32_t platform_pwm_set_duty( unsigned channel, uint32_t duty )
{
  // NODE_DBG("Function platform_pwm_set_duty() is called.\n");
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);	//the new duty is not valid, until ledc_update_duty is called
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
  return duty;
}

uint32_t platform_pwm_setup( unsigned pin, uint32_t frequency, unsigned duty, unsigned channel )
{
  ledc_timer_config_t ledc_timer = {
        //set timer counter bit number
        .bit_num = LEDC_TIMER_13_BIT,
        //set frequency of pwm
        .freq_hz = frequency,
        //timer mode,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        //timer index
        .timer_num = LEDC_TIMER_0
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {
        //set LEDC channel 0
        .channel = channel,
        //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
        .duty = duty,
        //GPIO number
        .gpio_num = pin,
        //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
        .intr_type = LEDC_INTR_FADE_END,
        //set LEDC mode, from ledc_mode_t
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        //set LEDC timer source, if different channel use one timer,
        //the frequency and bit_num of these channels should be the same
        .timer_sel = LEDC_TIMER_0
  };
  //set the configuration
  ledc_channel_config(&ledc_channel);

  return frequency;
}

void platform_pwm_close( unsigned pin )
{
  // NODE_DBG("Function platform_pwm_stop() is called.\n");
  // deprecated
}

void platform_pwm_start(void)
{
  // NODE_DBG("Function platform_pwm_start() is called.\n");
  //deprecated
}

void platform_pwm_stop( unsigned channel )
{
  // NODE_DBG("Function platform_pwm_stop() is called.\n");
  int output_idle_level = 0;
  ledc_stop(LEDC_HIGH_SPEED_MODE, channel, output_idle_level);
}

// *****************************************************************************
// I2C platform interface
void platform_i2c_setup( uint8_t mode, uint8_t port, uint8_t scl, uint8_t sda, uint8_t addr ){
  i2c_config_t conf;
  conf.mode = mode;
  conf.sda_io_num = sda;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = scl;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  if (mode == I2C_MODE_MASTER) {
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
  } else {
    conf.slave.addr_10bit_en = 0;
	conf.slave.slave_addr = addr;
  }
  
  i2c_param_config(port, &conf);

  if (mode == I2C_MODE_MASTER) {
    i2c_driver_install(port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
  } else {
    i2c_driver_install(port, conf.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
  }
  
}

void platform_i2c_send_start( unsigned id ){
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
}

void platform_i2c_send_stop( unsigned id ){
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_stop(cmd);
}

#if 0
int platform_i2c_send_address( unsigned id, uint16_t address, int direction ){
  // Convert enum codes to R/w bit value.
  // If TX == 0 and RX == 1, this test will be removed by the compiler
  if ( ! ( PLATFORM_I2C_DIRECTION_TRANSMITTER == 0 &&
           PLATFORM_I2C_DIRECTION_RECEIVER == 1 ) ) {
    direction = ( direction == PLATFORM_I2C_DIRECTION_TRANSMITTER ) ? 0 : 1;
  }

  i2c_master_writeByte( (uint8_t) ((address << 1) | direction ));
  // Low-level returns nack (0=acked); we return ack (1=acked).
  return ! i2c_master_getAck();
}
#endif

int platform_i2c_send_byte( unsigned mode, unsigned port, unsigned addr, uint8_t *data, uint32_t len ){
  if (mode == I2C_MODE_SLAVE) {
	size_t d_size = i2c_slave_write_buffer(port, data, len, 1000 / portTICK_RATE_MS);
	return d_size;
  } else {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( addr << 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, data, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd); 
	return ret;
  }
  return 999;
}


int platform_i2c_recv_byte( uint8_t mode, uint8_t port, uint8_t addr, uint8_t * data, uint32_t len ){
  if (mode == I2C_MODE_SLAVE) {
    int size = i2c_slave_read_buffer( port, data, len, 1000 / portTICK_RATE_MS);
	return size;
  } else {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( addr << 1 ) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data + len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	return ret;
  }
  return len;
}


// *****************************************************************************
// SPI platform interface
uint32_t platform_spi_setup( uint8_t id, int mode, unsigned cpol, unsigned cpha, uint32_t clock_div)
{
  spi_master_init( id, cpol, cpha, clock_div );
  return 1;
}

int platform_spi_send( uint8_t id, uint8_t bitlen, spi_data_type data )
{
  if (bitlen > 32)
    return PLATFORM_ERR;

  spi_mast_transaction( id, 0, 0, bitlen, data, 0, 0, 0 );
  return PLATFORM_OK;
}

spi_data_type platform_spi_send_recv( uint8_t id, uint8_t bitlen, spi_data_type data )
{
  if (bitlen > 32)
    return 0;

  spi_mast_set_mosi( id, 0, bitlen, data );
  spi_mast_transaction( id, 0, 0, 0, 0, bitlen, 0, -1 );
  return spi_mast_get_miso( id, 0, bitlen );
}

int platform_spi_set_mosi( uint8_t id, uint8_t offset, uint8_t bitlen, spi_data_type data )
{
  if (offset + bitlen > 512)
    return PLATFORM_ERR;

  spi_mast_set_mosi( id, offset, bitlen, data );

  return PLATFORM_OK;
}

spi_data_type platform_spi_get_miso( uint8_t id, uint8_t offset, uint8_t bitlen )
{
  if (offset + bitlen > 512)
    return 0;

  return spi_mast_get_miso( id, offset, bitlen );
}

int platform_spi_transaction( uint8_t id, uint8_t cmd_bitlen, spi_data_type cmd_data,
                              uint8_t addr_bitlen, spi_data_type addr_data,
                              uint16_t mosi_bitlen, uint8_t dummy_bitlen, int16_t miso_bitlen )
{
  if ((cmd_bitlen   >  16) ||
      (addr_bitlen  >  32) ||
      (mosi_bitlen  > 512) ||
      (dummy_bitlen > 256) ||
      (miso_bitlen  > 512))
    return PLATFORM_ERR;

  spi_mast_transaction( id, cmd_bitlen, cmd_data, addr_bitlen, addr_data, mosi_bitlen, dummy_bitlen, miso_bitlen );

  return PLATFORM_OK;
}

// ****************************************************************************
// Flash access functions
uint32_t platform_get_flash_size(uint32_t phy_start_addr)
{
  return ((SYS_PARAM_SEC_START * INTERNAL_FLASH_SECTOR_SIZE) - phy_start_addr);
}

uint32_t platform_flash_mapped2phys (uint32_t mapped_addr)
{
  uint32_t cache_ctrl = READ_PERI_REG(CACHE_FLASH_CTRL_REG);
  if (!(cache_ctrl & CACHE_FLASH_ACTIVE))
    return -1;
  bool b0 = (cache_ctrl & CACHE_FLASH_MAPPED0) ? 1 : 0;
  bool b1 = (cache_ctrl & CACHE_FLASH_MAPPED1) ? 1 : 0;
  uint32_t meg = (b1 << 1) | b0;
  return mapped_addr - INTERNAL_FLASH_MAPPED_ADDRESS + meg * 0x100000;
}

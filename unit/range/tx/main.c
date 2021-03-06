#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../../src/spi.c"
#include "../../../src/nrf24l01p.c"
#include "../../../src/uart.c"

static void make_pattern(uint8_t* buf)
{
  uint8_t i;
  uint8_t x;

  for (i = 0, x = 0x2a; i < NRF24L01P_PAYLOAD_WIDTH; ++i, ++x)
    buf[i] = x;
}

int main(void)
{
  uint8_t buf[NRF24L01P_PAYLOAD_WIDTH];
  uint32_t counter = 0;
#if 0
  uint8_t x;
#endif

  /* setup spi first */
  spi_setup_master();
  spi_set_sck_freq(SPI_SCK_FREQ_FOSC2);

  uart_setup();

  nrf24l01p_setup();

  /* sparkfun usb serial board configuration */
  /* NOTE: nrf24l01p_enable_crc8(); for nrf24l01p board */
  /* nrf24l01p_enable_crc16(); */
  nrf24l01p_disable_crc();
  /* auto ack disabled */
  /* auto retransmit disabled */
  /* 4 bytes payload */
  /* 1mbps, 0dbm */
  /* nrf24l01p_set_rate(NRF24L01P_RATE_1MBPS); */
  nrf24l01p_set_rate(NRF24L01P_RATE_2MBPS);
  /* nrf24l01p_set_rate(NRF24L01P_RATE_250KBPS); */
  /* channel 2 */
  nrf24l01p_set_chan(2);
  /* 5 bytes addr width */
  /* nrf24l01p_set_addr_width(NRF24L01P_ADDR_WIDTH_5); */
  nrf24l01p_set_addr_width(NRF24L01P_ADDR_WIDTH_3);
  /* rx address */
  nrf24l01p_cmd_buf[0] = 0xe7;
  nrf24l01p_cmd_buf[1] = 0xe7;
  nrf24l01p_cmd_buf[2] = 0xe7;
  nrf24l01p_cmd_buf[3] = 0xe7;
  nrf24l01p_cmd_buf[4] = 0xe7;
  nrf24l01p_write_reg40(NRF24L01P_REG_RX_ADDR_P0);
  /* tx address */
  nrf24l01p_cmd_buf[0] = 0xe7;
  nrf24l01p_cmd_buf[1] = 0xe7;
  nrf24l01p_cmd_buf[2] = 0xe7;
  nrf24l01p_cmd_buf[3] = 0xe7;
  nrf24l01p_cmd_buf[4] = 0xe7;
  nrf24l01p_write_reg40(NRF24L01P_REG_TX_ADDR);
  /* enable tx no ack command */
  nrf24l01p_enable_tx_noack();

  nrf24l01p_powerdown_to_standby();
  nrf24l01p_standby_to_tx();
  if (nrf24l01p_is_tx_empty() == 0) nrf24l01p_flush_tx();

#if 0
  uart_write((uint8_t*)"tx side\r\n", 9);
  uart_write((uint8_t*)"press space\r\n", 13);
  uart_read_uint8(&x);
  uart_write((uint8_t*)"starting\r\n", 10);
#endif

  /* enable interrupts */
  sei();

  while (1)
  {
    if ((++counter) != (200000UL / 8UL)) continue ;
    counter = 0;

    make_pattern(buf);
    nrf24l01p_write_tx_noack_zero(buf);
    nrf24l01p_complete_tx_noack_zero();
    while (nrf24l01p_is_tx_irq() == 0) ;
  }

  return 0;
}

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 *
 * @author      Attilio Dona'
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "nrf_soc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER I2C_0_PIN_SCL
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER  I2C_0_PIN_SDA

#define TWI_READ_BIT                 (0x80)        //!< If this bit is set in the address field, transfer direction is from slave to master.

#define TWI_ISSUE_STOP               ((bool)true)  //!< Parameter for @ref twi_master_transfer
#define TWI_DONT_ISSUE_STOP          ((bool)false) //!< Parameter for @ref twi_master_transfer

/* These macros are needed to see if the slave is stuck and we as master send dummy clock cycles to end its wait */
/*lint -e717 -save "Suppress do {} while (0) for these macros" */
/*lint ++flb "Enter library region" */
#define TWI_SCL_HIGH()   do { NRF_GPIO->OUTSET = (1UL << TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER); } while(0)   /*!< Pulls SCL line high */
#define TWI_SCL_LOW()    do { NRF_GPIO->OUTCLR = (1UL << TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER); } while(0)   /*!< Pulls SCL line low  */
#define TWI_SDA_HIGH()   do { NRF_GPIO->OUTSET = (1UL << TWI_MASTER_CONFIG_DATA_PIN_NUMBER);  } while(0)   /*!< Pulls SDA line high */
#define TWI_SDA_LOW()    do { NRF_GPIO->OUTCLR = (1UL << TWI_MASTER_CONFIG_DATA_PIN_NUMBER);  } while(0)   /*!< Pulls SDA line low  */
#define TWI_SDA_INPUT()  do { NRF_GPIO->DIRCLR = (1UL << TWI_MASTER_CONFIG_DATA_PIN_NUMBER);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI_SDA_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << TWI_MASTER_CONFIG_DATA_PIN_NUMBER);  } while(0)   /*!< Configures SDA pin as output */
#define TWI_SCL_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER); } while(0)   /*!< Configures SCL pin as output */
/*lint -restore */

#define TWI_SDA_READ() ((NRF_GPIO->IN >> TWI_MASTER_CONFIG_DATA_PIN_NUMBER) & 0x1UL)                     /*!< Reads current state of SDA */
#define TWI_SCL_READ() ((NRF_GPIO->IN >> TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER) & 0x1UL)                    /*!< Reads current state of SCL */

#define TWI_DELAY() nrf_delay_us(4) /*!< Time to wait when pin states are changed. For fast-mode the delay can be zero and for standard-mode 4 us delay is sufficient. */

/* Max cycles approximately to wait on RXDREADY and TXDREADY event,
 * This is optimized way instead of using timers, this is not power aware. */
#define MAX_TIMEOUT_LOOPS (20000UL) /**< MAX while loops to wait for RXD/TXD event */

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] =
{
#if I2C_0_EN
		[I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
		[I2C_1] = MUTEX_INIT,
#endif
#if I2C_2_EN
		[I2C_2] = MUTEX_INIT
#endif
#if I2C_3_EN
		[I2C_3] = MUTEX_INIT
#endif
	};

static void __INLINE nrf_delay_us(uint32_t volatile number_of_us) {
	do {
		__ASM volatile (
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
				"NOP\n\t"
		);
	} while(--number_of_us);
}

/**
 * @brief Function for detecting stuck slaves (SDA = 0 and SCL = 1) and tries to clear the bus.
 *
 * @return
 * @retval false Bus is stuck.
 * @retval true Bus is clear.
 */
static bool twi_master_clear_bus(void) {
	uint32_t twi_state;
	bool bus_clear;
	uint32_t clk_pin_config;
	uint32_t data_pin_config;

	// Save and disable TWI hardware so software can take control over the pins.
	twi_state = NRF_TWI1->ENABLE;
	NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

	clk_pin_config =
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER];
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);

	data_pin_config =
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_DATA_PIN_NUMBER];
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_DATA_PIN_NUMBER] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);

	TWI_SDA_HIGH()
	;
	TWI_SCL_HIGH()
	;
	TWI_DELAY();

	if((TWI_SDA_READ() == 1) && (TWI_SCL_READ() == 1)) {
		bus_clear = true;
	}
	else {
		uint_fast8_t i;
		bus_clear = false;

		// Clock max 18 pulses worst case scenario(9 for master to send the rest of command and 9
		// for slave to respond) to SCL line and wait for SDA come high.
		for(i = 18; i--;) {
			TWI_SCL_LOW()
			;
			TWI_DELAY();
			TWI_SCL_HIGH()
			;
			TWI_DELAY();

			if(TWI_SDA_READ() == 1) {
				bus_clear = true;
				break;
			}
		}
	}

	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER] = clk_pin_config;
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_DATA_PIN_NUMBER] = data_pin_config;

	NRF_TWI1->ENABLE = twi_state;

	return bus_clear;
}

/** @brief Function for initializing the twi_master.
 */
int i2c_init_master(i2c_t dev, i2c_speed_t speed) {

	/* To secure correct signal levels on the pins used by the TWI
	 master when the system is in OFF mode, and when the TWI master is
	 disabled, these pins must be configured in the GPIO peripheral.
	 */
	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);

	NRF_GPIO->PIN_CNF[TWI_MASTER_CONFIG_DATA_PIN_NUMBER] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);

	NRF_TWI1->EVENTS_RXDREADY = 0;
	NRF_TWI1->EVENTS_TXDSENT = 0;
	NRF_TWI1->PSELSCL = TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER;
	NRF_TWI1->PSELSDA = TWI_MASTER_CONFIG_DATA_PIN_NUMBER;
	NRF_TWI1->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;

#ifdef SOFTDEVICE_PRESENT
	sd_ppi_channel_assign(0, &NRF_TWI1->EVENTS_BB,
			&NRF_TWI1->TASKS_SUSPEND);
	sd_ppi_channel_enable_clr(PPI_CHENCLR_CH0_Msk);
#else
	NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TWI1->EVENTS_BB;
	NRF_PPI->CH[0].TEP = (uint32_t)&NRF_TWI1->TASKS_SUSPEND;
	NRF_PPI->CHENCLR = PPI_CHENCLR_CH0_Msk;
#endif

	NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;

	if(twi_master_clear_bus()) {
		return 0;
	}
	return -1;
}

int i2c_acquire(i2c_t dev) {
	if(dev >= I2C_NUMOF) {
		return -1;
	}
	mutex_lock(&locks[dev]);
	return 0;
}

int i2c_release(i2c_t dev) {
	if(dev >= I2C_NUMOF) {
		return -1;
	}
	mutex_unlock(&locks[dev]);
	return 0;
}

/** @brief Function for read by twi_master.
 */
static bool twi_master_read(uint8_t * data, uint8_t data_length,
bool issue_stop_condition) {
	uint32_t timeout = MAX_TIMEOUT_LOOPS; /* max loops to wait for RXDREADY event*/

	if(data_length == 0) {
		/* Return false for requesting data of size 0 */
		return false;
	}
	else if(data_length == 1) {
#ifdef SOFTDEVICE_PRESENT
		sd_ppi_channel_assign(0, &NRF_TWI1->EVENTS_BB, &NRF_TWI1->TASKS_STOP);
#else
		NRF_PPI->CH[0].TEP = (uint32_t) &NRF_TWI1->TASKS_STOP;
#endif
	}
	else {
#ifdef SOFTDEVICE_PRESENT
		sd_ppi_channel_assign(0, &NRF_TWI1->EVENTS_BB,
				&NRF_TWI1->TASKS_SUSPEND);
#else
		NRF_PPI->CH[0].TEP = (uint32_t) &NRF_TWI1->TASKS_SUSPEND;
#endif
	}
#ifdef SOFTDEVICE_PRESENT
	sd_ppi_channel_enable_set(PPI_CHENSET_CH0_Msk);
#else
	NRF_PPI->CHENSET = PPI_CHENSET_CH0_Msk;
#endif

	NRF_TWI1->EVENTS_RXDREADY = 0;
	NRF_TWI1->TASKS_STARTRX = 1;

	/** @snippet [TWI HW master read] */
	while(true) {
		while(NRF_TWI1->EVENTS_RXDREADY == 0 && NRF_TWI1->EVENTS_ERROR == 0 && (--timeout)) {
			// Do nothing.
		}
		NRF_TWI1->EVENTS_RXDREADY = 0;

		if(timeout == 0 || NRF_TWI1->EVENTS_ERROR != 0) {
			// Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
			// Product Anomaly Notification document found at
			// https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
			NRF_TWI1->EVENTS_ERROR = 0;
			NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
			NRF_TWI1->POWER = 0;
			nrf_delay_us(5);
			NRF_TWI1->POWER = 1;
			NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;

			(void) i2c_init_master(0, 0);

			return false;
		}

		*data++ = NRF_TWI1->RXD;

		/* Configure PPI to stop TWI master before we get last BB event */
		if(--data_length == 1) {
#ifdef SOFTDEVICE_PRESENT
			sd_ppi_channel_assign(0, &NRF_TWI1->EVENTS_BB,
					&NRF_TWI1->TASKS_STOP);
#else

			NRF_PPI->CH[0].TEP = (uint32_t) &NRF_TWI1->TASKS_STOP;
#endif
		}

		if(data_length == 0) {
			break;
		}

		// Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
		// Product Anomaly Notification document found at
		// https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
		nrf_delay_us(20);
		NRF_TWI1->TASKS_RESUME = 1;
	}
	/** @snippet [TWI HW master read] */

	/* Wait until stop sequence is sent */
	while(NRF_TWI1->EVENTS_STOPPED == 0) {
		// Do nothing.
	}
	NRF_TWI1->EVENTS_STOPPED = 0;

#ifdef SOFTDEVICE_PRESENT
	sd_ppi_channel_enable_clr(PPI_CHENCLR_CH0_Msk);
#else
	NRF_PPI->CHENCLR = PPI_CHENCLR_CH0_Msk;
#endif
	return true;
}

static bool twi_master_write(uint8_t * data, uint8_t data_length,
bool issue_stop_condition) {
	uint32_t timeout = MAX_TIMEOUT_LOOPS; /* max loops to wait for EVENTS_TXDSENT event*/

	if(data_length == 0) {
		/* Return false for requesting data of size 0 */
		return false;
	}

	NRF_TWI1->TXD = *data++;
	NRF_TWI1->TASKS_STARTTX = 1;

	/** @snippet [TWI HW master write] */
	while(true) {
		while(NRF_TWI1->EVENTS_TXDSENT == 0 && NRF_TWI1->EVENTS_ERROR == 0 && (--timeout)) {
			// Do nothing.
		}

		if(timeout == 0 || NRF_TWI1->EVENTS_ERROR != 0) {
			// Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
			// Product Anomaly Notification document found at
			// https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
			NRF_TWI1->EVENTS_ERROR = 0;
			NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
			NRF_TWI1->POWER = 0;
			nrf_delay_us(5);
			NRF_TWI1->POWER = 1;
			NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;

			(void) i2c_init_master(0, 0);

			return false;
		}
		NRF_TWI1->EVENTS_TXDSENT = 0;
		if(--data_length == 0) {
			break;
		}

		NRF_TWI1->TXD = *data++;
	}
	/** @snippet [TWI HW master write] */

	if(issue_stop_condition) {
		NRF_TWI1->EVENTS_STOPPED = 0;
		NRF_TWI1->TASKS_STOP = 1;
		/* Wait until stop sequence is sent */
		while(NRF_TWI1->EVENTS_STOPPED == 0) {
			// Do nothing.
		}
	}
	return true;
}

/** @brief  Function for transfer by twi_master.
 */
bool twi_master_transfer(uint8_t address, uint8_t * data, uint8_t data_length,
bool issue_stop_condition) {
	bool transfer_succeeded = false;
	if(data_length > 0 && twi_master_clear_bus()) {
		NRF_TWI1->ADDRESS = address;

		if((address & TWI_READ_BIT)) {
			transfer_succeeded = twi_master_read(data, data_length,
					issue_stop_condition);
		}
		else {
			transfer_succeeded = twi_master_write(data, data_length,
					issue_stop_condition);
		}
	}
	return transfer_succeeded;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data) {
	return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length) {
	bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(address | TWI_READ_BIT, data,
			length, TWI_ISSUE_STOP);
	if(transfer_succeeded) {
		return length;
	}
	return -1;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data) {
	return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data,
		int length) {
	bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(address, &reg, 1,
			TWI_DONT_ISSUE_STOP);
	transfer_succeeded &= twi_master_transfer(address | TWI_READ_BIT, data,
			length, TWI_ISSUE_STOP);
	if(transfer_succeeded) {
		return length;
	}
	return -1;
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data) {
	return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length) {
	bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(address, (void *) data, length,
	TWI_ISSUE_STOP);
	if(transfer_succeeded) {
		return length;
	}
	return -1;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data) {
	return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data,
		int length) {
	bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(address, &reg, 1,
	TWI_DONT_ISSUE_STOP);
	transfer_succeeded &= twi_master_transfer(address, (void *) data, length,
	TWI_ISSUE_STOP);
	if(transfer_succeeded) {
		return length;
	}
	return -1;
}

void i2c_poweron(i2c_t dev) {
	switch(dev) {
#if I2C_0_EN
		case I2C_0:

			break;
#endif
#if I2C_1_EN
			case I2C_1:

			break;
#endif
	}
}

void i2c_poweroff(i2c_t dev) {
}

#if I2C_0_EN
void I2C_0_ERR_ISR(void) {
}
#endif

#if I2C_1_EN
void I2C_1_ERR_ISR(void)
{
}
#endif

//#endif /* I2C_0_EN || I2C_1_EN */

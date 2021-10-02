/******************************************************************************
 *
 * 2017 (c) Lattice Semiconductor Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 * 
 * This program is distributed "AS IS" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * You may contact Lattice Semiconductor Corporation at
 * 111 SW 5th Ave, Suite 700, Portland, OR 97204.
 *
 *****************************************************************************/
/**
* @file sii9437_api.h
*
* @brief Driver API for SiI9437 Rx devices.
*
*******************************************************************************/

#ifndef SII9437_API_H
#define SII9437_API_H

#include "sii_datatypes.h"
#include "sii_lib_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************** API Return Codes *******************************/

/**
 * Return value for successful API execution
 */
#define SII_RETURN_VALUE__SUCCESS		0

/**
 * Error value returned when the device ID used to create instance
 * using Create API is not valid or duplicate.
 */
#define SII_RETURN_VALUE__DEV_ID_ERR		1

/**
 * Error value returned when the input argument to the API is invalid
 */
#define SII_RETURN_VALUE__INVALID_ARG_ERR	2

/**
 * Error value returned when the driver got asserted for an error and no more
 * in operational mode. Use sii9437_assert_query API to find out the assertion
 * details.
 */
#define SII_RETURN_VALUE_ASSERTION_ERR		255

/**
 * Return value for failure in API execution
 */
#define SII_RETURN_VALUE__FAIL			(-1)

/******************************************************************************/

typedef void (*sii9437_event_callback_func)(uint32_t *callback_param);

/*** Events to be notified ***/

/** Event when logging FIFO buffer becomes non-empty */
#define SII9437_EVENT_FLAGS__LOG_FIFO_CHNG	((uint32_t)1 << 0)

/** Event when audio extraction mode is changed among SPDIF and I2S */
#define SII9437_EVENT_FLAGS__EXTRACT_MODE_CHNG	((uint32_t)1 << 1)

/** Event when incoming channel status is changed during
 * a stable audio input signal */
#define SII9437_EVENT_FLAGS__CHAN_STAT_CHNG	((uint32_t)1 << 3)

/** Event when incoming HPD level is changed */
#define SII9437_EVENT_FLAGS__HPD_CHNG		((uint32_t)1 << 5)

/** Event when any one of the GPIOs, which is configured as input and enabled
 * for event notifications through sii9437_gpio_event_mask_set API, has
 * input level change, either from low to high or from high to low*/
#define SII9437_EVENT_FLAGS__GPIO_CHNG		((uint32_t)1 << 6)

/** Event to notify an assertion failure in the driver. This event should
 * be handled by calling sii9437_assert_query API to get the details of
 * assertion failure. After this event, there will be no guarantee for the
 * driver to be functionally correct until it is restarted.*/
#define SII9437_EVENT_FLAGS__ASSERT		((uint32_t)1 << 7)

/** Event to notify the request from eARC TX for eARC RX to adjust
 * the audio latency */
#define SII9437_EVENT_FLAGS__ERX_LATENCY_REQ_CHNG	((uint32_t)1 << 8)

/** Event when eARC link is established or lost */
#define SII9437_EVENT_FLAGS__EARC_LINK_CHNG		((uint32_t)1 << 9)

/** Event when eARC discovery failure happened due to timeout */
#define SII9437_EVENT_FLAGS__EARC_DISC_TIMEOUT	((uint32_t)1 << 10)

enum sii9437_extraction_mode {
	SII9437_EXTRACTION_MODE__NONE,
	SII9437_EXTRACTION_MODE__SPDIF2,
	SII9437_EXTRACTION_MODE__SPDIF8,
	SII9437_EXTRACTION_MODE__I2S2,
	SII9437_EXTRACTION_MODE__I2S8
};

enum sii9437_pref_extraction_mode {
	SII9437_PREF_EXTRACTION_MODE__SPDIF,
	SII9437_PREF_EXTRACTION_MODE__I2S
};

enum sii9437_mclk_mode {
	SII9437_MCLK_MODE__NONE,
	SII9437_MCLK_MODE__FS128,
	SII9437_MCLK_MODE__FS256,
	SII9437_MCLK_MODE__FS512
};

struct sii9437_config {
	/* Hardware device instantiation identification number.
	* When the system includes more than one instance of
	* SiI9437, The user must make sure that each
	* instantiation is created with a unique dev_id.
	* dev_id is passed into the platform host interface
	* layer and is used to select a different I2C device
	* address or a different SPI chip select signal.
	*/
	uint32_t dev_id;

	/* Pointer to a call back function invoked by the
	* driver when any status change event happens.
	* If set as NULL, the callback is not invoked.
	*/
	sii9437_event_callback_func callback_func;

	/* Logging FIFO Queue size in bytes */
	uint32_t log_fifo_size;

	/* I2C address to access eARC registers */
	uint8_t i2c_addr_1;

	/* I2C address to access PHY registers */
	uint8_t i2c_addr_2;

	/* Current audio latency in milliseconds from the eARC RX audio
	 * input to the speakers*/
	struct sii_erx_latency erx_latency;
};

/******************************************************************************/
/**
* @brief Adapter driver creation
*
* Allocates resources for Adapter Driver and all child modules.
* This function has to be called before using of any other module function.
* This function also registers a callback function provided in the
* the config structure that'll be called when any status change event occurs.
*
* @param[in]	p_config	Pointer to a static configuration structure
*				containing static configuration parameters for
*				this driver instantiation.
* @param[out]	p_inst		Pointer to driver instance handle required
*				by all other functions.Zero will be returned
*				when creation of instance has failed
*
* @retval	See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_create(const struct sii9437_config *p_config,
			sii_inst_t *p_inst);

/******************************************************************************/
/**
* @brief Adapter driver deletion
*
* Frees resources allocated by sii9437_create for the driver instance
* identified by inst and shuts down the driver.
* After this function is called, no other module functions can be called for
* this driver instance until sii9437_create() is called again and a new driver
* handle is received.
*
* @param[in]	inst	Driver instance handle obtained from calling
*			sii9437_create. This is used by sii9437_delete to
*			identify the driver instance to delete.

* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_delete(sii_inst_t inst);

/******************************************************************************/
/**
* @brief Adapter Driver handle
*
* This function is expected to be called by the user application within 50msec
* after SiI9437 chip generates an interrupt (INT='1'). If the interrupt signal
* (INT) from the SiI9437 is not connected to application processor, this
* function must be called periodically with a time interval of
* no more than 50msec.
* This function checks if there are any pending notifications from the SiI9437
* chip, services them and may generate an SII9437_EVENT 'notifications' by
* calling the sii9437_event_callback_func callback function registered through
* sii9437_create().
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_handle(sii_inst_t inst);

/******************************************************************************/
/**
* @brief Query chip ID
*
* p_chip_id_stat should contain 0x9437 after calling.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[out]	p_chip_id_stat	Pointer to a variable in which the chip ID
*				information is returned.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_chip_id_query(sii_inst_t inst, uint32_t *p_chip_id_stat);

/******************************************************************************/
/**
* @brief Query chip version
*
* Outputs chip version number
*
* @param[in]	inst		Driver instance handle obtained by calling
*				sii9437_create.
* @param[out]	p_chip_version	Pointer to a variable in which the chip
*				version information is returned.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_chip_version_query(sii_inst_t inst, uint8_t *p_chip_version);

/******************************************************************************/
/**
* @brief Query software version number
*
* Outputs software version number.
*
* @param[out]	version_str	Pointer to a string in which the software
*				version information is returned.
* @param[in]	size		Size of the version string to be returned
*
* @retval			The number of characters that were missed out
*				from the original string generated internally,
*				including missing string-end character ('0').
*
*				If the return value is zero then the version
*				string size was less than the 'size'.
*
*				If the return value is non-zero then
*				the returned string does not contain a
*				string-end character ('0').
*
*				The maximum number of returned characters by
*				design (including string-end character) is 65
*
*******************************************************************************/
uint32_t sii9437_software_version_query(char *version_str, uint8_t size);

/******************************************************************************/
/**
* @brief Query log queue
*
* Outputs logging string stored in internal FIFO. Size of logging FIFO
* can be allocated upon create().
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[out]	log_string_str	Pointer the return string.
* @param[in]	size	Size of the string to be returned
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_log_queue_query(sii_inst_t inst,
	char *log_string_str, uint32_t size);

/******************************************************************************/
/**
* @brief Log Queue Size
*
* Returns unsigned number of characters stored in logging queue.
* FIFO overflow is indicated by all bits set to '1'.
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[out]	p_size	Pointer the log size value returned.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_log_queue_size(sii_inst_t inst, uint32_t *p_size);

/******************************************************************************/
/**
* @brief Assert Query
*
* Returns 32 bit integer defined as follow:
* Bit  0-23 : line number in source code
* Bit 24-31 : Module ID#.
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[out]	p_value	Pointer to the integer value returned.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_assert_query(sii_inst_t inst, uint32_t *p_value);

/******************************************************************************/
/**
* @brief Set/Get event flags mask
*
* Controls which events will cause notification handler to be called.
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[in]	p_flags_mask	Pointer to the flags mask
*
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_event_flags_mask_set(sii_inst_t inst,
		uint32_t *p_flags_mask);
uint32_t sii9437_event_flags_mask_get(sii_inst_t inst,
		uint32_t *p_flags_mask);

/******************************************************************************/
/**
* @brief Event Flags clear request
*
* Clear internal event register bits.
* This function is only needed for unmasked events.
* Masked events are automatically cleared upon event call-back of
* sii9437_event_callback_func.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_flags_mask	Pointer to a variable containing the
*				notification mask to clear.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_event_flags_status_clear(sii_inst_t inst,
	uint32_t *p_flags_mask);

/******************************************************************************/
/**
* @brief Query event flags
*
* Gets a bitmask of current events.
* Typically, only un-masked events are returned,
* since masked events get cleared upon event call-back of
* sii9437_event_callback_func.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_flags_status	Pointer to a variable in which the notification
*				mask information is returned.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_event_flags_query(sii_inst_t inst,
	uint32_t *p_flags_status);

/******************************************************************************/
/**
* @brief Set/Get standby mode
*
* Controls whether SiI9437 is in standby mode or full operational mode.
* In standby mode SiI9437 is configured for low power consumption.
* Note : User application can still change configuration while in standby mode.
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[in]	p_on	TRUE  : Put SiI9437 device in standby mode.
*			FALSE : Put SiI9437 in full operational mode.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_standby_set(sii_inst_t inst, bool_t *p_on);
uint32_t sii9437_standby_get(sii_inst_t inst, bool_t *p_on);

/******************************************************************************/
/**
* @brief Set/Get GPIO configuration
*
* Input / Output configuration.
* '1' enables GPIO to be an output,
* '0' enables GPIO to be an input.
* By default all GPIOs are configured as input.
*
* @param[in] inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in] p_gpio_pin_mask
*			Each bit represents 1 GPIO pin based on the
*			following assignment:
*			Bit 0   : GPIO0
*			Bit 1   : GPIO1
*			Bit 2   : GPIO2
*			Bit 3   : GPIO3
*			Bit 4   : GPIO4
*			Bit 5   : GPIO5
*			Bit 6,7 : Unused
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_gpio_config_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);
uint32_t sii9437_gpio_config_get(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);

/******************************************************************************/
/**
* @brief Set/Get level on GPIO pins configured as output
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_gpio_pin_mask
*			Each bit represents 1 GPIO pin based on the
*			following assignment:
*			Bit 0   : GPIO0
*			Bit 1   : GPIO1
*			Bit 2   : GPIO2
*			Bit 3   : GPIO3
*			Bit 4   : GPIO4
*			Bit 5   : GPIO5
*			Bit 6,7 : Unused
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_gpio_output_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);
uint32_t sii9437_gpio_output_clr(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);

/******************************************************************************/
/**
* @brief Set/Get mask related to GPIO event control
*
* Any bit that is set causes an event SII9437_EVENT_FLAGS__GPIO_CHNG
* to be generated as soon if associated GPIO is configured as an input
* and input level was changed either from low to high or from high to low.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_gpio_pin_mask
*			Bit field in which each bit represents 1 GPIO
*			pin based on the following assignment:
*			Bit 0   : GPIO0
*			Bit 1   : GPIO1
*			Bit 2   : GPIO2
*			Bit 3   : GPIO3
*			Bit 4   : GPIO4
*			Bit 5   : GPIO5
*			Bit 6,7 : Unused
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_gpio_event_mask_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);
uint32_t sii9437_gpio_event_mask_get(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask);

/******************************************************************************/
/**
* @brief Retrieve current levels on all input GPIO pins
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[out]	p_gpio_status
*			Each bit represents 1 GPIO pin based on the
*			following assignment:
*			Bit 0   : GPIO0
*			Bit 1   : GPIO1
*			Bit 2   : GPIO2
*			Bit 3   : GPIO3
*			Bit 4   : GPIO4
*			Bit 5   : GPIO5
*			Bit 6,7 : Unused
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_gpio_query(sii_inst_t inst, uint16_t *p_gpio_status);

/******************************************************************************/
/**
* @brief Set/Get eARC Capabilities Data Structure configuration
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
*
* @param[in]	offset	Capabilities Data Structure offset pointer.
*			Must be less than 128.
* @param[out]	p_data	Pointer to local array of bytes.
* @param[in]	length	Length of requested caps data transaction.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_earc_caps_ds_set(sii_inst_t inst, uint16_t offset,
	uint8_t *pData, uint16_t length);
uint32_t sii9437_earc_caps_ds_get(sii_inst_t inst, uint16_t offset,
	uint8_t *pData, uint16_t length);

/******************************************************************************/
/**
* @brief Set/Get eARC_HPD when in eARC mode
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[out]	p_earc_hpd	Pointer to eARC HPD.
*
* @retval		See general SII_RETURN_VALUE definitions.
*
*******************************************************************************/
uint32_t sii9437_earc_hpd_set(sii_inst_t inst, bool_t *p_earc_hpd);
uint32_t sii9437_earc_hpd_get(sii_inst_t inst, bool_t *p_earc_hpd);

/******************************************************************************/
/**
* @brief Set/Get eARC_HPD low time when toggling
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create.
* @param[out]	p_earc_hpd_toggle_time
*			Pointer to eARC HPD low time in milliseconds.
*			It should be minimum of 100ms, invalid argument error
*			will be returned otherwise.
*
* @retval		See general SII_RETURN_VALUE definitions.
*
*******************************************************************************/
uint32_t sii9437_earc_hpd_toggle_time_set(sii_inst_t inst,
		uint32_t *p_earc_hpd_toggle_time);
uint32_t sii9437_earc_hpd_toggle_time_get(sii_inst_t inst,
		uint32_t *p_earc_hpd_toggle_time);

/******************************************************************************/
/**
* @brief Set/Get ARC mode
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[out]	p_arc_mode	Pointer to the ARC Mode
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_arc_mode_set(sii_inst_t inst,
	enum sii_arc_mode *p_arc_mode);
uint32_t sii9437_arc_mode_get(sii_inst_t inst,
	enum sii_arc_mode *p_arc_mode);

/******************************************************************************/
/**
* @brief Set/Get preferred extraction mode
*
* Configures/Returns preferred audio extraction mode (I2S, SPDIF)
* in case audio is received as PCM or compressed audio formats.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_mode	Pointer to preferred extraction mode value
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_preferred_extraction_mode_set(sii_inst_t inst,
	enum sii9437_pref_extraction_mode *p_mode);
uint32_t sii9437_preferred_extraction_mode_get(sii_inst_t inst,
	enum sii9437_pref_extraction_mode *p_mode);


/******************************************************************************/
/**
* @brief Query extraction mode
*
* Returns extraction mode (NONE, SPDIF2, SPDIF8, I2S2, I2S8)
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[out]	p_mode	Pointer to the extraction mode value
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_extraction_mode_query(sii_inst_t inst,
	enum sii9437_extraction_mode *p_mode);

/******************************************************************************/
/**
* @brief Set/Get I2S MCLK mode
*
* Configures/Returns mode of MCLK output signal.(NONE, FS128, FS256, FS512)
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create
* @param[in]	p_mode	Pointer to the value of i2s MCLK mode
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_i2s_mclk_mode_set(sii_inst_t inst,
	enum sii9437_mclk_mode *p_mode);
uint32_t sii9437_i2s_mclk_mode_get(sii_inst_t inst,
	enum sii9437_mclk_mode *p_mode);

/******************************************************************************/
/**
* @brief Query channel status data
*
* Returns channel status data (only applicable to SPDIF2, SPDIF8, I2S2, I2S8
* extraction modes).
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create
* @param[out]	p_channel_status	Pointer to the channel status
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_channel_status_query(sii_inst_t inst,
	struct sii_channel_status *p_channel_status);

/******************************************************************************/
/**
* @brief Set/Get mute enable for audio data
*
* Mutes(zeroes)/Unmutes audio data.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create.
* @param[in]	p_flag	Pointer to the mute enable flag.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_mute_enable_set(sii_inst_t inst, bool_t *p_flag);
uint32_t sii9437_mute_enable_get(sii_inst_t inst, bool_t *p_flag);

/******************************************************************************/
/**
* @brief Query HDMI HPD status
*
* Returns level of HDMI HPD signal.
*
* @param[in]	inst	Driver instance handle obtained by calling
			sii9437_create
* @param[out]	p_hpd	TRUE : Current HDMI HPD level is high
*			FALSE : Current HDMI HPD level is low
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_hpd_query(sii_inst_t inst, bool_t *p_hpd);

/******************************************************************************/
/**
* @brief Query the channel count/speaker alloc/lfe playback level from
* channel status.
*
* Returns channel count/speaker alloc/lfe playback level info.
*
* @param[in]	inst		Driver instance handle obtained by calling
				sii9437_create
* @param[out]	p_audio_info	pointer to a structure of channel count/
*				speaker alloc/lfepbl in the channel status
*
* @retval			See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_audio_info_query(sii_inst_t inst,
		struct sii_audio_info *p_audio_info);

/******************************************************************************/
/**
* @brief Set/Get eARC erx latency.
*
* Sets/Gets the current audio latency in milliseconds from the eARC RX audio
* input to the speakers
*
* @param[in]	inst		Driver instance handle obtained by calling
*				sii9437_create.
* @param[in]	p_erx_latency	Pointer to the erx latency data.
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_erx_latency_set(sii_inst_t inst,
				struct sii_erx_latency *p_erx_latency);
uint32_t sii9437_erx_latency_get(sii_inst_t inst,
				struct sii_erx_latency *p_erx_latency);

/******************************************************************************/
/**
* @brief Query the eARC erx latency value requested by eARC TX
*
* Queries the eARC erx latency value requested by eARC TX for eARC RX
*  to minimise the audio latency.
*
* @param[in]	inst		Driver instance handle obtained by calling
*				sii9437_create.
* @param[in]	p_erx_latency_get	Pointer to the erx latency data request
*				value.
*				The default value after entereing the [RX eARC]
*				state is 0
*				= 0 to 250: The requested audio latency in
*				  millisecondsfrom eARC inhput to the speakers
*				= 251 to 253: Reserved
*				= 254: eARC audio is not synced with video
*				  (no synchronisation required)
*				= 255: eARC TX does not have latency information
*				  (unknown latency)
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_erx_latency_req_query(sii_inst_t inst,
				struct sii_erx_latency_req *p_erx_latency_req);

/******************************************************************************/
/**
* @brief Query eARC link status
*
* Returns TRUE if eARC link is active.
*
* @param[in]	inst	Driver instance handle obtained by calling
*			sii9437_create
* @param[out]	p_on	Pointer to eARC link status
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_earc_link_query(sii_inst_t inst, bool_t *p_on);

/******************************************************************************/
/**
* @brief Query eARC link status
*
* Returns sample rate of the audio in Hz.
*
* @param[in]	inst		Driver instance handle obtained by calling
*				sii9437_create
* @param[out]	p_sample_rate	Pointer to sample rate
*
* @retval		See general SII_RETURN_VALUE definitions
*
*******************************************************************************/
uint32_t sii9437_sample_rate_query(sii_inst_t inst, uint32_t *p_sample_rate);

#ifdef __cplusplus
}
#endif
#endif /* SII9437_API_H */

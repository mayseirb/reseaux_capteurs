/*
 * Copyright (c) 2021, Koncepto.io
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_HPMA115_H_
#define CATIE_SIXTRON_HPMA115_H_

#include "mbed.h"
#include <cstdint>

namespace sixtron {

/*!
 * \brief Results of data measurements
 */
typedef struct {
    /** PM2.5 in µg/m³. */
    uint16_t pm2_5;
    /** PM10 in µg/m³. */
    uint16_t pm10;
    /** PM1.0 in µg/m³. */
    uint16_t pm1_0;
    /** PM4.0 in µg/m³. */
    uint16_t pm4_0;
    /** Validity of PM1.0 and PM4.0 values. */
    bool pm1_pm4_valid;
} hpma115_data_t;

/*!
 *  \class HPMA115
 *  HPMA115 particle sensors driver
 */
class HPMA115 {
public:
    /*!
     * \brief Commands error codes
     */
    enum class ErrorType : uint8_t {
        /** Command successful */
        Ok = 0,
        /** Generic error with the serial port or data received */
        SerialError,
        /** Timeout on receiving data */
        SerialTimeout,
        /** NACK received from sensor */
        SensorNack,
        /** CRC mismatch in received frame */
        CrcError,
    };

    enum class Header : uint8_t {
        Send = 0x68,
        Resp = 0x40,
        Ack = 0xA5,
        Nack = 0x96,
        AutoData1 = 0x42,
        AutoData2 = 0x4d,
    };

    enum class Command : uint8_t {
        ReadMeas = 0x04,
        StartMeas = 0x01,
        StopMeas = 0x02,
        SetCoef = 0x08,
        ReadCoef = 0x10,
        EnableAutoSend = 0x40,
        StopAutoSend = 0x20,
    };

    /*! Constructor
     *
     * \param tx Serial port TX pin
     * \param rx Serial port RX pin
     */
    HPMA115(PinName tx, PinName rx);

    /*!
     * \brief Read sensor measurements
     *
     * \param result pointer to structure to store read data
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType read_measurement(hpma115_data_t *result);

    /*!
     * \brief Start measurement
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType start_measurement();

    /*!
     * \brief Stop measurement
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType stop_measurement();

    /*!
     * \brief Stop auto send
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType stop_autosend();

    /*!
     * \brief Enable auto send
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType enable_autosend();

    /*!
     * \brief Set customer adjustment coefficient
     *
     * \param coef value of the coefficient
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType set_adjust_coef(uint8_t coef);

    /*!
     * \brief Read customer adjustment coefficient
     *
     * \param coef pointer to location to store data read
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType read_adjust_coef(uint8_t *coef);

private:
    mbed::BufferedSerial _serial;

    /*!
     * \brief Send a command
     *
     * \param cmd the type of command
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType send_command(Command cmd);

    /*!
     * \brief Send data with a given command
     *
     * \param cmd the type of command
     * \param data the value to send
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType send_data(Command cmd, uint8_t data);

    /*!
     * \brief Receive data from a given command
     *
     * \param cmd the type of command
     * \param len pointer to store length read
     * \param data pointer to array to store data location
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType read_data(Command cmd, uint8_t *len, uint8_t **data);

    /*!
     * \brief Read data with a timeout
     *
     * \param data location where to store data
     * \param len number of bytes to read
     *
     * return Ok on success, the reason of failure otherwise
     */
    ErrorType read_timeout(uint8_t *data, ssize_t len);

    /*!
     * \brief Compute checksum on a given frame
     *
     * \param frame frame to compute the CRC on
     *
     * return Ok on success, the reason of failure otherwise
     */
    uint8_t compute_checksum(uint8_t *frame);
};

} // namespace sixtron

#endif // CATIE_SIXTRON_HPMA115_H_

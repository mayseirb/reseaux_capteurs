/*
 * Copyright (c) 2021, Koncepto.io
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hpma115/hpma115.h"
#include "PinNames.h"
#include "mbed.h"

#define POLLIN_TIMEOUT_MS (50)
#define BUFFER_SIZE (16)

static void flush_rx(BufferedSerial *serial)
{
    char dummy;

    while (serial->readable())
        serial->read(&dummy, 1);
}

static uint8_t buf[BUFFER_SIZE];
static SingletonPtr<PlatformMutex> _serial_mutex;

namespace sixtron {

HPMA115::HPMA115(PinName tx, PinName rx): _serial(tx, rx, 9600)
{
}

HPMA115::ErrorType HPMA115::read_measurement(hpma115_data_t *result)
{
    ErrorType err;
    uint8_t len;
    uint8_t *data;

    _serial_mutex->lock();
    err = this->read_data(Command::ReadMeas, &len, &data);

    if (err != ErrorType::Ok) {
        /* Do nothing. */
    } else if (data[0] != static_cast<uint8_t>(Command::ReadMeas)) {
        err = ErrorType::SerialError;
    } else if (len == 13) { /* It's a compact sensor. */
        result->pm1_pm4_valid = true;
        result->pm1_0 = ((uint16_t)(data[1]) << 8) + data[2];
        result->pm2_5 = ((uint16_t)(data[3]) << 8) + data[4];
        result->pm4_0 = ((uint16_t)(data[5]) << 8) + data[6];
        result->pm10 = ((uint16_t)(data[7]) << 8) + data[8];
    } else if (len == 5) { /* It's a standard sensor. */
        result->pm1_pm4_valid = false;
        result->pm1_0 = 0;
        result->pm4_0 = 0;
        result->pm2_5 = ((uint16_t)(data[1]) << 8) + data[2];
        result->pm10 = ((uint16_t)(data[3]) << 8) + data[4];
    } else {
        err = ErrorType::SerialError;
    }
    _serial_mutex->unlock();
    return err;
}

HPMA115::ErrorType HPMA115::start_measurement()
{
    ErrorType err;

    _serial_mutex->lock();
    err = this->send_command(Command::StartMeas);
    _serial_mutex->unlock();

    return err;
}

HPMA115::ErrorType HPMA115::stop_measurement()
{
    ErrorType err;

    _serial_mutex->lock();
    err = this->send_command(Command::StopMeas);
    _serial_mutex->unlock();

    return err;
}

HPMA115::ErrorType HPMA115::stop_autosend()
{
    ErrorType err;

    _serial_mutex->lock();
    err = this->send_command(Command::StopAutoSend);
    _serial_mutex->unlock();

    return err;
}

HPMA115::ErrorType HPMA115::enable_autosend()
{
    ErrorType err;

    _serial_mutex->lock();
    err = this->send_command(Command::EnableAutoSend);
    _serial_mutex->unlock();

    return err;
}

HPMA115::ErrorType HPMA115::set_adjust_coef(uint8_t coef)
{
    ErrorType err;

    _serial_mutex->lock();
    err = this->send_data(Command::SetCoef, coef);
    _serial_mutex->unlock();

    return err;
}

HPMA115::ErrorType HPMA115::read_adjust_coef(uint8_t *coef)
{
    ErrorType err;
    uint8_t len;
    uint8_t *data;

    _serial_mutex->lock();
    err = this->read_data(Command::ReadCoef, &len, &data);

    if (err != ErrorType::Ok) {
        /* Do nothing.*/
    } else if (len != 2 || data[0] != static_cast<uint8_t>(Command::ReadCoef)) {
        err = ErrorType::SerialError;
    } else {
        *coef = data[1];
    }
    _serial_mutex->unlock();
    return err;
}

HPMA115::ErrorType HPMA115::send_command(Command cmd)
{
    ErrorType err;

    flush_rx(&this->_serial);

    buf[0] = static_cast<char>(Header::Send);
    buf[1] = 1;
    buf[2] = static_cast<char>(cmd);

    /* Invalid checksum and compute it */
    buf[3] = 0;
    buf[3] = compute_checksum(buf);

    if (_serial.write(buf, 4) != 4) {
        err = ErrorType::SerialError;
        goto send_cmd_end;
    }

    err = read_timeout(buf, 2);
    if (err != ErrorType::Ok) {
        goto send_cmd_end;
    }

    if (buf[0] == buf[1] && buf[0] == static_cast<char>(Header::Ack)) {
        err = ErrorType::Ok;
    } else if (buf[0] == buf[1] && buf[0] == static_cast<char>(Header::Nack)) {
        err = ErrorType::SensorNack;
    } else {
        /* Unexpected data. */
        err = ErrorType::SerialError;
    }

send_cmd_end:
    return err;
}

HPMA115::ErrorType HPMA115::send_data(Command cmd, uint8_t data)
{
    ErrorType err;

    flush_rx(&this->_serial);

    buf[0] = static_cast<char>(Header::Send);
    buf[1] = 2;
    buf[2] = static_cast<char>(cmd);
    buf[3] = data;

    /* Invalid checksum and compute it */
    buf[4] = 0;
    buf[4] = compute_checksum(buf);

    if (_serial.write(buf, 5) != 5) {
        err = ErrorType::SerialError;
        goto send_data_end;
    }

    err = read_timeout(buf, 2);
    if (err != ErrorType::Ok) {
        goto send_data_end;
    }

    if (buf[0] == buf[1] && buf[0] == static_cast<char>(Header::Ack)) {
        err = ErrorType::Ok;
    } else if (buf[0] == buf[1] && buf[0] == static_cast<char>(Header::Nack)) {
        err = ErrorType::SensorNack;
    } else {
        /* Unexpected data. */
        err = ErrorType::SerialError;
    }

send_data_end:
    return err;
}

HPMA115::ErrorType HPMA115::read_data(Command cmd, uint8_t *len, uint8_t **data)
{
    ErrorType err;

    flush_rx(&this->_serial);

    buf[0] = static_cast<char>(Header::Send);
    buf[1] = 1;
    buf[2] = static_cast<char>(cmd);

    /* Invalid checksum and compute it */
    buf[3] = 0;
    buf[3] = compute_checksum(buf);

    if (_serial.write(buf, 4) != 4) {
        err = ErrorType::SerialError;
        goto read_data_end;
    }

    err = read_timeout(buf, 2);
    if (err != ErrorType::Ok) {
        goto read_data_end;
    }

    if (buf[0] == static_cast<char>(Header::Resp)) {
        if ((uint8_t)buf[1] > (BUFFER_SIZE - 3)) {
            err = ErrorType::SerialError;
            goto read_data_end;
        }

        err = read_timeout(buf + 2, buf[1] + 1);
        if (err != ErrorType::Ok) {
            goto read_data_end;
        }

        if (compute_checksum(buf) != 0) {
            err = ErrorType::CrcError;
            goto read_data_end;
        }

        *len = static_cast<uint8_t>(buf[1]);
        *data = buf + 2;

    } else if (buf[0] == buf[1] && buf[0] == static_cast<char>(Header::Nack)) {
        err = ErrorType::SensorNack;
    } else {
        err = ErrorType::SerialError;
    }

read_data_end:
    if (err != ErrorType::Ok) {
        *len = 0;
        *data = NULL;
    }
    return err;
}

HPMA115::ErrorType HPMA115::read_timeout(uint8_t *data, ssize_t len)
{
    ErrorType err = ErrorType::Ok;
    struct pollfh sfh = {
        .fh = &this->_serial,
        .events = POLLIN,
    };

    for (int i = 0; i < len; i++) {
        if (!poll(&sfh, 1, POLLIN_TIMEOUT_MS)) {
            err = ErrorType::SerialTimeout;
            break;
        }
        if (!this->_serial.read(data + i, 1)) {
            err = ErrorType::SerialError;
            break;
        }
    }
    return err;
}

uint8_t HPMA115::compute_checksum(uint8_t *frame)
{
    uint8_t csum = -frame[0];
    csum -= frame[1];

    for (int i = 0; i <= frame[1]; i++) {
        csum -= frame[2 + i];
    }

    return csum;
}

} // namespace sixtron

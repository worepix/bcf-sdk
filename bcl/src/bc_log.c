#include <bc_log.h>
#include <bc_uart.h>
#include <bc_error.h>

typedef struct
{
    bool initialized;
    bc_log_level_t level;
    bc_log_timestamp_t timestamp;
    bc_tick_t tick_last;

    char buffer[256];

} bc_log_t;

#ifndef RELEASE

static bc_log_t _bc_log = { .initialized = false };

void application_error(bc_error_t code);

static void _bc_log_message(bc_log_level_t level, char id, const char *format, va_list ap);

void bc_log_init(bc_log_level_t level, bc_log_timestamp_t timestamp)
{
    if (_bc_log.initialized)
    {
        return;
    }

    memset(&_bc_log, 0, sizeof(_bc_log));

    _bc_log.level = level;
    _bc_log.timestamp = timestamp;

    bc_uart_init(BC_UART_UART2, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);
    bc_uart_write(BC_UART_UART2, "\r\n", 2);

    _bc_log.initialized = true;
}

void bc_log_debug(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    _bc_log_message(BC_LOG_LEVEL_DEBUG, 'D', format, ap);
    va_end(ap);
}

void bc_log_info(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    _bc_log_message(BC_LOG_LEVEL_INFO, 'I', format, ap);
    va_end(ap);
}

void bc_log_warning(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    _bc_log_message(BC_LOG_LEVEL_WARNING, 'W', format, ap);
    va_end(ap);
}

void bc_log_error(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    _bc_log_message(BC_LOG_LEVEL_ERROR, 'E', format, ap);
    va_end(ap);
}

void bc_log_dump(void *buffer, size_t length, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    _bc_log_message(BC_LOG_LEVEL_DEBUG, 'D', format, ap);
    va_end(ap);

    strcpy(_bc_log.buffer, "# <DUMP> ");

    size_t offset = 9;

    for (size_t i = 0; i < length; i++)
    {
        offset += sprintf(_bc_log.buffer + offset, "%02X, ", ((uint8_t *) buffer)[i]);

        if (i % 16 == 15)
        {
            _bc_log.buffer[offset - 2] = '\r';
            _bc_log.buffer[offset - 1] = '\n';

            bc_uart_write(BC_UART_UART2, _bc_log.buffer, offset);

            offset = 9;
        }
    }

    _bc_log.buffer[offset - 2] = '\r';
    _bc_log.buffer[offset - 1] = '\n';

    bc_uart_write(BC_UART_UART2, _bc_log.buffer, offset);
}

static void _bc_log_message(bc_log_level_t level, char id, const char *format, va_list ap)
{
    if (!_bc_log.initialized)
    {
        application_error(BC_ERROR_LOG_NOT_INITIALIZED);
    }

    if (_bc_log.level == BC_LOG_LEVEL_OFF || _bc_log.level > level)
    {
        return;
    }

    size_t offset;

    if (_bc_log.timestamp == BC_LOG_TIMESTAMP_ABS)
    {
        bc_tick_t tick_now = bc_tick_get();

        uint32_t timestamp_abs = tick_now / 10;

        offset = sprintf(_bc_log.buffer, "# %lu.%02lu <%c> ", timestamp_abs / 100, timestamp_abs % 100, id);
    }
    else if (_bc_log.timestamp == BC_LOG_TIMESTAMP_REL)
    {
        bc_tick_t tick_now = bc_tick_get();

        uint32_t timestamp_rel = (tick_now - _bc_log.tick_last) / 10;

        offset = sprintf(_bc_log.buffer, "# +%lu.%02lu <%c> ", timestamp_rel / 100, timestamp_rel % 100, id);

        _bc_log.tick_last = tick_now;
    }
    else
    {
        strcpy(_bc_log.buffer, "# <!> ");

        _bc_log.buffer[3] = id;

        offset = 6;
    }

    offset += vsnprintf(&_bc_log.buffer[offset], sizeof(_bc_log.buffer) - offset - 3, format, ap);

    _bc_log.buffer[offset++] = '\r';
    _bc_log.buffer[offset++] = '\n';

    bc_uart_write(BC_UART_UART2, _bc_log.buffer, offset);
}

#endif

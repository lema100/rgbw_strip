#ifndef MAIN_H
#define MAIN_H

#include <QWidget>
#include <QDebug>
#include <QMap>
#include <QDateTime>
#include <QThread>

#define PARAM_COUNT				50
#define MAX_ARCH 				256
#define MAX_ARCH_PAR			256

struct port_parametrs
{
	QString com_Port;
	int baud_Rate;
	int stop_bit;
	int address;
};

struct device_parametrs
{
	QVector<uint32_t> list;
};

struct device_data
{
	QString version;
	int baudrate;
	int address;
};

enum
{
	CMD_SET_COLOR1 = 1,
	CMD_SET_COLOR2,
	CMD_RESET,
	CMD_WRITE_BAUDRATE,
	CMD_WRITE_ADDRESS,
	CMD_WRITE_PARAM,
	CMD_BURN_BIN,
	CMD_CHECK_BIN,
};

enum
{
	MB_PWM_W = 0,
	MB_PWM_B,
	MB_PWM_G,
	MB_PWM_R,
	MB_PWM2_W,
	MB_PWM2_B,
	MB_PWM2_G,
	MB_PWM2_R,
	MB_SETT0 = 50,
	MB_SETT_MB_BAUD,
	MB_SETT_MB_SLAVE,
	MB_SETT3,
	MB_SETT4,
	MB_SETT5,
	MB_SETT6,
	MB_SETT7,
	MB_SETT8,
	MB_SETT_MAX,
	MB_VERSION = 80,
	MB_RESET = 99,
	MB_SET_SETT0 = 100,
	MB_SET_SETT_MB_BAUD,
	MB_SET_SETT_MB_SLAVE,
	MB_SET_SETT3,
	MB_SET_SETT4,
	MB_SET_SETT5,
	MB_SET_SETT6,
	MB_SET_SETT7,
	MB_SET_SETT8,
	MB_SET_SETT_MAX,
	MB_UPDATE_CMD = 256,
	MB_UPDATE_ADDR,
	MB_UPDATE_LEN,
	MB_UPDATE_DATA,
};

#endif // MAIN_H

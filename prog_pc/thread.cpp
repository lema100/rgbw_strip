#include "thread.h"

void thread::run(){
	QTimer timer;
	connection_status=0;

	connect(&timer, SIGNAL(timeout()), this, SLOT(m_Timeout()));
	timer.start(50);
	qDebug() << "Transmiter start, thread_id = " << QThread::currentThreadId();

	exec();
}

int thread::mb_err(modbus_t * my_modbus, QString err)
{
	_cmd = 0;
	qDebug() << err;
	modbus_close(my_modbus);
	modbus_free(my_modbus);
	emit Print_to_textBrowser(err);
	return -1;
}

int thread::m_Timeout(){
	uint16_t tab_reg[UINT8_MAX];

	if (connection_status)
	{
		qDebug() << "Transmiter, thread_id = " << QThread::currentThreadId();

		modbus_t* my_modbus = modbus_new_rtu((char *)modbus_port.com_Port.toLatin1().data(), modbus_port.baud_Rate, 'N', 8, modbus_port.stop_bit);
		modbus_set_slave(my_modbus, modbus_port.address);

		if (modbus_connect(my_modbus) == -1)
			return mb_err(my_modbus, "Error conection");

		if ((modbus_read_registers(my_modbus, 0, 120, tab_reg)) == -1)
		{
			return mb_err(my_modbus, "Error read data");
		}
		else
		{
			QList<uint16_t> list;
			device_data data;

			for (uint16_t i = 0; i < 120; i++)
				list.append(tab_reg[i]);
			qDebug() << list;

			data.address = tab_reg[MB_SETT_MB_SLAVE];
			data.baudrate = tab_reg[MB_SETT_MB_BAUD] * 10;
			data.version = QString::fromLatin1((char *) &tab_reg[MB_VERSION]);

			switch (_cmd)
			{
			case CMD_SET_COLOR1:
			case CMD_SET_COLOR2:
			{
				tab_reg[0] = (_cmd_arg & 0xFF000000) >> 24;
				tab_reg[1] = (_cmd_arg & 0x00FF0000) >> 16;
				tab_reg[2] = (_cmd_arg & 0x0000FF00) >> 8;
				tab_reg[3] = (_cmd_arg & 0x000000FF) >> 0;
				if ((modbus_write_registers(my_modbus, _cmd == CMD_SET_COLOR1 ? MB_PWM_W : MB_PWM2_W, 4, tab_reg)) == -1)
					return mb_err(my_modbus, "Error write color cmd");
				_cmd = 0;
				emit Print_to_textBrowser("Write color OK");
				break;
			}
			case CMD_RESET:
			{
				tab_reg[0] = 1;
				if ((modbus_write_registers(my_modbus, MB_RESET, 1, tab_reg)) == -1)
					return mb_err(my_modbus, "Error reset cmd");
				_cmd = 0;
				emit Print_to_textBrowser("Reset OK");
				break;
			}
			case CMD_WRITE_BAUDRATE:
			{
				tab_reg[0] = _cmd_arg / 10;
				if ((modbus_write_registers(my_modbus, MB_SET_SETT_MB_BAUD, 1, tab_reg)) == -1)
					return mb_err(my_modbus, "Error write baudrate cmd");
				_cmd = 0;
				emit Print_to_textBrowser("Write baudrate OK");
				break;
			}
			case CMD_WRITE_ADDRESS:
			{
				tab_reg[0] = _cmd_arg;
				if ((modbus_write_registers(my_modbus, MB_SET_SETT_MB_SLAVE, 1, tab_reg)) == -1)
					return mb_err(my_modbus, "Error write address cmd");
				_cmd = 0;
				emit Print_to_textBrowser("Write address OK");
				break;
			}
			default:
				emit device_read(data);
				emit Print_to_textBrowser("Read OK");
				break;
			}
		}
		modbus_close(my_modbus);
		modbus_free(my_modbus);
	}
	return 1;
}


void thread::connect_to(port_parametrs par)
{
	modbus_port = par;
	connection_status = 1;
}

void thread::disconect()
{
	connection_status = 0;
}

void thread::cmd(uint32_t cmd, uint32_t cmd_arg)
{
	_cmd = cmd;
	_cmd_arg = cmd_arg;
}

void thread::write_parametrs(device_parametrs par)
{
	_cmd = CMD_WRITE_PARAM;
}

void thread::burn_bin(QByteArray bin)
{
	_bin = bin;
	_cmd = CMD_BURN_BIN;
}

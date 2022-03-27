#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowTitle(windowTitle() + "   " + __DATE__);

	on_comboBox_activated(0);

	colorDialog = new QColorDialog(this);
	ui->verticalLayout_2->addWidget(colorDialog);
	colorDialog->setWindowFlags(Qt::Widget);
	colorDialog->setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons);

	timer = new QTimer;
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	QObject::connect(colorDialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(currentColorChanged(const QColor &)));
	timer->start(500);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::currentColorChanged(const QColor & color)
{
	update_color(color, ui->horizontalSlider_white->value());
}

void MainWindow::on_horizontalSlider_white_valueChanged(int value)
{
	update_color(colorDialog->currentColor(), value);
}

void MainWindow::on_pushButton_reset_clicked(bool checked)
{
	(void) checked;
	emit cmd(CMD_RESET, 0);
}

void MainWindow::on_pushButton_burn_update_clicked(bool checked)
{
	(void) checked;

	auto file_name = QFileDialog::getOpenFileName(this, "Open File", "", "Binary (*.bin)");
	if (!file_name.isEmpty())
	{
		QByteArray data;

		auto file = std::make_shared<QFile>(file_name);
		file->open(QIODevice::ReadOnly);
		data = file->readAll();
		file->close();
		emit burn_bin(data);
	}
}

void MainWindow::on_pushButton_check_update_clicked(bool checked)
{
	(void) checked;
	emit cmd(CMD_CHECK_BIN, 0);
}

void MainWindow::on_lineEdit_slave_par_textChanged(const QString &text)
{
	emit cmd(CMD_WRITE_ADDRESS, text.toUInt());
}

void MainWindow::on_comboBox_baudrate_par_currentTextChanged(const QString &text)
{
	emit cmd(CMD_WRITE_BAUDRATE, text.toUInt());
}

void MainWindow::update_color(const QColor & color, int white)
{
	uint32_t arg = 0;
	qDebug() << color.red() << color.blue() << color.green();
	arg += white;
	arg <<= 8;
	arg += color.blue();
	arg <<= 8;
	arg += color.green();
	arg <<= 8;
	arg += color.red();
	emit cmd(ui->comboBox_chanel->currentIndex() == 0 ? CMD_SET_COLOR1 : CMD_SET_COLOR2, arg);
}

void MainWindow::timeout(void)
{
	qDebug() << "Timeout";
}

void MainWindow::device_param(device_parametrs par)
{
}

void MainWindow::device_read(device_data data)
{
	ui->label_version->setText("Version: " + data.version);
	if (!ui->lineEdit_slave_par->hasFocus())
		ui->lineEdit_slave_par->setText(QString::number(data.address));
	if (!ui->comboBox_baudrate_par->hasFocus())
		ui->comboBox_baudrate_par->setCurrentText(QString::number(data.baudrate));
}

void MainWindow::Print_to_textBrowser(const QString & text)
{
	static uint32_t i;
	i++;
	ui->textBrowser->append(QString("%1: %2").arg(i).arg(text));
}

void MainWindow::on_comboBox_activated(int index)
{
	qDebug() << "Combobox clicked" << index;

	if (string_comport.size() != 0)
	{
		port_param.com_Port = string_comport[ui->comboBox->currentIndex()];
		port_param.baud_Rate = ui->comboBox_baudrate->currentText().toInt();
		port_param.stop_bit = 1;
		port_param.address = ui->lineEdit_slave->text().toInt();

		emit connect_to(port_param);
	}

	ui->comboBox->clear();
	string_comport.clear();
	for (const auto & info : QSerialPortInfo::availablePorts())
	{
		QString s = info.portName() + ": " + info.description();
		ui->comboBox->addItem(s);
		string_comport.append(info.systemLocation());
	}
}

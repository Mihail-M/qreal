#include <utils/tcpRobotCommunicator.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QFileInfo>

#include <QtCore/QDebug>

#include <qrkernel/settingsManager.h>
#include <qrkernel/exception/exception.h>
#include <qrutils/inFile.h>

using namespace utils;

static uint const port = 8888;

TcpRobotCommunicator::TcpRobotCommunicator(const QString &settings)
	: mIsConnected(false),
	  mSettings(settings)
{
}

TcpRobotCommunicator::~TcpRobotCommunicator()
{
	disconnect();
}

bool TcpRobotCommunicator::uploadProgram(QString const &programName)
{
	if (programName.isEmpty()) {
		return false;
	}

	QString fileContents;
	try {
		fileContents = utils::InFile::readAll(programName);
	} catch (qReal::Exception const &) {
		return false;
	}

	connect();
	if (mSocket.state() != QAbstractSocket::ConnectedState) {
		return false;
	}

	QString const &fileNameOnARobot = QFileInfo(programName).fileName();

	QString const command = "file:" + fileNameOnARobot + ":" + fileContents;
	mSocket.write(command.toUtf8());
	mSocket.waitForBytesWritten(3000);

	disconnect();

	return true;
}

bool TcpRobotCommunicator::runProgram(QString const &programName)
{
	connect();
	if (mSocket.state() != QAbstractSocket::ConnectedState) {
		return false;
	}

	QString const command = "run:" + programName;
	mSocket.write(command.toUtf8());
	mSocket.waitForBytesWritten(3000);

	disconnect();

	return true;
}

bool TcpRobotCommunicator::runDirectCommand(QString const &directCommand)
{
	connect();
	if (mSocket.state() != QAbstractSocket::ConnectedState) {
		return false;
	}

	QString const command = "direct:" + directCommand;
	mSocket.write(command.toUtf8());
	mSocket.waitForBytesWritten(3000);

	disconnect();

	return true;
}

bool TcpRobotCommunicator::stopRobot()
{
	connect();
	if (mSocket.state() != QAbstractSocket::ConnectedState) {
		return false;
	}

	QString const command = "stop";
	mSocket.write(command.toUtf8());
	mSocket.waitForBytesWritten(3000);

	disconnect();

	return true;
}

void TcpRobotCommunicator::connect()
{
	if (mIsConnected) {
		return;
	}

	///"TrikTcpServer"
	QString const server = qReal::SettingsManager::value(mSettings).toString();
	QHostAddress hostAddress(server);
	if (hostAddress.isNull()) {
		qDebug() << "Unable to resolve host. Check server address and try again";
		return;
	}

	mSocket.connectToHost(hostAddress, static_cast<quint16>(port));
	bool const result = mSocket.waitForConnected(5000);
	if (!result) {
		qDebug() << mSocket.errorString();
	}

	mIsConnected = true;
	emit connected(result);
}

void TcpRobotCommunicator::disconnect()
{
	if (mSocket.state() == QTcpSocket::ConnectedState) {
		mSocket.disconnectFromHost();
		mSocket.waitForDisconnected(3000);
	}

	mIsConnected = false;
	emit disconnected();
}
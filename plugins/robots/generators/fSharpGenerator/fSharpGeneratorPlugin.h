#pragma once

#include <generatorBase/robotsGeneratorPluginBase.h>

namespace fSharp {

/// Generation of F# program for TRIK, uploading and execution of a program.
/// Uses setting "tcpServer" from RobotsInterpreter.
class FSharpGeneratorPlugin : public generatorBase::RobotsGeneratorPluginBase
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "trik.FSharpGeneratorPlugin")

public:
	FSharpGeneratorPlugin();
	virtual ~FSharpGeneratorPlugin();

	QString kitId() const override;

	QList<qReal::ActionInfo> actions() override;
	QList<HotKeyActionInfo> hotKeyActions() override;

protected:
	generatorBase::MasterGeneratorBase *masterGenerator() override;
	void regenerateExtraFiles(QFileInfo const &newFileInfo) override;
	QString defaultFilePath(QString const &projectName) const override;
	QString extension() const override;
	QString extensionDescription() const override;
	QString generatorName() const override;

private slots:

	/// Generates and uploads script to a robot. Program then can be launched manually or remotely
	/// by runCommand. Program is stored on robot as a file in /home/root/trik/scripts and named
	/// as <qReal save name>.fs.
	/// @returns True, if successful.
	bool uploadProgram();

	/// Runs currently opened program on a robot. Uploads it first.
	void runProgram();

	/// Tries to remotely abort script execution and stop robot.
	void stopRobot();

private:
	/// Action that launches code generator
	QAction mGenerateCodeAction;

	/// Action that generates and uploads program on a robot
	QAction mUploadProgramAction;

	/// Action that generates and uploads program on a robot
	QAction mRunProgramAction;

	/// Action that stops script execution and turns off motors.
	QAction mStopRobotAction;

	QTranslator mAppTranslator;
};

}

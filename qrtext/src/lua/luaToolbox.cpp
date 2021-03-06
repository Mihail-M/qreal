#include "qrtext/lua/luaToolbox.h"

#include <qslog/QsLog.h>

#include "qrtext/src/lua/luaLexer.h"
#include "qrtext/src/lua/luaParser.h"
#include "qrtext/src/lua/luaSemanticAnalyzer.h"
#include "qrtext/src/lua/luaInterpreter.h"

using namespace qrtext::lua;
using namespace qrtext::core;
using namespace qrtext::core::ast;

LuaToolbox::LuaToolbox()
	: mLexer(new details::LuaLexer(mErrors))
	, mParser(new details::LuaParser(mErrors))
	, mAnalyzer(new details::LuaSemanticAnalyzer(mErrors))
	, mInterpreter(new details::LuaInterpreter(mErrors))
{
}

LuaToolbox::~LuaToolbox()
{
}

QVariant LuaToolbox::interpret(QSharedPointer<Node> const &root)
{
	auto const result = mInterpreter->interpret(root, *mAnalyzer);
	reportErrors();
	return result;
}

void LuaToolbox::interpret(qReal::Id const &id, QString const &propertyName, QString const &code)
{
	interpret<int>(id, propertyName, code);
}

void LuaToolbox::interpret(QString const &code)
{
	interpret<int>(qReal::Id(), "", code);
}

QSharedPointer<Node> const &LuaToolbox::parse(qReal::Id const &id, QString const &propertyName
		, QString const &code)
{
	mErrors.clear();

	if (mParsedCache[id][propertyName] != code) {
		auto tokenStream = mLexer->tokenize(code);
		auto ast = mParser->parse(tokenStream);

		if (mErrors.isEmpty()) {
			mAstRoots[id][propertyName] = ast;
			/// Note that analyze() changes mErrors, so these two ifs are correct.
			mAnalyzer->analyze(ast);
		}

		if (mErrors.isEmpty()) {
			mParsedCache[id][propertyName] = code;
		} else {
			reportErrors();
		}
	}

	return mAstRoots[id][propertyName];
}

QSharedPointer<Node> LuaToolbox::ast(qReal::Id const &id, QString const &propertyName) const
{
	return mAstRoots[id][propertyName];
}

QSharedPointer<qrtext::core::types::TypeExpression> LuaToolbox::type(QSharedPointer<Node> const &expression) const
{
	return mAnalyzer->type(expression);
}

QList<Error> const &LuaToolbox::errors() const
{
	return mErrors;
}

void LuaToolbox::addIntrinsicFunction(QString const &name
		, core::types::TypeExpression * const returnType
		, QList<core::types::TypeExpression *> const &parameterTypes
		, std::function<QVariant(QList<QVariant> const &)> const &semantic)
{
	QList<QSharedPointer<core::types::TypeExpression>> wrappedParameterTypes;
	for (core::types::TypeExpression * const type : parameterTypes) {
		wrappedParameterTypes << QSharedPointer<core::types::TypeExpression>(type);
	}

	auto functionType = QSharedPointer<types::Function>(new types::Function(
			QSharedPointer<core::types::TypeExpression>(returnType)
			, wrappedParameterTypes
			));

	mAnalyzer->addIntrinsicFunction(name, functionType);
	mInterpreter->addIntrinsicFunction(name, semantic);
}

QStringList LuaToolbox::identifiers() const
{
	return mInterpreter->identifiers();
}

QVariant LuaToolbox::value(QString const &identifier) const
{
	return mInterpreter->value(identifier);
}

void LuaToolbox::setVariableValue(QString const &name, QString const &initCode, QVariant const &value)
{
	if (!mInterpreter->identifiers().contains(name)) {
		parse(qReal::Id(), "", initCode);
	}

	mInterpreter->setVariableValue(name, value);
}

void LuaToolbox::reportErrors()
{
	for (qrtext::core::Error const &error : mErrors) {
		if (error.severity() == Severity::internalError) {
			QLOG_ERROR() << QString("Parser internal error at %1:%2 when parsing %3:%4: %5")
					.arg(error.connection().line())
					.arg(error.connection().column())
					.arg(error.connection().id().toString())
					.arg(error.connection().propertyName())
					.arg(error.errorMessage());
		}
	}
}

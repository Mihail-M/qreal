#include "gesturesWidget.h"

#include "ui_gesturesWidget.h"
#include "gesturePainter.h"

using namespace qReal::gestures;

int const gestureAreaSize = 450;
int const pointsAtSegment = 5;

GesturesWidget::GesturesWidget(QWidget *parent)
	: QWidget(parent)
	, mUi(new Ui::GesturesWidget)
{
	mUi->setupUi(this);
	connect(mUi->listWidget, &QListWidget::currentItemChanged, this, &GesturesWidget::currentElementChanged);
}

GesturesWidget::~GesturesWidget()
{
	delete mUi;
}

void GesturesWidget::draw(QString const &paths)
{
	QSize const size(gestureAreaSize, gestureAreaSize);
	GesturePainter painter(paths, Qt::white, Qt::blue, gestureAreaSize);
	QPixmap const gestureIcon = painter.pixmap(size, QIcon::Mode::Normal, QIcon::State::Off);
	mUi->gesturePixmap->setPixmap(gestureIcon);
}

int GesturesWidget::coord(int previous, int next, int part)
{
	return previous + (next - previous) * part / pointsAtSegment;
}

qReal::Id GesturesWidget::currentElement() const
{
	return mUi->listWidget->currentItem()->data(Qt::UserRole).value<qReal::Id>();
}

void GesturesWidget::setElements(QList<QPair<QString, qReal::Id> > const &elements)
{
	mUi->listWidget->clear();
	QListIterator<QPair<QString, qReal::Id> > iterator(elements);
	while (iterator.hasNext()) {
		QPair<QString, qReal::Id> const element(iterator.next());
		QListWidgetItem *item = new QListWidgetItem(element.first);
		item->setData(Qt::UserRole, QVariant::fromValue(element.second));
		mUi->listWidget->addItem(item);
	}
}

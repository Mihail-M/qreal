#pragma once

#include <QtWidgets/QGraphicsItem>

#include "../utilsDeclSpec.h"
#include "abstractItem.h"

namespace graphicsUtils
{

class QRUTILS_EXPORT RotateItem : public AbstractItem
{
public:
	virtual void setRotation(qreal angle) = 0;
	virtual QRectF rect() const = 0;
	qreal horizontalRadius() const;
	virtual void setSelected(bool isSelected) = 0;
	virtual void checkSelection() = 0;
};

}

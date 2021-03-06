#include "objectpixmapitem.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QApplication>
#include <QErrorMessage>
#include <QTimer>
#include <QString>
#include <QScreen>
#include <QGuiApplication>
#include <QFileInfo>
#include <QPixmap>

#include <QGraphicsWidget>
#include <QGraphicsView>

#include <QPropertyAnimation>

ObjectPixmapItem::ObjectPixmapItem(QString fileName, QObject *parent) :
    QObject(parent)
{
    _fileName = fileName;
    _showTimer = NULL;

    _scale = 150;

    _pictureData = new ExifMetadata(fileName);
}

ObjectPixmapItem::ObjectPixmapItem(const QPixmap& pixmap, QObject* parent) :
    QObject(parent),
    QGraphicsPixmapItem(pixmap)
{
    _pictureData = NULL;
    _showTimer = NULL;

    _scale = 150;

    setCacheMode(DeviceCoordinateCache);
}

ObjectPixmapItem::~ObjectPixmapItem()
{
    if (_pictureData != NULL)
        delete _pictureData;
}

void ObjectPixmapItem::load()
{
    if (!_realImage.load(_fileName)) {
        qDebug () << "Cannot read the file " << _fileName;
    }

    if (!_realImage.isNull()) {
        calculateScale();

        correctOrientationPicture();

        //_minimalZoom = _zoomPercent;

        refresh ();
    }

    emit itemLoaded();
}

void ObjectPixmapItem::refresh()
{
    qDebug () << "ZoomLevel (refresh): " << _scale;

    //setPixmap (scaledImage (_correctedImage));
    setPixmap (_correctedImage);

    // this->scene()->setSceneRect(_correctedImage.rect());

    centerOnScene ();
}

void ObjectPixmapItem::resize()
{
    qDebug () << "ZoomLevel (resize): " << _scale;

    calculateScale();
    correctOrientationPicture();

    refresh();
}

QDateTime ObjectPixmapItem::getDate()
{
    QFileInfo info;
    QDateTime result;

    info.setFile(_fileName);
    result = _pictureData->pictureDate();
    if (result == QDateTime::fromTime_t(0)) {
        result = info.created();
    }

    return result;
}

AbstractMetadata *ObjectPixmapItem::metadata ()
{
    return _pictureData;
}


/**
 * @brief Get the current picture in the correct orientation
 * @return
 *
 * Rotate the selected picture in the correct orientation.
 * EXIF Orientation can has this values:
 * Value    0th Row        0th Column
 *   1        top          left side
 *   2        top          right side
 *   3        bottom       right side
 *   4        bottom       left side
 *   5        left side    top
 *   6        right side   top
 *   7        right side   bottom
 *   8        left side    bottom
 *
 * Here is what the letter F would look like if it were tagged correctly and
 * displayed by a program that ignores the orientation tag:
 *    1        2       3      4          5           6          7            8
 *  888888  888888      88  88      8888888888  88                  88  8888888888
 *  88          88      88  88      88  88      88  88          88  88      88  88
 *  8888      8888    8888  8888    88          8888888888  8888888888          88
 *  88          88      88  88
 *  88          88  888888  888888
 *
 * http://sylvana.net/jpegcrop/exif_orientation.html
 */
void ObjectPixmapItem::correctOrientationPicture()
{
    QTransform trans;

    // Set the correct orientation
    switch (_pictureData->orientation())
    {
    case 1:
        // Nothing to do
        break;

    case 2:
        // Horizontal mirror
        trans.scale(-1, 1);
        break;

    case 3:
        // Rotate 180 degrees
        trans.rotate(180);
        break;

    case 4:
        // Vertical mirror
        trans.scale(1, -1);
        break;

    case 5:
        // Vertical mirror and rotate 90 degrees
        trans.scale(1, -1);
        trans.rotate(90);
        break;

    case 6:
        // Rotate 90 degrees
        trans.rotate(90);
        break;

    case 7:
        // Horizontal mirror and rotate 90 degrees
        trans.scale(-1, 1);
        trans.rotate(90);
        break;

    case 8:
        // Rotate 270 degrees
        trans.rotate(270);
        break;
    }

    _correctedImage = scaledImage(_realImage.transformed(trans));
}

QPixmap ObjectPixmapItem::scaledImage (QPixmap src)
{
/*
    QPixmap image;

    image = src.scaled(src.width() * _zoomPercent, src.height() * _zoomPercent, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (_zoomPercent == _minimalZoom) {
        this->scene()->setSceneRect(this->scene()->views()[0]->rect());
    }
    else {
        this->scene()->setSceneRect(image.rect());
    }

    if (_zoomPercent == 1) {
        this->scene()->setSceneRect(this->scene()->views()[0]->rect());
    }

    image = src.scaledToWidth((this->scene()->width() * _zoomPercent), Qt::SmoothTransformation);
    if (_zoomPercent == 1 && image.height() > this->scene()->height()) {
        image = src.scaledToHeight((this->scene()->height() * _zoomPercent), Qt::SmoothTransformation);
    }

    this->scene()->setSceneRect(image.rect());
*/
/*
    this->scene()->setSceneRect(0, 0, round(_realImage.width() * _scale), round(_realImage.height() * _scale));

    qDebug () << "Scene: " << this->scene()->width() << "x" << this->scene()->height();

    image = src.scaledToWidth((this->scene()->width()), Qt::SmoothTransformation);
    qDebug () << "Image: " << image.width() << "x" << image.height();
    if (image.height() > this->scene()->height()) {
        qDebug () << "ScaledToHeight width: " << image.width() << "x" << image.height();
        image = src.scaledToHeight((this->scene()->height()), Qt::SmoothTransformation);
    }
*/

    QPixmap image;

    //image = src.scaledToWidth(this->scene()->width(), Qt::FastTransformation);
    image = src.scaledToWidth(this->scene()->width(), Qt::SmoothTransformation);
    if (image.height() > this->scene()->height()) {
        // image = src.scaledToHeight(this->scene()->height(), Qt::FastTransformation);
        image = src.scaledToHeight(this->scene()->height(), Qt::SmoothTransformation);
    }

    return image;

    return image;
}

void ObjectPixmapItem::centerOnScene()
{
    qreal newX;
    qreal newY;
    qreal adjustWidth;
    qreal adjustHeight;
    QPixmap image;

    adjustWidth = this->scene()->width();
    adjustHeight = this->scene()->height();
    image = this->pixmap();

    if (image.width() < adjustWidth) {
        newX = (adjustWidth / 2) - (image.width() / 2);
    }
    else {
        newX = 0;
    }

    if (image.height() < adjustHeight) {
        newY = (adjustHeight / 2) - (image.height() / 2);
    }
    else {
        newY = 0;
    }

    QTransform transform;

    transform.translate(newX, newY);
    this->setTransform(transform);
}

void ObjectPixmapItem::connectNotify ( const char * signal )
{
    Q_UNUSED(signal);
}

void ObjectPixmapItem::setShowTime(int time)
{
    if (time > 0) {
        _showTimer = new QTimer(this);
        _showTimer ->setInterval(time);
        this->connect(_showTimer, SIGNAL(timeout()),
                      this, SLOT(on_showTimeEnded()));

        _showTimer->setSingleShot(true);
        _showTimer->start();
    }
    else {
        if (_showTimer) {
            if (_showTimer->isActive()) {
                _showTimer->stop();
            }

            delete _showTimer;
            _showTimer = NULL;
        }
    }
}

void ObjectPixmapItem::on_showTimeEnded()
{
    emit showTimeEnded();
}

bool ObjectPixmapItem::rotateLeft()
{
    switch (_pictureData->orientation())
    {
    case 1:
        _pictureData->setOrientation (8);
        break;

    case 2:
        break;

    case 3:
        _pictureData->setOrientation (6);
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        _pictureData->setOrientation (1);
        break;

    case 7:
        break;

    case 8:
        _pictureData->setOrientation (3);
        break;
    }

    return true;
}

bool ObjectPixmapItem::rotateRight()
{
    switch (_pictureData->orientation())
    {
    case 1:
        _pictureData->setOrientation (6);
        break;

    case 2:
        break;

    case 3:
        _pictureData->setOrientation (8);
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        _pictureData->setOrientation (3);
        break;

    case 7:
        break;

    case 8:
        _pictureData->setOrientation (1);
        break;
    }

    return true;
}

void ObjectPixmapItem::endRotateAnimation()
{
    correctOrientationPicture();
    this->setRotation(0);
    this->setScale(1);
}

void ObjectPixmapItem::setItemRotation(qreal angle)
{
    QGraphicsItem::setRotation(angle);
}

void ObjectPixmapItem::setItemScale(qreal scale)
{
    QGraphicsItem::setScale(scale);
}

qreal ObjectPixmapItem::itemRotation()
{
    return QGraphicsItem::rotation();
}

qreal ObjectPixmapItem::itemScale()
{
    return QGraphicsItem::scale();
}

void ObjectPixmapItem::setZoom(qreal zoomPercent)
{
    _scale = (zoomPercent / 100);

    // correctOrientationPicture();
    // refresh();
}

qreal ObjectPixmapItem::zoom()
{
    return _scale * 100;
}

void ObjectPixmapItem::calculateScale ()
{
    _scale = (this->scene()->width() / _realImage.width());
    if (_scale * _realImage.height() > this->scene()->height()) {
        _scale = (this->scene()->height() / _realImage.height());
    }

    emit zoomChanged (zoom());
}

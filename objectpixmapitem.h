#ifndef OBJECTPIXMAPITEM_H
#define OBJECTPIXMAPITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

#include "pictureviewitem.h"
#include "exifmetadata.h"

class ObjectPixmapItem : public QObject,
        public QGraphicsPixmapItem,
        public PictureViewItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    explicit ObjectPixmapItem(QString fileName, QObject *parent = 0);
    explicit ObjectPixmapItem(const QPixmap& pixmap, QObject *parent = 0);
    ~ObjectPixmapItem();

    void load (bool fireEvent);
    void resize ();
    QDateTime getDate ();

    AbstractMetadata *metadata ();

    void setShowTime (int time);

    bool rotateLeft();
    bool rotateRight();

signals:
    void itemLoaded ();
    void showTimeEnded ();

public slots:
    void on_showTimeEnded ();

protected:
    void connectNotify ( const char * signal );

private:
    QString _fileName;
    ExifMetadata *_pictureData;
    QPixmap _correctedImage;

    QTimer *_showTimer;

    QPixmap correctOrientationPicture(QPixmap src);
    QPixmap scaledImage(QPixmap src);
    void centerOnScene ();
};

#endif // OBJECTPIXMAPITEM_H
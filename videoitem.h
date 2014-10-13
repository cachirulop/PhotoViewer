#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QGraphicsRectItem>
#include <QPushButton>

#include "pictureviewitem.h"
#include "exifmetadata.h"
#include "videocontrolpanel.h"

class VideoItem : public QGraphicsVideoItem,
        public PictureViewItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    explicit VideoItem(QString fileName, QObject *parent = 0);
    ~VideoItem();

    void load ();
    void resize ();
    QDateTime getDate ();

    AbstractMetadata *metadata ();

    void setShowTime (int time);

    bool rotateLeft();
    bool rotateRight();

signals:
    void itemLoaded ();
    void playMedia();
    void pauseMedia();
    void stopMedia();
    void volumeChanged(int volume);

    void showTimeEnded ();

private slots:

public slots:
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_videoAvailableChanged(bool available);
    void on_stateChanged(QMediaPlayer::State state);
    void on_volumeChanged(int volume);
    void on_nativeSizeChanged(const QSizeF & size);

    void on_beginItemAnimation();
    void on_endItemAnimation();

private:
    QMediaPlayer *_player;
    QString _fileName;

    bool _emitShowTimeEnded;

    VideoControlPanel *_panel;

    ExifMetadata *_videoData;

    void createControls ();
};

#endif // VIDEOITEM_H

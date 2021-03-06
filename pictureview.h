#ifndef PICTUREVIEW_H
#define PICTUREVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QAnimationGroup>
#include <QGraphicsItem>
#include <QMouseEvent>

#include "exifmetadata.h"
#include "clickabletextitem.h"
#include "pictureanimation.h"
#include "animateditem.h"

#include "pictureviewitemcontainer.h"

class PictureView : public QGraphicsView
{
    Q_OBJECT
public:
    enum PictureAnimationType {
        None,
        LeftToRight,
        RightToLeft,
        Random
    };

    explicit PictureView(QWidget *parent = 0);
    ~PictureView ();

    bool hasPicture ();
    void setNormalBackground ();
    void setFullScreenBackground ();
    void cleanPicture ();

    double pictureLatitude ();
    double pictureLongitude ();

    void showFileInfo ();
    void showGeoInfo ();
    void showRating ();

    void hideFileInfo ();
    void hideGeoInfo ();
    void hideRating ();

    void setShowTime (int time);

    void rotatePictureLeft ();
    void rotatePictureRight ();

    qreal zoom ();
    void setZoom(qreal zoomPercent);

signals:
    void mouseDoubleClick (QMouseEvent *event);
    void mouseMove (QMouseEvent *event);
    void requestMapWindow (double latitude, double longitude, double altitude);

    void beginItemAnimationIn ();
    void endItemAnimationIn ();
    void beginItemAnimationOut ();
    void endItemAnimationOut ();
    void showTimeEnded ();

    void zoomChanged (qreal zoom);

public slots:
    void loadPicture (QString fileName);
    void showPicture (PictureAnimationType animType /* = PictureAnimationType::None */);
    void setPictureRating (int rating);

private slots:
    void on_finishPrevItemAnimation ();
    void on_finishCurrentItemAnimation ();
    void on_itemRequestMapWindow (double latitude, double longitude, double altitude);
    void on_itemLoaded ();

    void on_showTimeEnded ();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    const int ANIMATION_DURATION_MILLISECONDS = 3000;

    QGraphicsScene *_pictureScene;
    PictureViewItemContainer *_currentItem;
    PictureViewItemContainer *_prevItem;
    QAnimationGroup *_currentAnimation;
    PictureAnimationType _currentAnimationType;
    bool _infoVisible;
    int _showTime;

    QList<AbstractPictureAnimation *> _pictureAnimations;
    QList<AbstractPictureAnimation *> _videoAnimations;
};

#endif // PICTUREVIEW_H

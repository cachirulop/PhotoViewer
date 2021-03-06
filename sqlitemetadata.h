#ifndef SQLITEMETADATA_H
#define SQLITEMETADATA_H

#include <QString>
#include <QDateTime>
#include <QVariant>

#include "abstractmetadata.h"

class SQLiteMetadata : public AbstractMetadata
{
public:
    explicit SQLiteMetadata(QString fileName);
    ~SQLiteMetadata();

    QDateTime pictureDate ();
    int rating ();
    quint16 orientation();

    bool hasGpsInfo ();
    double gpsLatitude ();
    QString gpsLatitudeRef();
    double gpsLongitude();
    QString gpsLongitudeRef();
    double gpsAltitude ();

    void setRating (int value);
    void setOrientation (quint16 value);

signals:

public slots:

private:
    QString _fileName;

    void initDB ();
    void setValue (QString column, QVariant value);

    bool exist ();

    QVariant getValue (QString column);
    int getInt (QString column);
    quint16 getUInt16 (QString column);
    QString getString (QString column);
};

#endif // SQLITEMETADATA_H

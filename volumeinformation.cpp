#include "volumeinformation.h"

#include <QDebug>

#include <QFileInfoList>
#include <QDir>

#ifdef Q_OS_WIN32

#include <qt_windows.h>

VolumeInformation::VolumeInformation(QString path)
{
    WCHAR szVolumeName[256] ;
    WCHAR szFileSystemName[256];
    DWORD dwSerialNumber = 0;
    DWORD dwMaxFileNameLength = 256;
    DWORD dwFileSystemFlags = 0;

    int pos;

    _srcPath = QDir::toNativeSeparators(path);

    pos = _srcPath.indexOf("\\");
    if (pos == -1) {
        _rootPath = "C:\\";
    }
    else {
        _rootPath = _srcPath.left(pos + 1);
    }

    bool ret = GetVolumeInformation( (WCHAR *) _srcPath.left(pos + 1).utf16(),
                                     szVolumeName,
                                     256,
                                     &dwSerialNumber,
                                     &dwMaxFileNameLength,
                                     &dwFileSystemFlags,
                                     szFileSystemName,
                                     256);

    if (!ret) {
        _uuid = QString(QObject::tr("Unknown"));
    }
    else {
        _uuid = QString::number(dwSerialNumber, 16);
    }

    qDebug () << _srcPath << "-.-" << _rootPath << "-.-" << _uuid << "-.-" << filePath() << "-.-" << fileName();
}

#endif

#ifdef Q_OS_LINUX

#include <QDBusReply>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>

extern "C" {
#include <stdio.h>
#include <mntent.h>
#include <blkid/blkid.h>
}

VolumeInformation::VolumeInformation (QString path)
{
    _srcPath = path;

    QFileInfoList fileinfolist = QDir(QString(QStringLiteral("/dev/disk/by-uuid/"))).entryInfoList(QDir::AllEntries | QDir::NoDot | QDir::NoDotDot);
    if (!fileinfolist.isEmpty()) {
        FILE *fsDescription;
        struct mntent entry;
        char buffer[512];
        QString currentPath;

        fsDescription = setmntent(_PATH_MOUNTED, "r");
        while ((getmntent_r(fsDescription, &entry, buffer, sizeof(buffer))) != NULL) {
            currentPath = QString::fromLatin1(entry.mnt_dir);
            if (path.startsWith(currentPath)) {
                int idx;

                idx = fileinfolist.indexOf(QString::fromLatin1(entry.mnt_fsname));
                if (idx != -1) {
                    _rootPath = currentPath;
                    _uuid = fileinfolist[idx].fileName();
                    if (currentPath != "/") {
                        break;
                    }
                }
            }
        }

        endmntent(fsDescription);
    }

/*
#if !defined(QT_NO_UDISKS)
    QDBusReply<QList<QDBusObjectPath> > reply = QDBusConnection::systemBus().call(
                QDBusMessage::createMethodCall(QString(QStringLiteral("org.freedesktop.UDisks")),
                                               QString(QStringLiteral("/org/freedesktop/UDisks")),
                                               QString(QStringLiteral("org.freedesktop.UDisks")),
                                               QString(QStringLiteral("EnumerateDevices"))));
    if (reply.isValid()) {
        QList<QDBusObjectPath> paths = reply.value();
        foreach (const QDBusObjectPath &currentPath, paths) {
            QDBusInterface interface(QString(QStringLiteral("org.freedesktop.UDisks")), currentPath.path(),
                                     QString(QStringLiteral("org.freedesktop.DBus.Properties")),
                                     QDBusConnection::systemBus());
            if (!interface.isValid())
                continue;
            QDBusReply<QVariant> reply = interface.call(QString(QStringLiteral("Get")),
                                                        QString(QStringLiteral("org.freedesktop.UDisks.Device")),
                                                        QString(QStringLiteral("DeviceMountPaths")));
            if (reply.isValid() && reply.value().toString() == path) {
                reply = interface.call(QString(QStringLiteral("Get")),
                                       QString(QStringLiteral("org.freedesktop.UDisks.Device")),
                                       QString(QStringLiteral("IdUuid")));
                if (reply.isValid())
                    return reply.value().toString();
            }
        }
    }
#endif // QT_NO_UDISKS
*/

}

#endif

QString VolumeInformation::uuid()
{
    return _uuid;
}

QString VolumeInformation::filePath()
{
    QFileInfo info (_srcPath);

    if (_rootPath == "/") {
        return info.absolutePath();
    }
    else {
        QString result;

        result = info.absolutePath().remove(0, _rootPath.length());
        if (!result.startsWith("/")) {
            result.insert(0, "/");
        }

        return result;
    }
}

QString VolumeInformation::rootPath()
{
    return _rootPath;
}

QString VolumeInformation::fileName()
{
    QFileInfo info;

    info.setFile(QDir::fromNativeSeparators(_srcPath));

    return info.fileName();
}





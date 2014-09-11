#include "photoviewer.h"
#include "ui_photoviewer.h"

#include <QFileDialog>
#include <QDir>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QScrollArea>
#include <QDebug>
#include <QSettings>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>

#include "settingshelper.h"
#include "starsaction.h"

/**
 * PhotoView application to view pictures in a simple mode
 *
 * @brief PhotoViewer::PhotoViewer
 * @param parent
 */
PhotoViewer::PhotoViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PhotoViewer)
{
    ui->setupUi(this);

    StarsAction *action;

    action = new StarsAction (ui->mainToolBar);
    ui->mainToolBar->addAction(action);

    QObject::connect(action,
                     SIGNAL(setRating(int)),
                     ui->gvPicture,
                     SLOT(setPictureRating(int)));

    QString lastDirectory;
    QStringList filters;

    filters << "*.gif" << "*.jpg" << "*.jpeg" << "*.png";

    lastDirectory = SettingsHelper::instance ().lastDirectory ();

    _currentDir = new QDir ();
    _currentDir->setFilter (QDir::Files | QDir::Readable);
    _currentDir->setSorting (QDir::Name);
    _currentDir->setPath(lastDirectory);
    _currentDir->setNameFilters(filters);

    _currentFile = 0;

    if (lastDirectory.compare("~") != 0) {
        showCurrentPicture();
    }

    QObject::connect(ui->gvPicture,
                     SIGNAL(mouseDoubleClick(QMouseEvent*)),
                     this,
                     SLOT(on_pictureDoubleClick()));

    _playerTimer = new QTimer (this);
    connect (_playerTimer, SIGNAL(timeout()), this, SLOT(on_playerTimerTimeout()));
}

PhotoViewer::~PhotoViewer()
{
    delete ui;
    delete _currentDir;
}

void PhotoViewer::on_actionChange_folder_triggered()
{
    QString directory;
    QFileDialog *fd = new QFileDialog;

    fd->setDirectory(SettingsHelper::instance ().lastDirectory ());
    fd->setFileMode (QFileDialog::Directory);
    fd->setOption (QFileDialog::ShowDirsOnly);
    fd->setViewMode (QFileDialog::Detail);

    int result = fd->exec();
    if (result)
    {
        directory = fd->selectedFiles ()[0];
        SettingsHelper::instance().setLastDirectory (directory);

        _currentDir->setPath (fd->selectedFiles()[0]);
        _currentFile = 0;
        showCurrentPicture ();
    }

    delete fd;
}

void PhotoViewer::on_actionNext_picture_triggered()
{
    if (_currentFile < _currentDir->entryList().count() - 1) {
        _currentFile++;
        showCurrentPicture(PictureView::PictureAnimationType::RightToLeft);
    }
    else {
        QMessageBox::information(this,
                                 tr("Image Viewer"),
                                 tr("Last picture"));
    }
}

void PhotoViewer::on_actionPrevious_picture_triggered()
{

    if (_currentFile > 0) {
        _currentFile--;
        showCurrentPicture(PictureView::PictureAnimationType::LeftToRight);
    }
    else {
        QMessageBox::information(this,
                                 tr("Image Viewer"),
                                 tr("First picture"));
    }
}

void PhotoViewer::showCurrentPicture(PictureView::PictureAnimationType anim)
{
    QString fileName;

    fileName = _currentDir->absoluteFilePath(_currentDir->entryList()[_currentFile]);

    ui->gvPicture->loadPicture (fileName);
    ui->gvPicture->showPicture (anim);
}

void PhotoViewer::on_pictureDoubleClick()
{
    toggleFullScreen();
}

void PhotoViewer::toggleFullScreen()
{
    if (this->isFullScreen()) {
        QList<QStatusBar *> statusbars;
        statusbars = this->findChildren<QStatusBar*> ();
        for (int i = 0; i < statusbars.length(); i++) {
            statusbars[i]->show();
        }
        QList<QMenuBar *> menubars;
        menubars = this->findChildren<QMenuBar*> ();
        for (int i = 0; i < menubars.length(); i++) {
            menubars[i]->show();
        }

        if (_lastStatusMaximized) {
            this->showMaximized();
        }
        else {
            this->showNormal();
        }
    }
    else {
        _lastStatusMaximized = this->isMaximized();

        QList<QStatusBar *> statusbars;
        statusbars = this->findChildren<QStatusBar*> ();
        for (int i = 0; i < statusbars.length(); i++) {
            statusbars[i]->hide();
        }
        QList<QMenuBar *> menubars;
        menubars = this->findChildren<QMenuBar*> ();
        for (int i = 0; i < menubars.length(); i++) {
            menubars[i]->hide();
        }

        this->showFullScreen();
    }
}

void PhotoViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (this->ui->gvPicture->hasPicture()) {
        this->ui->gvPicture->resize ();
    }
}


void PhotoViewer::on_actionSet_1_star_hovered()
{
}

void PhotoViewer::on_actionSet_2_stars_hovered()
{
   this->ui->actionSet_1_star->setChecked(true);
}

void PhotoViewer::on_actionPlay_triggered()
{
    if (_playerTimer->isActive()) {
        _playerTimer->stop();
    }
    else {
        _playerTimer->start(5000);
    }
}

void PhotoViewer::on_playerTimerTimeout ()
{
    if (_currentFile < _currentDir->entryList().count() - 1) {
        _currentFile++;
        showCurrentPicture(PictureView::PictureAnimationType::RightToLeft);
    }
    else {
        QMessageBox::information(this,
                                 tr("Image Viewer"),
                                 tr("End of slide show!"));
        _playerTimer->stop();
    }
}

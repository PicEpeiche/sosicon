#ifndef DLGMAIN_H
#define DLGMAIN_H

#include <QDialog>
#include <QMutex>
#include <memory>
#include "../../src/log_event.h"

namespace Ui {
    class DlgMain;
}

class DlgMain :
        public QDialog
{
    Q_OBJECT

    QMutex mMutex;
    QString mShapeFileTitle;
    QString mShapeFilePath;

public:
    explicit DlgMain( QWidget *parent = 0 );
    ~DlgMain();

private slots:
    void onRunSosicon();
    void onQuitSosicon();
    void onAddSosiFile();
    void onConversionChanged( int tabIndex );
    void onFileSelect();
    void onFileRemove();
    void onFileClear();
    void onLogMessage( QString msg, bool update );
    void onShapefileBrowse();
    void onCreateSubdirChanged();
    void onUseFileTitleChanged();
    void onFileTitleChanged();

private:
    void updateUi();
    void updateCommandLine();
    void updateAll();
    Ui::DlgMain *mUi;

signals:
    void startConversion( QString cmdStr, QMutex *mutex );
};

#endif // DLGMAIN_H

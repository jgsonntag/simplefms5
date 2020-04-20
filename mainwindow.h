#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QMenu>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QInputDialog>

#include "genericclient.h"
#include "setupgui.h"
#include "constants.h"
#include "utility.h"


struct wp
{
    QString name;
    double lat;
    double lon;
    double gs;
    double manmin;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QMenu *settingsMenu;
    QMenu *helpMenu;
    QAction *exitAct;
    QAction *openAct;
    QAction *closeAct;
    QAction *hdgtolAct;
    QAction *xtdtolAct;
    QAction *aboutAct;
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);
    void ingestWaypoints();
    GenericClient *client;
    QString host;
    quint16 port;
    QString wpfilename;
    QFile *wpfile;
    QString stemp;
    QString stemp2;
    QList<wp> waypoints;
    wp newwaypoint;
    QGroupBox *netbox;
    QGroupBox *fpbox;
    QGroupBox *timebox;
    QVBoxLayout *fplayout;
    QGroupBox *wpmodebox;
    QGroupBox *nextwpbox;
    QGroupBox *lastwpbox;
    QLabel *netlabel;
    QLabel *timelabel;
    QLabel *wpmodelabel;
    QLabel *autolabel;
    QTimer *timer;
    QPushButton *autowpbutton;
    QPushButton *prevwpbutton;
    QPushButton *nextwpbutton;
    bool autowp;
    QTableWidget *fptable;
    void configFPTable();
    QTableWidgetItem *newitem;
    QList<QTableWidgetItem> names;
    QList<QTableWidgetItem> disttos;
    QList<QTableWidgetItem> timetos;
    QList<QTableWidgetItem> etas;
    int i;
    int k;
    int wpindex;
    void updateFPTable();
    void updateNext();
    void updateLast();
    void updateAutoLabel();
    void updateTime();
    double hms;
    int hour;
    int min;
    double timesecs;
    double teta;
    double eta2next;
    double sec;
    double lat;
    double lon;
    double speed;
    double course;
    double dist2next;
    double sec2next;
    double alldist;
    double allsec;
    double dist;
    QLabel *nextwpnamelabel;
    QLabel *lastwpnamelabel;
    QLabel *nextwpdisttolabel;
    QLabel *lastwpdisttolabel;
    QLabel *nextwptimetolabel;
    QLabel *lastwptimetolabel;
    QLabel *nextwpetalabel;
    QLabel *lastwpetalabel;
    double hdgtol;
    double xtdtol;
    int findsegment();
    bool firstoff;
    int autostat;

private slots:
    void slotStatus();
    void slotNewData(QString);
    void slotAbout();
    void slotOpenWPFile(QString);
    void slotCloseWPFile();
    void slotGetWPFileName();
    void slotSetAutoWP();
    void slotPrevWP();
    void slotNextWP();
    void slotGetHdgTol();
    void slotGetXtdTol();

};

#endif // MAINWINDOW_H

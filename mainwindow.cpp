#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up the styles
    qApp->setStyleSheet("QGroupBox { background: white }");

    // Retrieve the saved settings
    readSettings();

    // Get starting info from user
    setupGui *setup = new setupGui(&host,&port);
    while (setup->exec() == QDialog::Rejected)
    {
        QMessageBox msgBox;
        msgBox.setText("Invalid input");
        msgBox.exec();
    }

    // Build the network client
    client = new GenericClient(this,host,port);
    connect(client,SIGNAL(newLine(QString)),this,SLOT(slotNewData(QString)));

    // Periodically check the connection to the server
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotStatus()));
    timer->start(500);

    // Build the menu bar
    createActions();
    createMenus();

    // Window management
    setWindowTitle("SimpleFMS5");
    setMinimumSize(160,160);
    resize(520,750);
    QWidget *window = new QWidget(this);

    // Build the network status area
    netbox = new QGroupBox("GPS status");
    QHBoxLayout *netlayout = new QHBoxLayout();
    netlabel = new QLabel();
    netlabel->setFont(QFont("Helvetica",12,QFont::Bold));
    //netlabel->setAlignment(Qt::AlignCenter);
    netlayout->addWidget(netlabel);
    netbox->setLayout(netlayout);

    // Build the waypoint mode area
    wpmodebox = new QGroupBox("Waypoint mode");
    QVBoxLayout *wpmodelayout = new QVBoxLayout();
    wpmodelabel = new QLabel();
    wpmodelabel->setFont(QFont("Helvetica",12,QFont::Bold));
    autolabel = new QLabel();
    autolabel->setFont(QFont("Helvetica",8));
    wpmodelayout->addWidget(wpmodelabel);
    wpmodelayout->addWidget(autolabel);
    wpmodebox->setLayout(wpmodelayout);
    if (autowp)
        slotSetAutoWP();
    else
    {
        wpmodelabel->setText("MANUAL");
        autolabel->setText("");
    }

    // Build the time area
    timebox = new QGroupBox("Time (UTC)");
    QHBoxLayout *timelayout = new QHBoxLayout();
    timelabel = new QLabel();
    timelabel->setFont(QFont("Helvetica",12,QFont::Bold));
    //netlabel->setAlignment(Qt::AlignCenter);
    timelayout->addWidget(timelabel);
    timebox->setLayout(timelayout);

    // Build the next waypoint area
    nextwpbox = new QGroupBox("Next waypoint");
    QVBoxLayout *nextwplayout = new QVBoxLayout();
    QLabel *nextwpname = new QLabel("Name:");
    nextwpnamelabel = new QLabel();
    nextwpnamelabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *nextwpdistto = new QLabel("Dist to (nm):");
    nextwpdisttolabel = new QLabel();
    nextwpdisttolabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *nextwptimeto = new QLabel("Time to (hh:mm:ss)");
    nextwptimetolabel = new QLabel();
    nextwptimetolabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *nextwpeta = new QLabel("ETA (hh:mm:ss)");
    nextwpetalabel = new QLabel();
    nextwpetalabel->setFont(QFont("Helvetica",12,QFont::Bold));
    nextwplayout->addWidget(nextwpname);
    nextwplayout->addWidget(nextwpnamelabel);
    nextwplayout->addWidget(nextwpdistto);
    nextwplayout->addWidget(nextwpdisttolabel);
    nextwplayout->addWidget(nextwptimeto);
    nextwplayout->addWidget(nextwptimetolabel);
    nextwplayout->addWidget(nextwpeta);
    nextwplayout->addWidget(nextwpetalabel);
    nextwpbox->setLayout(nextwplayout);

    // Build the last waypoint area
    lastwpbox = new QGroupBox("Last waypoint");
    QVBoxLayout *lastwplayout = new QVBoxLayout();
    QLabel *lastwpname = new QLabel("Name:");
    lastwpnamelabel = new QLabel();
    lastwpnamelabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *lastwpdistto = new QLabel("Dist to (nm):");
    lastwpdisttolabel = new QLabel();
    lastwpdisttolabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *lastwptimeto = new QLabel("Time to (hh:mm:ss):");
    lastwptimetolabel = new QLabel();
    lastwptimetolabel->setFont(QFont("Helvetica",12,QFont::Bold));
    QLabel *lastwpeta = new QLabel("ETA (hh:mm:ss)");
    lastwpetalabel = new QLabel();
    lastwpetalabel->setFont(QFont("Helvetica",12,QFont::Bold));
    lastwplayout->addWidget(lastwpname);
    lastwplayout->addWidget(lastwpnamelabel);
    lastwplayout->addWidget(lastwpdistto);
    lastwplayout->addWidget(lastwpdisttolabel);
    lastwplayout->addWidget(lastwptimeto);
    lastwplayout->addWidget(lastwptimetolabel);
    lastwplayout->addWidget(lastwpeta);
    lastwplayout->addWidget(lastwpetalabel);
    lastwpbox->setLayout(lastwplayout);

    // Build the flight plan area
    fpbox = new QGroupBox("Flight plan");
    fplayout = new QVBoxLayout();

    // Build the waypoint control buttons
    autowpbutton = new QPushButton("Auto WP");
    connect(autowpbutton,SIGNAL(clicked()),this,SLOT(slotSetAutoWP()));
    prevwpbutton = new QPushButton("Prev WP");
    connect(prevwpbutton,SIGNAL(clicked()),this,SLOT(slotPrevWP()));
    nextwpbutton = new QPushButton("Next WP");
    connect(nextwpbutton,SIGNAL(clicked()),this,SLOT(slotNextWP()));

    // Build the flight plan table
    fptable = new QTableWidget(0,4);
    QHeaderView *vheader = fptable->verticalHeader();
    vheader->hide();
    QTableWidgetItem *fpheader0 = new QTableWidgetItem("Name");
    fpheader0->setFont(QFont("Helvetica",8,QFont::Bold));
    fptable->setHorizontalHeaderItem(0,fpheader0);
    QTableWidgetItem *fpheader1 = new QTableWidgetItem("Dist to");
    fpheader1->setFont(QFont("Helvetica",8,QFont::Bold));
    fptable->setHorizontalHeaderItem(1,fpheader1);
    QTableWidgetItem *fpheader2 = new QTableWidgetItem("Time to");
    fpheader2->setFont(QFont("Helvetica",8,QFont::Bold));
    fptable->setHorizontalHeaderItem(2,fpheader2);
    QTableWidgetItem *fpheader3 = new QTableWidgetItem("ETA");
    fpheader3->setFont(QFont("Helvetica",8,QFont::Bold));
    fptable->setHorizontalHeaderItem(3,fpheader3);
    fptable->setColumnWidth(0,60);
    fptable->setColumnWidth(1,60);
    fptable->setColumnWidth(2,80);
    fptable->setColumnWidth(3,80);
    fplayout->addWidget(fptable);
    fpbox->setLayout(fplayout);
    newitem = new QTableWidgetItem();
    newitem->setFont(QFont("Helvetica",8));

    // Finalize the overall GUI
    QHBoxLayout *pnblayout = new QHBoxLayout;
    pnblayout->addWidget(prevwpbutton);
    pnblayout->addWidget(nextwpbutton);
    QVBoxLayout *leftlayout = new QVBoxLayout;
    leftlayout->addWidget(netbox);
    leftlayout->addWidget(wpmodebox);
    leftlayout->addWidget(timebox);
    leftlayout->addWidget(nextwpbox);
    leftlayout->addWidget(lastwpbox);
    leftlayout->addLayout(pnblayout);
    leftlayout->addWidget(autowpbutton);
    QHBoxLayout *mainlayout = new QHBoxLayout;
    mainlayout->addLayout(leftlayout);
    mainlayout->addWidget(fpbox);
    window->setLayout(mainlayout);
    setCentralWidget(window);

    // Open the previously saved waypoint file
    slotOpenWPFile(wpfilename);

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
}


void MainWindow::configFPTable()
{

    // Calculate number of table rows (#wps+#maneuvers)
    int nrows = waypoints.size();
    for (i=0;i<waypoints.size();i++)
    {
        if(waypoints[i].manmin>0)
            ++nrows;
    }

    // Set up the table members, with waypoint names
    fptable->setRowCount(nrows);
    k = 0;
    newitem->setTextAlignment(Qt::AlignCenter);
    for (i=0;i<waypoints.size();i++)
    {
        newitem->setText(waypoints[i].name);
        names.append(*newitem);
        fptable->setItem(k,0,&names[k]);
        newitem->setText("");
        disttos.append(*newitem);
        timetos.append(*newitem);
        etas.append(*newitem);
        fptable->setItem(k,1,&disttos[k]);
        fptable->setItem(k,2,&timetos[k]);
        fptable->setItem(k,3,&etas[k]);
        ++k;
        if (waypoints[i].manmin>0)
        {
            newitem->setText("*manvr*");
            names.append(*newitem);
            fptable->setItem(k,0,&names[k]);
            newitem->setText("");
            disttos.append(*newitem);
            timetos.append(*newitem);
            etas.append(*newitem);
            fptable->setItem(k,1,&disttos[k]);
            fptable->setItem(k,2,&timetos[k]);
            fptable->setItem(k,3,&etas[k]);
            ++k;
        }
    }

}



void MainWindow::writeSettings()
{

    QSettings settings("NASA","SimpleFMS5");
    settings.setValue("host",host);
    settings.setValue("port",port);
    settings.setValue("waypoints",wpfilename);
    settings.setValue("wpmode",autowp);
    settings.setValue("wpindex",wpindex);
    settings.setValue("htolerance",hdgtol);
    settings.setValue("xtdtolerance",xtdtol);

}


void MainWindow::readSettings()
{

    QSettings settings("NASA","SimpleFMS5");
    host = settings.value("host","localhost").toString();
    port = settings.value("port",4062).toInt();
    wpfilename = settings.value("waypoints","").toString();
    autowp = settings.value("wpmode",false).toBool();
    wpindex = settings.value("wpindex",0).toInt();
    hdgtol = settings.value("htolerance",20).toDouble();
    xtdtol = settings.value("xtdtolerance",10).toDouble();

}


void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(openAct);
    fileMenu->addAction(exitAct);

    settingsMenu = menuBar()->addMenu("Settings");
    settingsMenu->addAction(hdgtolAct);
    settingsMenu->addAction(xtdtolAct);

    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}


void MainWindow::createActions()
{

    // File menu
    openAct = new QAction("Open waypoint file",this);
    connect(openAct,SIGNAL(triggered()),this,SLOT(slotGetWPFileName()));
    //closeAct = new QAction("Close waypoint file",this);
    //connect(closeAct,SIGNAL(triggered()),this,SLOT(slotCloseWPFile()));
    exitAct = new QAction("Exit",this);
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    // Settings menu
    hdgtolAct = new QAction("Heading tolerance",this);
    connect(hdgtolAct,SIGNAL(triggered()),this,SLOT(slotGetHdgTol()));
    xtdtolAct = new QAction("Cross-track distance tolerance",this);
    connect(xtdtolAct,SIGNAL(triggered()),this,SLOT(slotGetXtdTol()));

    // Help menu
    aboutAct = new QAction("About",this);
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(slotAbout()));

}


void MainWindow::slotGetWPFileName()
{

    // Prompt the user for the desired waypoint filename
    wpfilename = QFileDialog::getOpenFileName(this,"Select a waypoint file",
                                              ".",
                                              "Sequence files (*.sequence)");
    slotOpenWPFile(wpfilename);

}


void MainWindow::slotOpenWPFile(QString wpfilename)
{

    // Ingest the waypoint file if it exists
    wpfile = new QFile(wpfilename);
    if (wpfile->exists())
    {
        stemp = "Close waypoint file";
        stemp.append(wpfilename);
        closeAct = new QAction(stemp,this);
        connect(closeAct,SIGNAL(triggered()),this,SLOT(slotCloseWPFile()));
        fileMenu->removeAction(openAct);
        fileMenu->insertAction(exitAct,closeAct);
        ingestWaypoints();
        configFPTable();
    }

    // Selected file does not exist
    else if (wpfilename!="")
    {
        stemp = "Waypoint file ";
        stemp.append(wpfilename);
        stemp.append(" does not exist");
        QMessageBox msgBox;
        msgBox.setText(stemp);
        msgBox.exec();
        wpfilename = "";
        waypoints.clear();
    }

    // Null filename
    else
        waypoints.clear();

}


void MainWindow::ingestWaypoints()
{

    wpfile->open(QIODevice::ReadOnly);
    QTextStream instream(wpfile);
    while (!instream.atEnd())
    {
        stemp = instream.readLine();
        newwaypoint.name = stemp.section(' ',0,0,QString::SectionSkipEmpty);
        stemp2 = stemp.section(' ',1,1,QString::SectionSkipEmpty);
        newwaypoint.lat = (stemp2.toDouble())*DEG2RAD;
        stemp2 = stemp.section(' ',2,2,QString::SectionSkipEmpty);
        newwaypoint.lon = (stemp2.toDouble())*DEG2RAD;
        stemp2 = stemp.section(' ',3,3,QString::SectionSkipEmpty);
        newwaypoint.gs = stemp2.toDouble();
        stemp2 = stemp.section(' ',4,4,QString::SectionSkipEmpty);
        newwaypoint.manmin = stemp2.toDouble();
        waypoints.append(newwaypoint);
    }

}


void MainWindow::slotCloseWPFile()
{

    fileMenu->removeAction(closeAct);
    fileMenu->insertAction(exitAct,openAct);
    wpfilename = "";
    waypoints.clear();
    names.clear();
    disttos.clear();
    timetos.clear();
    etas.clear();
    wpindex = 0;
    fptable->clearContents();
    configFPTable();

}


void MainWindow::slotAbout()
{
    QMessageBox msgBox;
    msgBox.setText("SimpleFMS5\n"
                   "Qt5-Based Flight Management System\n"
                   "Author: John G. Sonntag\n"
                   "Released: 10 April 2018");
    msgBox.exec();
}


void MainWindow::slotNewData(QString newgps)
{

    // Parse the network message
    stemp = newgps.section(',',0,0);
    if (stemp=="11")
    {
      stemp = newgps.section(',',2,2);
      hms = stemp.toDouble();
      parsehms(hms,&hour,&min,&sec);
      timesecs = double(hour)*3600.0+double(min)*60.0+sec;
      stemp = newgps.section(',',3,3);
      lat = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',4,4);
      lon = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',6,6);
      course = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',7,7);
      speed = stemp.toDouble();
    }
    else if (stemp=="10")
    {
      stemp = newgps.section(',',1,1);
      timesecs = stemp.toDouble();
      stemp = newgps.section(',',2,2);
      lat = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',3,3);
      lon = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',5,5);
      course = stemp.toDouble()*DEG2RAD;
      stemp = newgps.section(',',6,6);
      speed = stemp.toDouble();
    }
    else
    {
      qDebug() << "Unrecognized network message type\n";
    }

    // Find the segment we are flying, if in auto waypoint mode
    if (autowp && !waypoints.isEmpty())
    {
        k = findsegment();
        if (k==-999)
        {
            if (firstoff&&wpindex<(waypoints.count()-1))
            {
                ++wpindex;
                firstoff = false;
            }
        }
        else
        {
            firstoff = true;
            wpindex = k;
        }
    }

    // Update the various displays
    updateTime();
    updateNext();
    if (!waypoints.isEmpty())
      updateFPTable();
    updateLast();
    if (autowp)
        updateAutoLabel();

}



int MainWindow::findsegment()
{
    int i,indx_sm,indx_crs_sm;
    double distAD,distBD,distAB,crsAD,crsAB;
    double xtd_sm,xtd_crs_sm,xtd,dhdg;

    // Loop over waypoint list
    xtd_sm = 999.9;
    xtd_crs_sm = 999.9;
    for (i=0; i<(waypoints.count()-1); i++)
    {
      distAD = gcdist(waypoints[i].lat,waypoints[i].lon,lat,lon);
      distBD = gcdist(waypoints[i+1].lat,waypoints[i+1].lon,lat,lon);
      distAB = gcdist(waypoints[i].lat,waypoints[i].lon,
                      waypoints[i+1].lat,waypoints[i+1].lon);
      crsAD  = gccourse1(waypoints[i].lat,waypoints[i].lon,lat,lon);
      crsAB  = gccourse1(waypoints[i].lat,waypoints[i].lon,
                         waypoints[i+1].lat,waypoints[i+1].lon);
      if (distAD > distAB || 180.0/PI*fabs(crsAB-crsAD)>hdgtol)
        xtd = 999.9;
      else
        xtd = RAD2KM*fabs(asin(sin(distAD)*sin(crsAD-crsAB)));
      crsAB *= 180.0/PI;
      dhdg = fabs(course*180.0/PI - crsAB);
      if ( xtd <= xtd_sm )
      {
        indx_sm = i;
        xtd_sm = xtd;
      }
      if ( xtd <= xtd_crs_sm && dhdg <= hdgtol )
      {
        indx_crs_sm = i;
        xtd_crs_sm = xtd;
      }
    }

    //  Select xtd to return
    if ( xtd_crs_sm < xtdtol )
    {
        autostat = 0;
        return(indx_crs_sm+1);
    }
    else if (xtd_sm < xtdtol)
    {
        autostat = 1;
        return(indx_sm+1);
    }
    else
    {
        autostat = 2;
        return(-999);
    }

}


void MainWindow::updateTime()
{
    double t1 = timesecs;
    while (t1>86400.0) t1 -= 86400.0;
    int hr1 = int(t1/3600.0);
    int min1 = int(t1-hr1*3600.0)/60.0;
    double sec1 = (t1-hr1*3600.0-min1*60.0);
    //stemp.sprintf("%lf",timesecs);
    stemp.sprintf("%02d:%02d:%04.1lf",hr1,min1,sec1);
    timelabel->setText(stemp);
    //qDebug() << hour << min << sec << "\n";
}



void MainWindow::updateAutoLabel()
{
    if (autostat==0)
        autolabel->setText("Locked on hdg and xtd");
    else if (autostat==1)
        autolabel->setText("Locked on xtd only");
    else
        autolabel->setText("Off course, assuming next");

}


void MainWindow::updateNext()
{

    // Blank the fields if no flight plan loaded
    if (waypoints.empty())
    {
        nextwpnamelabel->setText("----");
        nextwpdisttolabel->setText("----");
        stemp.sprintf("--:--:--");
        nextwptimetolabel->setText(stemp);
        nextwpetalabel->setText(stemp);
    }

    // Otherwise update normally
    else
    {

        // Update waypoint name
        nextwpnamelabel->setText(waypoints[wpindex].name);

        // Update distance to next
        dist2next = RAD2NM*gcdist(waypoints[wpindex].lat,waypoints[wpindex].lon,
                                  lat,lon);
        stemp.sprintf("%.1lf",dist2next);
        nextwpdisttolabel->setText(stemp);

        // Update time to next
        if (speed>0.0)
        {
          sec2next = 3600.0*dist2next/speed;
          eta2next  = timesecs + sec2next;
          sec2hms(sec2next,&hour,&min,&sec);
          stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
          nextwptimetolabel->setText(stemp);
        }
        else
        {
          stemp.sprintf("--:--:--");
          nextwptimetolabel->setText(stemp);
        }

        // Update ETA at next
        if (speed>0.0)
        {
          sec2hms(eta2next,&hour,&min,&sec);
          stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
          nextwpetalabel->setText(stemp);
        }
        else
        {
          stemp.sprintf("--:--:--");
          nextwpetalabel->setText(stemp);
        }

    }

}



void MainWindow::updateLast()
{

    // Blank the fields if no flight plan loaded
    if (waypoints.empty())
    {
        lastwpnamelabel->setText("----");
        lastwpdisttolabel->setText("----");
        stemp.sprintf("--:--:--");
        lastwptimetolabel->setText(stemp);
        lastwpetalabel->setText(stemp);
    }

    // Otherwise update normally
    else
    {

        //qDebug() << teta << "  " << allsec << "\n";

        // Update waypoint name
        lastwpnamelabel->setText(waypoints[i-1].name);

        // Update distance to next
        stemp.sprintf("%.1lf",alldist);
        lastwpdisttolabel->setText(stemp);

        // Update time to next
        if (speed>0.0)
        {
          sec2hms(allsec,&hour,&min,&sec);
          stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
          lastwptimetolabel->setText(stemp);
        }
        else
        {
          stemp.sprintf("--:--:--");
          lastwptimetolabel->setText(stemp);
        }

        // Update ETA at next
        if (speed>0.0)
        {
          sec2hms(teta,&hour,&min,&sec);
          stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
          lastwpetalabel->setText(stemp);
        }
        else
        {
          stemp.sprintf("--:--:--");
          lastwpetalabel->setText(stemp);
        }

    }

}



void MainWindow::updateFPTable()
{

    // Update everything prior to the current waypoint
    k = 0;
    for (i=0;i<wpindex;i++)
    {
        disttos[k].setText("-");
        timetos[k].setText("-");
        etas[k].setText("-");
        ++k;
        if (waypoints[i].manmin>0.0)
        {
            disttos[k].setText("-");
            timetos[k].setText("-");
            etas[k].setText("-");
            ++k;
        }
    }

    // Next waypoint
    stemp.sprintf("%.1lf",dist2next);
    disttos[k].setText(stemp);
    sec2hms(sec2next,&hour,&min,&sec);
    stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
    timetos[k].setText(stemp);
    sec2hms(eta2next,&hour,&min,&sec);
    stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
    etas[k].setText(stemp);
    ++k;

    // Maneuver immediately following next waypoint
    alldist = dist2next;
    allsec = sec2next;
    teta = timesecs + allsec;
    if (waypoints[i].manmin>0.0)
    {
      allsec += waypoints[i].manmin*60.0;
      alldist += (waypoints[i].manmin/60.0)*waypoints[i].gs;
      stemp.sprintf("%.1lf",alldist);
      disttos[k].setText(stemp);
      sec2hms(allsec,&hour,&min,&sec);
      stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
      timetos[k].setText(stemp);
      teta = timesecs + allsec;
      sec2hms(teta,&hour,&min,&sec);
      stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
      etas[k].setText(stemp);
      ++k;
    }

    // Waypoints after next
    for (i=wpindex+1;i<waypoints.size();i++)
    {
      dist = RAD2NM*gcdist(waypoints[i-1].lat,waypoints[i-1].lon,
             waypoints[i].lat,waypoints[i].lon);
      alldist += dist;
      stemp.sprintf("%.1lf",alldist);
      disttos[k].setText(stemp);
      allsec += 3600.0*dist/waypoints[i-1].gs;
      sec2hms(allsec,&hour,&min,&sec);
      stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
      timetos[k].setText(stemp);
      teta = timesecs + allsec;
      sec2hms(teta,&hour,&min,&sec);
      stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
      etas[k].setText(stemp);
      ++k;
      if (waypoints[i].manmin>0.0)
      {
        allsec += waypoints[i].manmin*60.0;
        alldist += waypoints[i].manmin/60.0*waypoints[i].gs;
        stemp.sprintf("%.1lf",alldist);
        disttos[k].setText(stemp);
        sec2hms(allsec,&hour,&min,&sec);
        stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
        timetos[k].setText(stemp);
        teta = timesecs + allsec;
        sec2hms(teta,&hour,&min,&sec);
        stemp.sprintf("%02d:%02d:%02d",hour,min,(int)(sec));
        etas[k].setText(stemp);
        ++k;
      }
    }

    // Force a repaint of the table; not sure why this is necessary but
    // the table doesnt update properly without it
    fptable->viewport()->update();

}



void MainWindow::slotStatus()
{

    if (client->slotConnectStatus())
    {
        netlabel->setText("Connected to server");
        netbox->setStyleSheet("background-color:white;");
    }
    else
    {
        netlabel->setText("Disconnected from server");
        netbox->setStyleSheet("background-color:red;");
    }
}


void MainWindow::slotSetAutoWP()
{

    wpmodelabel->setText("AUTOMATIC");
    //autolabel->setText("");
    autowp = true;

}


void MainWindow::slotPrevWP()
{

    wpmodelabel->setText("MANUAL");
    autolabel->setText("");
    autowp = false;
    if (wpindex!=0)
        --wpindex;

}


void MainWindow::slotNextWP()
{

    wpmodelabel->setText("MANUAL");
    autolabel->setText("");
    autowp = false;
    if (wpindex<(waypoints.count()-1))
        ++wpindex;

}



void MainWindow::slotGetHdgTol()
{

    bool ok;
    double dtemp = QInputDialog::getDouble(this,"Heading tolerance",
                                           "Enter heading tolerance (deg)",
                                           hdgtol,0.0,360.0,1,&ok);
    if (ok) hdgtol = dtemp;    qDebug() << "new hdgtol " << hdgtol;

}



void MainWindow::slotGetXtdTol()
{

    bool ok;
    double dtemp = QInputDialog::getDouble(this,"XTD tolerance",
                                           "Enter heading tolerance (km)",
                                           xtdtol,0.0,1000.0,1,&ok);
    if (ok) xtdtol = dtemp;

}

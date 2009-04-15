
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2008, All Rights Reserved
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./CellViewer.cpp"                         |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#include  <algorithm>

#include  <QAction>
#include  <QMenu>
#include  <QMenuBar>
#include  <QStatusBar>
#include  <QDockWidget>
#include  <QApplication>
#include  <QPrinter>
#include  <QPrintDialog>

#include  "hurricane/DataBase.h"
#include  "hurricane/Cell.h"

//#include  "MapView.h"
#include  "hurricane/viewer/Graphics.h"
#include  "hurricane/viewer/CellViewer.h"
#include  "hurricane/viewer/MousePositionWidget.h"
#include  "hurricane/viewer/ControllerWidget.h"


namespace Hurricane {


  CellViewer::CellViewer ( QWidget* parent ) : QMainWindow(parent)
                                             , _applicationName(tr("Viewer"))
                                             , _openAction(NULL)
                                             , _nextAction(NULL)
                                             , _printAction(NULL)
                                             , _saveAction(NULL)
                                             , _closeAction(NULL)
                                             , _exitAction(NULL)
                                             , _refreshAction(NULL)
                                             , _fitToContentsAction(NULL)
                                             , _showSelectionAction(NULL)
                                             , _rubberChangeAction(NULL)
                                             , _clearRulersAction(NULL)
                                             , _controllerAction(NULL)
                                             , _fileMenu(NULL)
                                             , _viewMenu(NULL)
                                             , _toolsMenu(NULL)
                                             , _debugMenu(NULL)
                                             //, _mapView(NULL)
                                             , _palette(NULL)
                                             , _mousePosition(NULL)
                                             , _controller(NULL)
                                             , _cellWidget(NULL)
                                             , _moveCommand()
                                             , _zoomCommand()
                                             , _rulerCommand()
                                             , _selectCommand()
                                             , _hierarchyCommand()
                                             , _cellHistory()
                                             , _firstShow(false)
                                             , _updateState(ExternalEmit)
  {
    setObjectName("viewer");

    createMenus  ();
    createLayout ();
  }


  CellViewer::~CellViewer ()
  {
    _controller->deleteLater ();
  }




  void  CellViewer::createActions ()
  {
    if ( _openAction ) return;

    _openAction = new QAction  ( tr("&Open Cell"), this );
    _openAction->setObjectName ( "viewer.menuBar.file.openCell" );
    _openAction->setShortcut   ( QKeySequence(tr("CTRL+O")) );
    _openAction->setIcon       ( QIcon(":/images/stock_open.png") );
    _openAction->setStatusTip  ( tr("Open (load) a new Cell") );

    _nextAction = new QAction  ( tr("&Next Breakpoint"), this );
    _nextAction->setObjectName ( "viewer.menuBar.file.nextBreakpoint" );
    _nextAction->setStatusTip  ( tr("Proceed to the next breakpoint") );

    for ( int i=0 ; i<CellHistorySize ; i++ ) {
      _cellHistoryAction[i] = new QAction ( this );
      _cellHistoryAction[i]->setObjectName ( QString("viewer.menuBar.file.cellHistory[%1]").arg(i) );
      _cellHistoryAction[i]->setVisible ( false );
      _cellHistoryAction[i]->setData    ( i );
      _cellHistoryAction[i]->setFont    ( Graphics::getFixedFont(QFont::Bold,false,false) );
      connect ( _cellHistoryAction[i], SIGNAL(triggered()), this, SLOT(openHistoryCell()));
    }

    _printAction = new QAction  ( tr("&Print"), this );
    _printAction->setObjectName ( "viewer.menuBar.file.print" );
    _printAction->setStatusTip  ( tr("Print the displayed area") );
    _printAction->setShortcut   ( QKeySequence(tr("CTRL+P")) );
    _printAction->setVisible    ( true );
    connect ( _printAction, SIGNAL(triggered()), this, SLOT(printDisplay()) );

    _saveAction = new QAction  ( tr("&Save Cell"), this );
    _saveAction->setObjectName ( "viewer.menuBar.file.saveCell" );
    _saveAction->setIcon       ( QIcon(":/images/stock_save.png") );
    _saveAction->setStatusTip  ( tr("Save the current Cell") );
    _saveAction->setVisible    ( false );

    _closeAction = new QAction  ( tr("&Close"), this );
    _closeAction->setObjectName ( "viewer.menuBar.file.close" );
    _closeAction->setStatusTip  ( tr("Close This Coriolis CellViewer") );
    _closeAction->setShortcut   ( QKeySequence(tr("CTRL+W")) );
    connect ( _closeAction, SIGNAL(triggered()), this, SLOT(close()) );

    _exitAction = new QAction  ( tr("&Exit"), this );
    _exitAction->setObjectName ( "viewer.menuBar.file.exit" );
    _exitAction->setStatusTip  ( tr("Exit All Coriolis CellViewer") );
    _exitAction->setShortcut   ( QKeySequence(tr("CTRL+Q")) );
    connect ( _exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()) );

    _refreshAction = new QAction  ( tr("&Refresh"), this );
    _refreshAction->setObjectName ( "viewer.menuBar.view.refresh" );
    _refreshAction->setStatusTip  ( tr("Force full redrawing of the display") );
    _refreshAction->setShortcut   ( QKeySequence(tr("CTRL+L")) );

    _fitToContentsAction = new QAction  ( tr("&Fit to Contents"), this );
    _fitToContentsAction->setObjectName ( "viewer.menuBar.view.fit" );
    _fitToContentsAction->setStatusTip  ( tr("Adjust zoom to fit the whole cell's contents") );
    _fitToContentsAction->setShortcut   ( Qt::Key_F );

    _showSelectionAction = new QAction  ( tr("&Show Selection"), this );
    _showSelectionAction->setObjectName ( "viewer.menuBar.view.showSelection" );
    _showSelectionAction->setStatusTip  ( tr("Highlight the selected items (darken others)") );
    _showSelectionAction->setShortcut   ( Qt::Key_S );
    _showSelectionAction->setCheckable  ( true );

    _rubberChangeAction = new QAction  ( tr("Change Rubber Style"), this );
    _rubberChangeAction->setObjectName ( "viewer.menuBar.view.changeRubber" );
    _rubberChangeAction->setStatusTip  ( tr("Cycle through all avalaibles rubber drawing styles") );
    _rubberChangeAction->setShortcut   ( Qt::Key_Asterisk );

    _clearRulersAction = new QAction  ( tr("Clear Rulers"), this );
    _clearRulersAction->setObjectName ( "viewer.menuBar.view.clearRulers" );
    _clearRulersAction->setStatusTip  ( tr("Remove all rulers") );
    _clearRulersAction->setShortcut   ( QKeySequence(tr("CTRL+R")) );

    _controllerAction = new QAction  ( tr("Controller"), this );
    _controllerAction->setObjectName ( "viewer.menuBar.tools.controller" );
    _controllerAction->setStatusTip  ( tr("Fine Tune && Inspect DataBase") );
    _controllerAction->setIcon       ( QIcon(":/images/swiss-knife.png") );
    _controllerAction->setShortcut   ( QKeySequence(tr("CTRL+I")) );
  }


  void  CellViewer::createMenus ()
  {
    if (  _fileMenu   ) return;
    if ( !_openAction ) createActions ();

    menuBar()->setObjectName ( tr("viewer.menuBar") );

    _fileMenu = menuBar()->addMenu ( tr("File") );
    _fileMenu->setObjectName ( "viewer.menuBar.file" );
    _fileMenu->addAction ( _openAction );
    _fileMenu->addAction ( _nextAction );
    _fileMenu->addSeparator ();
    for ( size_t i=0 ; i<CellHistorySize ; i++ ) {
      _fileMenu->addAction ( _cellHistoryAction[i] );
    }
    _fileMenu->addSeparator ();
    _fileMenu->addAction ( _printAction );
    _fileMenu->addAction ( _saveAction );
    _fileMenu->addSeparator ();
    _fileMenu->addAction ( _closeAction );
    _fileMenu->addAction ( _exitAction );

    _viewMenu = menuBar()->addMenu ( tr("View") );
    _viewMenu->setObjectName ( "viewer.menuBar.view" );
    _viewMenu->addAction ( _refreshAction );
    _viewMenu->addAction ( _fitToContentsAction );
    _viewMenu->addAction ( _showSelectionAction );
    _viewMenu->addAction ( _rubberChangeAction );
    _viewMenu->addAction ( _clearRulersAction );

    _toolsMenu = menuBar()->addMenu ( tr("Tools") );
    _toolsMenu->setObjectName ( "viewer.menuBar.tools" );
    _toolsMenu->addAction ( _controllerAction );
  }


  QMenu* CellViewer::createDebugMenu ()
  {
    if ( !_debugMenu ) {
      _debugMenu = menuBar()->addMenu ( tr("Debug") );
      _debugMenu->setObjectName ( "viewer.menuBar.debug" );
    }
    return _debugMenu;
  }


  void  CellViewer::createLayout ()
  {
    if ( _cellWidget ) return;

    _cellWidget       = new CellWidget       ();
    _controller       = new ControllerWidget ();
  //_mapView    = _cellWidget->getMapView ();

    _cellWidget->bindCommand ( &_moveCommand );
    _cellWidget->bindCommand ( &_zoomCommand );
    _cellWidget->bindCommand ( &_rulerCommand );
    _cellWidget->bindCommand ( &_selectCommand );
    _cellWidget->bindCommand ( &_hierarchyCommand );
    _controller->setCellWidget ( _cellWidget );

    MousePositionWidget* _mousePosition = new MousePositionWidget ();
    statusBar()->addPermanentWidget ( _mousePosition );

    setCorner ( Qt::TopLeftCorner    , Qt::LeftDockWidgetArea  );
    setCorner ( Qt::BottomLeftCorner , Qt::LeftDockWidgetArea  );
    setCorner ( Qt::TopRightCorner   , Qt::RightDockWidgetArea );
    setCorner ( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

  //   QDockWidget* mapViewDock = new QDockWidget ( tr("Map") );
  //   mapViewDock->setFeatures     ( QDockWidget::DockWidgetVerticalTitleBar
  //                                | QDockWidget::DockWidgetMovable
  //                                | QDockWidget::DockWidgetFloatable
  //                                );
  //   mapViewDock->setObjectName   ( "viewer.menuBar.dock.mapView" );
  //   mapViewDock->setWidget       ( _mapView );
  //   mapViewDock->setAllowedAreas ( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  //   addDockWidget ( Qt::RightDockWidgetArea, mapViewDock );

    setCentralWidget ( _cellWidget );

    connect ( this                   , SIGNAL(redrawCellWidget()) , _cellWidget, SLOT(refresh()) );
    connect ( _refreshAction         , SIGNAL(triggered())        , _cellWidget, SLOT(refresh()) );
    connect ( _fitToContentsAction   , SIGNAL(triggered())        , _cellWidget, SLOT(fitToContents()) );
    connect ( _showSelectionAction   , SIGNAL(toggled(bool))      , this       , SLOT(setShowSelection(bool)) );
    connect ( _rubberChangeAction    , SIGNAL(triggered())        , _cellWidget, SLOT(rubberChange()) );
    connect ( _clearRulersAction     , SIGNAL(triggered())        , _cellWidget, SLOT(clearRulers()) );
    connect ( _controllerAction      , SIGNAL(triggered())        , this       , SLOT(showController()) );

    connect ( _cellWidget            , SIGNAL(mousePositionChanged(const Point&))
            , _mousePosition         , SLOT(setPosition(const Point&)) );

    connect ( _cellWidget            , SIGNAL(selectionModeChanged())
            , this                   , SLOT  (changeSelectionMode ()) );
    connect ( &_selectCommand        , SIGNAL(selectionToggled (Occurrence))
            ,  _cellWidget           , SLOT  (toggleSelection  (Occurrence)) );

    connect ( _cellWidget            , SIGNAL(stateChanged(shared_ptr<CellWidget::State>&))
            , this                   , SLOT  (setState    (shared_ptr<CellWidget::State>&)) );
    connect ( this                   , SIGNAL(stateChanged(shared_ptr<CellWidget::State>&))
            , _cellWidget            , SLOT  (setState    (shared_ptr<CellWidget::State>&)) );

    _cellWidget->refresh ();
  }


  void  CellViewer::refreshTitle ()
  {
    QString  cellName = "None";
    if ( getCell() )
      cellName = getString(getCell()->getName()).c_str();

    QString  title = QString("%1:<%2>").arg(_applicationName).arg(cellName);
    setWindowTitle ( title );
  }


  void  CellViewer::refreshHistory ()
  {
    if ( !getCell() ) return;

    shared_ptr<CellWidget::State> activeState = _cellWidget->getState();
    _cellHistory.remove ( activeState );

    if ( _cellHistory.size() > CellHistorySize-1 )
      _cellHistory.pop_front ();
    _cellHistory.push_back ( activeState );

    list< shared_ptr<CellWidget::State> >::iterator istate = _cellHistory.begin();
    for ( size_t i=0 ; i<CellHistorySize ; i++ ) {
      if ( istate != _cellHistory.end() ) {
        QString entry = tr("&%1: %2").arg(i+1).arg( getString((*istate)->getName()).c_str() );
        _cellHistoryAction[i]->setText    ( entry );
        _cellHistoryAction[i]->setVisible ( true );
        istate++;
      } else {
        _cellHistoryAction[i]->setVisible ( false );
      }
    }
  }


  void  CellViewer::setState ( shared_ptr<CellWidget::State>& state )
  {
    static bool isEmitter = false;

    if ( sender() == this ) {
        isEmitter = true;
        emit stateChanged ( state );
    } else {
      if ( !isEmitter ) {
        blockSignals ( true );

        refreshTitle   ();
        refreshHistory ();
        
        blockSignals ( false );
      } else
        isEmitter = false;
    }
  }


  void  CellViewer::setCell ( Cell* cell )
  {
    Name cellName = (cell) ? cell->getName() : "empty";

    list< shared_ptr<CellWidget::State> >::iterator istate
      = find_if ( _cellHistory.begin(), _cellHistory.end(), CellWidget::FindStateName(cellName) );

    if ( istate != _cellHistory.end() ) {
      emit stateChanged ( *istate );
      return;
    }

    _cellWidget->setCell ( cell );
  }


  Cell* CellViewer::getCell ()
  { return getCellWidget()->getCell(); }


  Cell* CellViewer::getCellFromDb ( const char* name )
  {
    cerr << "[ERROR] virtual function CellViewer::getCellFromDb() has not been overloaded.\n"
         << "        (this will prevent \"Open Cell\" to work)"
         << endl;

    return NULL;
  }


  void  CellViewer::showController ()
  {
    _controller->show ();
  }


  void  CellViewer::changeSelectionMode ()
  {
    if ( _updateState != InternalEmit ) {
      _showSelectionAction->blockSignals ( true );
      _showSelectionAction->setChecked   ( _cellWidget->getState()->cumulativeSelection() );
      _showSelectionAction->blockSignals ( false );
    }
    _updateState = ExternalEmit;
  }


  void  CellViewer::setShowSelection ( bool state )
  {
    _updateState = InternalEmit;
    _cellWidget->setShowSelection ( state );
  }


  void  CellViewer::openHistoryCell ()
  {
    QAction* historyAction = qobject_cast<QAction*> ( sender() );
    if ( historyAction ) {
      list< shared_ptr<CellWidget::State> >::iterator  istate = _cellHistory.begin();
      size_t index = historyAction->data().toUInt();

      for ( ; index>0 ; index--, istate++ );

    //cerr << "History: " << (*istate)->getName() << endl;
      emit stateChanged ( *istate );
    }
  }


  void  CellViewer::select ( Occurrence& occurrence )
  { if ( _cellWidget ) _cellWidget->select ( occurrence ); }


  void  CellViewer::unselect ( Occurrence& occurrence )
  { if ( _cellWidget ) _cellWidget->unselect ( occurrence ); }


  void  CellViewer::unselectAll ()
  { if ( _cellWidget ) _cellWidget->unselectAll(); }


  void  CellViewer::printDisplay ()
  {
    if ( !_cellWidget ) return;
    if ( !_cellWidget->getCell() ) {
      cerr << Warning("Unable to print, no cell loaded yet.") << endl;
      return;
    }

    QPrinter printer ( QPrinter::ScreenResolution );
    printer.setOutputFileName ( "unicorn-snapshot.pdf" );

    QPrintDialog  dialog ( &printer );
    if ( dialog.exec() == QDialog::Accepted )
      _cellWidget->copyToPrinter ( &printer );
  }


} // End of Hurricane namespace.

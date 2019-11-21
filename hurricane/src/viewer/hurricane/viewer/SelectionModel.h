// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2018, All Rights Reserved
//
// +-----------------------------------------------------------------+ 
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./hurricane/viewer/SelectionModel.h"      |
// +-----------------------------------------------------------------+


#ifndef  HURRICANE_SELECTION_MODEL_H
#define  HURRICANE_SELECTION_MODEL_H

#include <vector>
#include <QFont>
#include <QApplication>
#include <QAbstractTableModel>
#include "hurricane/Commons.h"
#include "hurricane/Occurrence.h"
#include "hurricane/viewer/Graphics.h"
#include "hurricane/viewer/Selector.h"


namespace Hurricane {

  class CellWidget;


  class SelectionModel : public QAbstractTableModel {
      Q_OBJECT;

    public:
                                     SelectionModel  ( QObject* parent=NULL );
                                    ~SelectionModel  ();
             void                    setCellWidget   ( CellWidget* );
             Selector*               getSelector     ( const QModelIndex& index );
             void                    setSelection    ( const SelectorSet& selection );
             void                    setSelection    ( Selector* );
             void                    toggleSelection ( Selector* );
             Selector*               toggleSelection ( const QModelIndex& index );
             int                     rowCount        ( const QModelIndex& parent=QModelIndex() ) const;
             int                     columnCount     ( const QModelIndex& parent=QModelIndex() ) const;
             QVariant                data            ( const QModelIndex& index, int role=Qt::DisplayRole ) const;
             QVariant                headerData      ( int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const;
             const Selector*         getSelector     ( int row );
             bool                    isCumulative    () const;
    public slots:
             void                    unlink          ( Selector* );
             void                    clear           ();

    private:
             CellWidget*        _cellWidget;
             vector<Selector*>  _selection;
  };


} // Hurricane namespace.

#endif // HURRICANE_SELECTION_MODEL_H

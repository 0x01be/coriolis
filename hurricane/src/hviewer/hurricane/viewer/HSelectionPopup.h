
// -*- C++ -*-
//
// This file is part of the Coriolis Project.
// Copyright (C) Laboratoire LIP6 - Departement ASIM
// Universite Pierre et Marie Curie
//
// Main contributors :
//        Christophe Alexandre   <Christophe.Alexandre@lip6.fr>
//        Sophie Belloeil             <Sophie.Belloeil@lip6.fr>
//        Hugo Cl�ment                   <Hugo.Clement@lip6.fr>
//        Jean-Paul Chaput           <Jean-Paul.Chaput@lip6.fr>
//        Damien Dupuis                 <Damien.Dupuis@lip6.fr>
//        Christian Masson           <Christian.Masson@lip6.fr>
//        Marek Sroka                     <Marek.Sroka@lip6.fr>
// 
// The  Coriolis Project  is  free software;  you  can redistribute it
// and/or modify it under the  terms of the GNU General Public License
// as published by  the Free Software Foundation; either  version 2 of
// the License, or (at your option) any later version.
// 
// The  Coriolis Project is  distributed in  the hope that it  will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY  or FITNESS FOR  A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details.
// 
// You should have  received a copy of the  GNU General Public License
// along with the Coriolis Project; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// License-Tag
// Authors-Tag
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
// |  C++ Header  :       "./HSelectionPopup.h"                      |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#ifndef  __HURRICANE_SELECTION_POPUP_WIDGET_H__
#define  __HURRICANE_SELECTION_POPUP_WIDGET_H__


#include  <QWidget>
#include  <QTableView>

#include  "hurricane/Commons.h"
#include  "hurricane/Occurrence.h"


class QModelIndex;
class QTableView;
class QComboBox;
class QHeaderView;


namespace Hurricane {


  class HSelectionPopupModel;


  class HSelectionPopup : public QWidget {
      Q_OBJECT;

    public:
                                     HSelectionPopup    ( QWidget* parent=NULL );
              void                   updateLayout       ();
              void                   popup              ();
    signals:
              void                   occurrenceSelected ( Occurrence occurrence, bool fromPopup );
    public slots:
              void                   add                ( Occurrence occurrence, bool showChange=false );
              void                   clear              ();
              void                   forceRowHeight     ();
    protected:
      virtual void                   keyPressEvent      ( QKeyEvent * event );
      virtual void                   mouseMoveEvent     ( QMouseEvent* event );
      virtual void                   mouseReleaseEvent  ( QMouseEvent* event );

    private:
              HSelectionPopupModel*  _model;
              QTableView*            _view;
              int                    _rowHeight;
  };


} // End of Hurricane namespace.


#endif // __HURRICANE_SELECTION_POPUP_WIDGET_H__

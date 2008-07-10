
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
// |  C++ Header  :       "./DynamicLabel.h"                         |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#include  <QAction>
#include  <QMenu>
#include  <QMenuBar>


# ifndef  __DYNAMIC_LABEL_H__
#   define  __DYNAMIC_LABEL_H__

# include  <QString>
# include  <QWidget>

class QLabel;

# include  "hurricane/Commons.h"
# include  "hurricane/DbU.h"


namespace Hurricane {


  class DynamicLabel : public QWidget {
      Q_OBJECT;

    public:
    // Constructor.
                            DynamicLabel   ( QWidget* parent=NULL );
    // Methods.
              QString       getStaticText  () const;
              QString       getDynamicText () const;
              void          setStaticText  ( const QString& text );
              void          setDynamicText ( const QString& text );
              void          setDynamicText ( DbU::Unit u );

    protected:
    // Internal - Attributes.
              QLabel*       _staticLabel;
              QLabel*       _dynamicLabel;

    // Internal - Constructor.
                            DynamicLabel   ( const DynamicLabel& );
              DynamicLabel& operator=      ( const DynamicLabel& );
  };


} // End of Hurricane namespace.


# endif

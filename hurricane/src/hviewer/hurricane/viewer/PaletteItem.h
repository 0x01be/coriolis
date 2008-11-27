
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
// |  C++ Header  :       "./PaletteItem.h"                          |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#ifndef  __HURRICANE_PALETTE_ITEM__
#define  __HURRICANE_PALETTE_ITEM__

#include  <QPixmap>
#include  <QWidget>
#include  <QFont>

class QPaintEvent;
class QCheckBox;

#include  "hurricane/Commons.h"


namespace Hurricane {


  class Name;
  class PaletteItem;


// -------------------------------------------------------------------
// Class :  "Hurricane::DrawingStyleSample".


  class DrawingStyleSample : public QWidget {
      Q_OBJECT;

    public:
                   DrawingStyleSample ( PaletteItem* );
    protected:
      QPixmap      _sample;
      PaletteItem* _entry;
    protected:
      void         redraw             ();
      void         paintEvent         ( QPaintEvent* );
  };


// -------------------------------------------------------------------
// Class :  "Hurricane::PaletteItem".


  class PaletteItem : public QWidget {
      Q_OBJECT;

    public:
      virtual const Name&    getName     () const = 0;
      virtual bool           isChecked   () const = 0;
      virtual void           setChecked  ( bool state ) = 0;
    signals:
      virtual void           toggled     ();

    protected:
                             PaletteItem ();
                             PaletteItem ( const PaletteItem& );
              PaletteItem&   operator=   ( const PaletteItem& );
  };


} // End of Hurricane namespace.


# endif  // __HURRICANE_PALETTE_ITEM__

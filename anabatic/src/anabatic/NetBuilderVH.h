// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC 2017-2018, All Rights Reserved
//
// +-----------------------------------------------------------------+
// |                   C O R I O L I S                               |
// |         A n a b a t i c  -  Routing Toolbox                     |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :  "./anabatic/NetBuilderVH.h"                     |
// +-----------------------------------------------------------------+

#ifndef  ANABATIC_NET_BUILDER_VH_H
#define  ANABATIC_NET_BUILDER_VH_H

#include "anabatic/NetBuilder.h"


namespace Anabatic {


// -----------------------------------------------------------------
// Class  :  "NetBuilderVH".

  class NetBuilderVH : public NetBuilder {
    public:
                            NetBuilderVH       ();
      virtual              ~NetBuilderVH       ();
      virtual void          doRp_AutoContacts  ( GCell*, Component*, AutoContact*& source, AutoContact*& target, uint64_t flags );
      virtual AutoContact*  doRp_Access        ( GCell*, Component*, uint64_t  flags );
    private:                                     
      virtual bool          _do_1G_1PinM2      ();
      virtual bool          _do_xG_1PinM2      ();
      virtual bool          _do_1G_1PinM3      ();
      virtual bool          _do_xG_1PinM3      ();
      virtual bool          _do_1G_1M1         ();
      virtual bool          _do_1G_xM1         ();
      virtual bool          _do_2G_1M1         ();
      virtual bool          _do_xG_xM1_xM3     ();
      virtual bool          _do_xG             ();
      virtual bool          _do_globalSegment  ();
      virtual void          singleGCell        ( AnabaticEngine*, Net* );
    public:
      virtual string        getTypeName        () const;
  };


}  // Anabatic namespace.

#endif  // ANABATIC_NET_BUILDER_VH_H

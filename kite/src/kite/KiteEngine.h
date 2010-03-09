
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2009, All Rights Reserved
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                   C O R I O L I S                               |
// |      K i t e  -  D e t a i l e d   R o u t e r                  |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./KiteEngine.h"                           |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#ifndef  __KITE_KITE_ENGINE__
#define  __KITE_KITE_ENGINE__

#include  "hurricane/Name.h"
namespace Hurricane {
  class Layer;
  class Net;
  class Cell;
}

#include  "crlcore/RoutingGauge.h"
#include  "katabatic/KatabaticEngine.h"

namespace Knik {
  class KnikEngine;
}

#include  "kite/TrackElement.h"
#include  "kite/Configuration.h"


namespace Kite {


  using Hurricane::Name;
  using Hurricane::Layer;
  using Hurricane::Net;
  using Hurricane::Cell;
  using CRL::RoutingGauge;
  using Katabatic::KatabaticEngine;

  class GCellGrid;
  class Track;
  class RoutingPlane;
  class NegociateWindow;


// -------------------------------------------------------------------
// Enumerations


  enum GlobalFlags { BuildGlobalSolution=1, LoadGlobalSolution };

// -------------------------------------------------------------------
// Class  :  "Kite::KiteEngine".
 

  class KiteEngine : public KatabaticEngine {
    public:
      enum LookupFlags { OnAllCollapseds=1 };

    public:
      static  const Name&      staticGetName              ();
      static  KiteEngine*      create                     ( const RoutingGauge*, Cell* );
      static  KiteEngine*      get                        ( const Cell* );
    public:                                               
      inline  KatabaticEngine* base                       ();
      inline  Configuration*   getKiteConfiguration       ();
      inline  Net*             getBlockageNet             ();
      virtual Configuration*   getConfiguration           ();
      inline  bool             getToolSuccess             () const;
      inline  unsigned int     getRipupLimit              ( unsigned int type ) const;
              unsigned int     getRipupLimit              ( const TrackElement* ) const;
      inline  unsigned int     getRipupCost               () const;
      inline  float            getExpandStep              () const;
      inline  float            getEdgeCapacityPercent     () const;
      inline  GCellGrid*       getGCellGrid               () const;
      virtual const Name&      getName                    () const;
      inline  Configuration::PostEventCb_t&
                               getPostEventCb             ();
      inline  NegociateWindow* getNegociateWindow         ();
      inline  size_t           getRoutingPlanesSize       () const;
              RoutingPlane*    getRoutingPlaneByIndex     ( size_t index ) const;
              RoutingPlane*    getRoutingPlaneByLayer     ( const Layer* ) const;
              Track*           getTrackByPosition         ( const Layer*, DbU::Unit axis, unsigned int mode=Constant::Nearest ) const;
              void             printCompletion            () const;
      inline  void             setPostEventCb             ( Configuration::PostEventCb_t );
      inline  void             setMinimumWL               ( double );
      inline  void             setRipupLimit              ( unsigned int, unsigned int type );
      inline  void             setRipupCost               ( unsigned int );
      inline  void             setExpandStep              ( float );
      inline  void             setEdgeCapacityPercent     ( float );
              void             preProcess                 ();
              void             buildBlockages             ();
              void             buildPowerRails            ();
              void             createGlobalGraph          ( unsigned int mode );
      virtual void             createDetailedGrid         ();
              void             saveGlobalSolution         ();
              void             annotateGlobalGraph        ();
              void             runGlobalRouter            ( unsigned int mode );
      virtual void             loadGlobalRouting          ( unsigned int method, vector<Net*>& );
              void             runNegociate               ( unsigned int slowMotion=0 );
              void             setInterrupt               ( bool );
      virtual void             finalizeLayout             ();
              void             _gutKite                   ();
      inline  TrackElementLut& _getTrackElementLut        ();
              void             _link                      ( TrackElement* );
              void             _unlink                    ( TrackElement* );        
              TrackElement*    _lookup                    ( Segment* ) const;
      inline  TrackElement*    _lookup                    ( AutoSegment* ) const;
              bool             _check                     ( unsigned int& overlap, const char* message=NULL ) const;
              void             _check                     ( Net* ) const;
              void             _computeCagedConstraints   ();
              void             _computeCagedConstraints   ( Net* );
      virtual Record*          _getRecord                 () const;
      virtual string           _getString                 () const;
      virtual string           _getTypeName               () const;

    private:
    // Attributes.
      static Name                   _toolName;
    protected:
             Knik::KnikEngine*      _knik;
             Net*                   _obstacleNet;
             Configuration          _configuration;
             vector<RoutingPlane*>  _routingPlanes;
             GCellGrid*             _kiteGrid;
             NegociateWindow*       _negociateWindow;
             TrackElementLut        _trackSegmentLut;
             double                 _minimumWL;
             mutable bool           _toolSuccess;

    protected:
    // Constructors & Destructors.
                          KiteEngine  ( const RoutingGauge*, Cell* );
      virtual            ~KiteEngine  ();
      virtual void        _postCreate ();
      virtual void        _preDestroy ();
    private:
                          KiteEngine  ( const KiteEngine& );
              KiteEngine& operator=   ( const KiteEngine& );
  };


// Inline Functions.
  inline  KatabaticEngine*              KiteEngine::base                   () { return static_cast<KatabaticEngine*>(this); }
  inline  Configuration*                KiteEngine::getKiteConfiguration   () { return &_configuration; }
  inline  Net*                          KiteEngine::getBlockageNet         () { return _obstacleNet; }
  inline  Configuration::PostEventCb_t& KiteEngine::getPostEventCb         () { return _configuration.getPostEventCb(); }
  inline  bool                          KiteEngine::getToolSuccess         () const { return _toolSuccess; }
  inline  unsigned int                  KiteEngine::getRipupCost           () const { return _configuration.getRipupCost(); }
  inline  float                         KiteEngine::getExpandStep          () const { return _configuration.getExpandStep(); }
  inline  float                         KiteEngine::getEdgeCapacityPercent () const { return _configuration.getEdgeCapacityPercent(); }
  inline  unsigned int                  KiteEngine::getRipupLimit          ( unsigned int type ) const { return _configuration.getRipupLimit(type); }
  inline  GCellGrid*                    KiteEngine::getGCellGrid           () const { return _kiteGrid; }
  inline  NegociateWindow*              KiteEngine::getNegociateWindow     () { return _negociateWindow; }
  inline  size_t                        KiteEngine::getRoutingPlanesSize   () const { return _routingPlanes.size(); }
  inline  void                          KiteEngine::setRipupLimit          ( unsigned int limit, unsigned int type ) { _configuration.setRipupLimit(limit,type); }
  inline  void                          KiteEngine::setRipupCost           ( unsigned int cost ) { _configuration.setRipupCost(cost); }
  inline  void                          KiteEngine::setExpandStep          ( float step ) { _configuration.setExpandStep(step); }
  inline  void                          KiteEngine::setEdgeCapacityPercent ( float percent ) { _configuration.setEdgeCapacityPercent(percent); }
  inline  void                          KiteEngine::setMinimumWL           ( double minimum ) { _minimumWL = minimum; }
  inline  void                          KiteEngine::setPostEventCb         ( Configuration::PostEventCb_t cb ) { _configuration.setPostEventCb(cb); }
  inline  TrackElementLut&              KiteEngine::_getTrackElementLut    () { return _trackSegmentLut; }
  inline  TrackElement*                 KiteEngine::_lookup                ( AutoSegment* as ) const { return _lookup(as->base()); }


// Variables.
  extern const char* missingRW;


} // End of Kite namespace.


INSPECTOR_P_SUPPORT(Kite::KiteEngine);


#endif  // __KITE_KITE_ENGINE__
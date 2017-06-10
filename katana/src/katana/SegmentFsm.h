// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC 2008-2016, All Rights Reserved
//
// +-----------------------------------------------------------------+
// |                   C O R I O L I S                               |
// |      K i t e  -  D e t a i l e d   R o u t e r                  |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Header  :   "./katana/SegmentFsm.h"                        |
// +-----------------------------------------------------------------+


#ifndef  KATANA_SEGMENT_FSM_H
#define  KATANA_SEGMENT_FSM_H

#include "katana/TrackCost.h"

namespace Katana {

  class TrackElement;
  class DataNegociate;
  class RoutingEvent;
  class RoutingEventQueue;
  class RoutingEventHistory;


// -------------------------------------------------------------------
// Class  :  "SegmentAction".

  class SegmentAction {
    public:
      enum Type { Self                          = (1<< 0)
                , Other                         = (1<< 1)
                , Perpandicular                 = (1<< 2)
                , Insert                        = (1<< 3)
                , Ripup                         = (1<< 4)
                , RipedByLocal                  = (1<< 5)
                , ResetRipup                    = (1<< 6)
                , ToRipupLimit                  = (1<< 7)
                , MoveToAxis                    = (1<< 8)
                , AxisHint                      = (1<< 9)
                , PackingMode                   = (1<<10)
                , ToState                       = (1<<11)
                , EventLevel1                   = (1<<12)
                , EventLevel2                   = (1<<13)
                , EventLevel3                   = (1<<14)
                , EventLevel4                   = (1<<15)
                , EventLevel5                   = (1<<16)
                , SelfInsert                    = Self |Insert
                , SelfRipup                     = Self |Ripup
                , SelfRipupPerpand              = Self |Ripup|Perpandicular
                , SelfRipupPerpandWithAxisHint  = Self |Ripup|Perpandicular|EventLevel4|AxisHint
                , OtherRipup                    = Other|Ripup
                , OtherRipupPerpandAndPushAside = Other|Ripup|Perpandicular|EventLevel3|AxisHint
                , OtherRipupPerpandAndPacking   = Other|Ripup|Perpandicular|EventLevel4|PackingMode
                };
    public:
                           SegmentAction ( TrackElement*
                                         , uint32_t      type
                                         , DbU::Unit     axisHint=0
                                         , uint32_t      toState =0
                                         );
      inline TrackElement* getSegment    () const;
      inline uint32_t      getType       () const;
      inline void          setAxisHint   ( DbU::Unit );
      inline uint32_t      setFlag       ( uint32_t );
             bool          doAction      ( RoutingEventQueue& );
    private:
      TrackElement* _segment;
      uint32_t      _type;
      DbU::Unit     _axisHint;
      uint32_t      _toState;
  };


  inline TrackElement* SegmentAction::getSegment  () const { return _segment; }
  inline uint32_t      SegmentAction::getType     () const { return _type; }
  inline void          SegmentAction::setAxisHint ( DbU::Unit axis ) { _axisHint = axis; }
  inline uint32_t      SegmentAction::setFlag     ( uint32_t flag ) { _type |= flag; return _type; }


// -------------------------------------------------------------------
// Class  :  "SegmentFsm".

  class SegmentFsm {

    public:
      enum State        { MissingData              = (1<<0)
                        , EmptyTrackList           = (1<<1)
                        , Inserted                 = (1<<2)
                        , Self                     = (1<<3)
                        , Other                    = (1<<4)
                        , Ripup                    = (1<<5)
                        , MaximumSlack             = (1<<6)
                        , SelfInserted             = Self  | Inserted
                        , OtherRipup               = Other | Ripup
                        , SelfMaximumSlack         = Self  | MaximumSlack
                        };
      enum SlackenFlags { NoRecursive  = (1<<0)
                        , NoTransition = (1<<1)
                        };

    public:
                                    SegmentFsm             ( RoutingEvent*
                                                           , RoutingEventQueue&
                                                           , RoutingEventHistory&
                                                           );
                                   ~SegmentFsm             ();
      inline bool                   isFullBlocked          () const;
      inline bool                   isSymmetric            () const;
      inline RoutingEvent*          getEvent               () const;
      inline RoutingEvent*          getEvent1              () const;
      inline RoutingEvent*          getEvent2              () const;
      inline RoutingEventQueue&     getQueue               () const;
      inline RoutingEventHistory&   getHistory             () const;
      inline TrackElement*          getSegment1            () const;
      inline TrackElement*          getSegment2            () const;
      inline uint32_t               getState               () const;
      inline DataNegociate*         getData                ();
      inline DataNegociate*         getData1               ();
      inline DataNegociate*         getData2               ();
      inline Interval&              getConstraint          ();
      inline Interval&              getOptimal             ();
      inline vector<TrackCost*>&    getCosts               ();
      inline TrackCost*             getCost                ( size_t );
      inline Track*                 getTrack               ( size_t );
      inline Track*                 getTrack1              ( size_t );
      inline Track*                 getTrack2              ( size_t );
      inline size_t                 getBegin               ( size_t );
      inline size_t                 getBegin1              ( size_t );
      inline size_t                 getBegin2              ( size_t );
      inline size_t                 getEnd                 ( size_t );
      inline size_t                 getEnd1                ( size_t );
      inline size_t                 getEnd2                ( size_t );
      inline vector<SegmentAction>& getActions             ();
      inline void                   setState               ( uint32_t );
             void                   setDataState           ( uint32_t );
             void                   addAction              ( TrackElement*
                                                           , uint32_t   type
                                                           , DbU::Unit  axisHint=0
                                                           , uint32_t   toState =0
                                                           );
             void                   doActions              ();
      inline void                   clearActions           ();
      inline SegmentFsm&            useEvent1              ();
      inline SegmentFsm&            useEvent2              ();
             void                   incRipupCount          ();
             bool                   insertInTrack          ( size_t );
             void                   bindToTrack            ( size_t );
             void                   moveToTrack            ( size_t );
             void                   ripupPerpandiculars    ();
             bool                   canRipup               ( uint32_t flags=0 );
             bool                   conflictSolveByHistory ();
             bool                   conflictSolveByPlaceds ();
             bool                   solveTerminalVsGlobal  ();
             bool                   desaturate             ();
             bool                   slackenTopology        ( uint32_t flags=0 );
             bool                   solveFullBlockages     ();
    private:                                               
             bool                   _slackenStrap          ( TrackElement*& 
                                                           , DataNegociate*&
                                                           , uint32_t        flags );
             bool                   _slackenLocal          ( TrackElement*& 
                                                           , DataNegociate*&
                                                           , uint32_t        flags  );
             bool                   _slackenGlobal         ( TrackElement*& 
                                                           , DataNegociate*&
                                                           , uint32_t        flags  );
    private:                        
      RoutingEvent*                 _event1;
      RoutingEvent*                 _event2;
      RoutingEventQueue&            _queue;
      RoutingEventHistory&          _history;
      uint32_t                      _state;
      DataNegociate*                _data1;
      DataNegociate*                _data2;
      Interval                      _constraint;
      Interval                      _optimal;
      vector<TrackCost*>            _costs;
      vector<SegmentAction>         _actions;
      bool                          _fullBlocked;
      bool                          _sameAxis;
      bool                          _useEvent2;
  };


  inline bool                   SegmentFsm::isSymmetric   () const { return _event2 != NULL; }
  inline bool                   SegmentFsm::isFullBlocked () const { return _fullBlocked and _costs.size(); }
  inline RoutingEvent*          SegmentFsm::getEvent      () const { return (_useEvent2) ? _event2 : _event1; }
  inline RoutingEvent*          SegmentFsm::getEvent1     () const { return _event1; }
  inline RoutingEvent*          SegmentFsm::getEvent2     () const { return _event2; }
  inline RoutingEventQueue&     SegmentFsm::getQueue      () const { return _queue; }
  inline RoutingEventHistory&   SegmentFsm::getHistory    () const { return _history; }
  inline uint32_t               SegmentFsm::getState      () const { return _state; }
  inline TrackElement*          SegmentFsm::getSegment1   () const { return _event1->getSegment(); }
  inline TrackElement*          SegmentFsm::getSegment2   () const { return (_event2) ? _event2->getSegment() : NULL; }
  inline DataNegociate*         SegmentFsm::getData       () { return (_useEvent2) ? _data2 : _data1; }
  inline DataNegociate*         SegmentFsm::getData1      () { return _data1; }
  inline DataNegociate*         SegmentFsm::getData2      () { return _data2; }
  inline Interval&              SegmentFsm::getConstraint () { return _constraint; }
  inline Interval&              SegmentFsm::getOptimal    () { return _optimal; }
  inline vector<TrackCost*>&    SegmentFsm::getCosts      () { return _costs; }
  inline TrackCost*             SegmentFsm::getCost       ( size_t i ) { return _costs[i]; }
  inline Track*                 SegmentFsm::getTrack      ( size_t i ) { return (_useEvent2) ? getTrack2(i) : getTrack1(i); }
  inline size_t                 SegmentFsm::getBegin      ( size_t i ) { return (_useEvent2) ? getBegin2(i) : getBegin1(i); }
  inline size_t                 SegmentFsm::getEnd        ( size_t i ) { return (_useEvent2) ? getEnd2  (i) : getEnd1  (i); }
  inline Track*                 SegmentFsm::getTrack1     ( size_t i ) { return _costs[i]->getTrack(0,TrackCost::NoFlags  ); }
  inline Track*                 SegmentFsm::getTrack2     ( size_t i ) { return _costs[i]->getTrack(0,TrackCost::Symmetric); }
  inline size_t                 SegmentFsm::getBegin1     ( size_t i ) { return _costs[i]->getBegin(0,TrackCost::NoFlags  ); }
  inline size_t                 SegmentFsm::getBegin2     ( size_t i ) { return _costs[i]->getBegin(0,TrackCost::Symmetric); }
  inline size_t                 SegmentFsm::getEnd1       ( size_t i ) { return _costs[i]->getEnd  (0,TrackCost::NoFlags  ); }
  inline size_t                 SegmentFsm::getEnd2       ( size_t i ) { return _costs[i]->getEnd  (0,TrackCost::Symmetric); }
  inline vector<SegmentAction>& SegmentFsm::getActions    () { return _actions; }
  inline void                   SegmentFsm::setState      ( uint32_t state ) { _state = state; }
  inline void                   SegmentFsm::clearActions  () { _actions.clear(); }
  inline SegmentFsm&            SegmentFsm::useEvent1     () { _useEvent2 = false; return *this; }
  inline SegmentFsm&            SegmentFsm::useEvent2     () { _useEvent2 = true ; return *this; }


}  // Katana namespace.

#endif  // KATANA_SEGMENT_FSM_H

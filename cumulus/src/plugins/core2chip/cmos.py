#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This file is part of the Coriolis Software.
# Copyright (c) UPMC 2019-2018, All Rights Reserved
#
# +-----------------------------------------------------------------+
# |                   C O R I O L I S                               |
# |      C u m u l u s  -  P y t h o n   T o o l s                  |
# |                                                                 |
# |  Author      :                    Jean-Paul CHAPUT              |
# |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
# | =============================================================== |
# |  Python      :       "./plugins/core2chip/cmos.py"              |
# +-----------------------------------------------------------------+

import re
from   Hurricane            import DbU
from   Hurricane            import DataBase
from   Hurricane            import UpdateSession
from   Hurricane            import Breakpoint
from   Hurricane            import Transformation
from   Hurricane            import Instance
from   Hurricane            import Net
import Viewer               
from   CRL                  import Catalog
from   CRL                  import AllianceFramework
from   helpers.io           import ErrorMessage
from   core2chip.CoreToChip import CoreToChip


class cmos ( CoreToChip ):

    def __init__ ( self, core ):
        CoreToChip.__init__ ( self, core )
        self.ringNetNames = [ 'vsse', 'vssi', 'vdde', 'vddi', ('cki', 'ck') ]
        self.ioPads       = { Net.Direction.IN  : CoreToChip.IoPadInfo( 'pi_px', 'pad', ['t',] )
                            , Net.Direction.OUT : CoreToChip.IoPadInfo( 'po_px', 'pad', ['i',] )
                            }
        self._getPadLib()
        return

    def _getPadLib ( self ):
        self.padLib = AllianceFramework.get().getLibrary( "pxlib" )

        if not self.padLib:
          message = [ 'CoreToChip.cmos._getPadLib(): Unable to find Alliance "pxlib" library' ]
          raise ErrorMessage( 1, message )

        return 

    def getNetType ( self, netName ):
        if netName.startswith('vss'): return Net.Type.GROUND
        if netName.startswith('vdd'): return Net.Type.POWER
        if netName in ('cki', 'ck'):  return Net.Type.CLOCK
        return Net.Type.LOGICAL

    def isGlobal ( self, netName ):
        if netName in self.ringNetNames: return True
        return False

    def getCell ( self, masterCellName ):
       #cell = self.padLib.getCell( masterCellName )
        cell = AllianceFramework.get().getCell( masterCellName, Catalog.State.Views )
        if not cell:
          raise ErrorMessage( 1, 'cmos.getCell(): I/O pad library "%s" does not contain cell named "%s"' \
                                 % (self.padLib.getName(),masterCellName) )
        return cell

    def _buildGroundPads ( self, ioNet ):
        vssi = self.chip.getNet( 'vssi' )
        vssi.setExternal( True )
        vssi.setGlobal  ( True )
        vssi.setType    ( Net.Type.GROUND )
        vssi.merge( ioNet.chipIntNet )
        ioNet.chipIntNet = vssi

        vsse = self.chip.getNet( 'vsse' )
        vsse.setExternal( True )
        vsse.setGlobal  ( True )
        vsse.setType    ( Net.Type.GROUND )
        vsse.merge( ioNet.chipExtNet )
        ioNet.chipExtNet = vsse

        ioNet.pads.append( Instance.create( self.chip
                                          , ioNet.padInstanceName + '_i_%d' % self.groundPadCount
                                          , self.getCell('pvssick_px') ) )
        ioNet.pads.append( Instance.create( self.chip
                                          , ioNet.padInstanceName + '_o_%d' % self.groundPadCount
                                          , self.getCell('pvsseck_px') ) )

        CoreToChip._connect( ioNet.pads[0], ioNet.chipIntNet, 'vssi' )
        CoreToChip._connect( ioNet.pads[1], ioNet.chipExtNet, 'vsse' )

        for pad in ioNet.pads: self._connectRing( pad )
        self.groundPadCount += 1
        self.chipPads       += ioNet.pads
        return

    def _buildPowerPads ( self, ioNet ):
        vddi = self.chip.getNet( 'vddi' )
        vddi.setExternal( True )
        vddi.setGlobal  ( True )
        vddi.setType    ( Net.Type.POWER )
        vddi.merge( ioNet.chipIntNet )
        ioNet.chipIntNet = vddi

        vdde = self.chip.getNet( 'vdde' )
        vdde.setExternal( True )
        vdde.setGlobal  ( True )
        vdde.setType    ( Net.Type.POWER )
        vdde.merge( ioNet.chipExtNet )
        ioNet.chipExtNet = vdde

        ioNet.pads.append( Instance.create( self.chip
                                          , ioNet.padInstanceName + '_i_%d' % self.powerPadCount
                                          , self.getCell('pvddick_px') ) )
        ioNet.pads.append( Instance.create( self.chip
                                          , ioNet.padInstanceName + '_o_%d' % self.powerPadCount
                                          , self.getCell('pvddeck_px') ) )

        CoreToChip._connect( ioNet.pads[0], ioNet.chipIntNet, 'vddi' )
        CoreToChip._connect( ioNet.pads[1], ioNet.chipExtNet, 'vdde' )

        for pad in ioNet.pads: self._connectRing( pad )
        self.powerPadCount += 1
        self.chipPads      += ioNet.pads
        return

    def _buildClockPads ( self, ioNet ):
        ioNet.pads.append( Instance.create( self.chip
                                          , ioNet.padInstanceName + '_%d' % self.clockPadCount
                                          , self.getCell('pck_px') ) )

        CoreToChip._connect( ioNet.pads[0], ioNet.chipExtNet, 'pad' )

        for pad in ioNet.pads: self._connectRing( pad )
        self.clockPadCount += 1
        self.chipPads      += ioNet.pads

        p = re.compile( r'pv[ds]{2}[ei]ck_px' )
        for pad in self.chipPads: 
          if p.match( pad.getMasterCell().getName() ): 
            CoreToChip._connect( pad, ioNet.chipIntNet, 'cko' )
        return

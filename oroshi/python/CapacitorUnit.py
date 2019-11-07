#!/usr/bin/python

import sys                
import numpy
from   Hurricane  import *
from   CRL        import *
from   math       import sqrt, ceil
import helpers
from   helpers.io import ErrorMessage as Error
from   helpers    import trace
import oroshi

def toDbU ( l ): return DbU.fromPhysical( l, DbU.UnitPowerMicro )


## Draws a capacitor of type Poly-Poly or Metal-Metal in 350 nm AMS CMOS technology. 
#  PIP and MIM capacitors are the result of surface superposition between poly1 and poly2 or metal2 and metalcap layers, respectively.
#  Given the capacitor value, layout dimensions are computed, then,  capacitor layers are drawn. Capacitor value, \f$C\f$, is given in the expression below, where \f$ C_{a}, C_{p}, A \f$ and \f$ P \f$ are, area capacitance, perimeter capacitance, area and permiter of the capacitor, respectively :
#  \f[ C = C_{a}A + C_{p}P \f]
#  The drawn layout shape is square. Thus, metcap or poly2 length and width are equal and are computed using the capacitor expression. Furthermore, given \f$ C_{a} \f$, \f$ C_{p} \f$ and enclosure technological rules, dimensions and positions of the abutment box as well as the bottom plate are computed. Layouts with dimensions that exceed technological limits cannot be drawn.

class CapacitorUnit():
    
    rules = oroshi.getRules()

    ## This is the class constructor. Few of the class attributes final values are computed in this level. Most of attributes are only initialized to zero or empty values. Then, it is computed in dedicated class method. Input parameters are :
    # \param   device                         Hurricane AMS device into which layout is drawn. 
    # \param   capacitance                    Capacitor value, expressed in \f$ femto Farad (fF) \f$.
    # \param   abutmentBoxPosition            A list containing abscissa and ordinate of the bottom left corner of the abutment box.
    #
    # Class attributes are described in the list below. Most of class attributes refer to layout dimensions. Dictionaries are used to group attributes related to the same layout varibale. Layout dimensions and variables are described in Figure 1. 
    #
    # \param   device                         Hurricane AMS device into which layout is drawn. 
    # \param   capacitance                    Capacitor value, expressed in \f$ femto Farad (fF) \f$.
    # \param   capacitorType                  Can be 'MIMCap' or 'PIPCap' as capacitor type. 
    # \param   abutmentBoxDict                A dictionary containing abscissa and ordinate of the bottom left corner of the abutment box, (XMin) and (YMin), respectively. 
    # \param   abutmentBox                       Abutment box drawn square. It is an object of type \c Box.               
    # \param   bottomPlateBox                 Bottom plate drawn square. It is an object of type \c Box.
    # \param   topPlateBox                    Top plate drawn square. It is an object of type \c Box.
    # \param   cut2MatrixDict                      A dictionary containing center position of the left bottom, which is cut the first to be drawn in the matrix of cuts. Initially, the dictionary is empty. It is only updated when \c self.capacitorType is equal to \c 'MIMCap'. 
    #
    # \param   cutLeftLineDict                A dictionary containing abcissa and ordinate of the bottom cut in the left line of cuts to be drawn on bottom plate's layer. 
    # \param   cutRightLineDict               A dictionary containing abcissa and ordinate of the bottom cut in the right line of cuts to be drawn on bottom plate's layer.
    # \param   topCutLineDict                 A dictionary containing abcissa and ordinate of the bottom cut in the right line of cuts to be drawn on top plate's layer. Initially, the dictionary is empty. It is only updated when \c self.capacitorType is equal to \c 'PIPCap'. 
    #
    # \param   topPlateRLayerDict             A dictionary containing position information of the top plate's routing layer. The dictionary includes ordinates of the layer's top and bottom extremities, \c 'XMin' and \c 'YMin', respectively, the abcissa of it's center, \c 'XCenter' and its width, \c 'width'. 
    #
    # \param   bottomPlateRLayerDict          A dictionary containing
    # \param   enclosure_botPlate_topPlate    Top plate's layer encolusre in bottom plate's layer.
    # \param   minheight_topPlatecut          Minimum height of cuts for top plate connection to other metal layer.
    # \param   topCutLineNumber               Maximum possible number cuts to be drawn for top plate's connection.
    # \param   bottomCutLineNumber            Maximum possible number cuts to be drawn for top plate's connection.


    def __init__( self, device, capacitance, capacitorType, abutmentBoxPosition ):
       
        self.device                      =   device
        self.capacitorType               =   capacitorType
        self.capDim                      =   self.__computeCapDim__( capacitance, capacitorType  ) 

        self.abutmentBoxDict             = { "XMin" : abutmentBoxPosition[0], "YMin" : abutmentBoxPosition[1] }
        self.abutmentBox                 =   Box ()
        self.bottomPlateBox              =   Box ()
        self.topPlateBox                 =   Box ()

        self.bottomPlateBoxDict          =   {}
        self.topPlateBoxDict             =   {}

        self.cut2MatrixDict              =   {}     
        self.cutLeftLineDict             =   {} 
        self.cutRightLineDict            =   {}
        self.topCutLineDict              =   {}       
        self.topPlateRLayerDict          =   {}
        self.bottomPlateRLayerDict       =   {}

        self.enclosure_botPlate_topPlate =   0
        self.minheight_topPlatecut       =   0
        self.topCutLineNumber            =   0
        self.bottomCutLineNumber         =   0

#        self.nets                        = { "bNet" : bNet, "tNet" : tNet }

        return



    ## Sets the area and perimeter capacitances as specified in 350 nm AMS technology and according to \c capacitorType (MIM or PIP).
    #  \return    a list containing the area and perimeter capacitances.
    #  \remarks   An exception is raised if the entered capacitor type is unknown.
  
    def __setCapacitorPerUnit__( self, capacitorType ) :

        if   capacitorType == 'MIMCap':
            [ areaCapacitorPerUnit, perimeterCapacitorPerUnit ] = [ CapacitorUnit.rules.MIMCap, CapacitorUnit.rules.MIMPerimeterCap ]

        elif capacitorType == 'PIPCap':
            [ areaCapacitorPerUnit, perimeterCapacitorPerUnit ] = [ CapacitorUnit.rules.PIPCap, CapacitorUnit.rules.PIPPerimeterCap ]

        else:
            raise Error( 1, 'setCapacitorPerUnit() : Unsupported capacitor type : %s.' % capacitorType ) 

        return [ areaCapacitorPerUnit, perimeterCapacitorPerUnit ]


    ## Computes width and length of the capacitor.  Given \c capacitance value as well as the permiter and area capacitances, a quadratic equation is solved where the unknown parameter is the width ( also equivalent to the length ).
   #  \return    a dictionary containing width and length.
   #  \remark    The capacitor is square. Thus, length and width are equal.

    def __computeCapDim__( self, capacitance, capacitorType ) : 

        [a, b, c ] = [ self.__setCapacitorPerUnit__( capacitorType )[0], 4*self.__setCapacitorPerUnit__( capacitorType )[1], -capacitance ]
        delta = (b**2) - 4*a*c
        c1 = ( -b + sqrt(delta) ) / (2*a)

        return { "width" : toDbU(c1), "height" : toDbU(c1) }



    ## Checks if the computed capacitor dimensions exceed or are less than maximum and minimum limits, respectively, as specified in technology rules.
    ## \return   \c True if all rules are respected.
    #  \remark    Maximum poly2 layer dimensions for PIP capacitor are not specified in technology rules. Thus, only minimum limit condition is checked.

    def __isCapacitorUnitOK__( self, capDim ):

        if ( self.capacitorType == 'MIMCap' and CapacitorUnit.getMinimumCapWidth( self ) < capDim["width"] <  self.getMaximumCapWidth()) or self.capacitorType == 'PIPCap' and self.getMinimumCapWidth() < capDim["width"]:
            return True   



    ## Redefines technological rules as object attributes of the class. For example, class attributes \c CapacitorUnit.rules.minWidth_metcap becomes \c self.minWidth_topPlate and \c CapacitorUnit.rules.minSpacing_cut2_metcap becomes \c self.minSpacing_botPlateCut_topPlat. This obviously helps using shorter names for the attributes.  \c __setattr__() is declared in all \c Capacitor \c Generator classes, also in Hurricane and oroshi databases. 

    def __setattr__( self, attribute, value ):
 
        self.__dict__[attribute] = value

        return


    ## Selects technological rules according to the capacitor type.     
    #  \return a dictionary with rules labels as keys and rules as values. 
    #  Example of technology rules are :
    #  - minimum spacing between cuts or metals,
    #  - minimum width of a plate, a cut or a routing metal.
    #  - etc.  
    #  Every rule takes two possible value according to the capacitor type (MIM or PIP). Therefore, dictionary keys are generic and its values are specific to the capacitor type.
    #  \remark   An exception is raised if the entered capacitor type is unknown.

    def setRules ( self ):

        if self.capacitorType == 'MIMCap':

            self.__setattr__( "minWidth_topPlate"                   , CapacitorUnit.rules.minWidth_metcap          )
            self.__setattr__( "minWidth_botRMetal"                  , CapacitorUnit.rules.minWidth_metal3          )
            self.__setattr__( "minWidth_topRMetal"                  , CapacitorUnit.rules.minWidth_metal1          )
            self.__setattr__( "minWidth_topPlatecut"                , CapacitorUnit.rules.minWidth_cut2            )
            self.__setattr__( "minWidth_botPlatecut"                , CapacitorUnit.rules.minWidth_cut2            )
            self.__setattr__( "minWidth_routingTrackcut"            , CapacitorUnit.rules.minWidth_cut2            )

            self.__setattr__( "minSpacing_botPlate"                 , CapacitorUnit.rules.minSpacing_metbot        )        
            self.__setattr__( "minSpacing_botPlateCut_topPlate"     , CapacitorUnit.rules.minSpacing_cut2_metcap   )
            self.__setattr__( "minSpacingOnBotPlate_cut"            , CapacitorUnit.rules.minSpacingOnMetBot_cut2  )
            self.__setattr__( "minSpacingOnTopPlate_cut"            , CapacitorUnit.rules.minSpacingOnMetCap_cut2  )
 
            self.__setattr__( "minEnclo_botPlate_botPlateCut"       , CapacitorUnit.rules.minEnclosure_metbot_cut2 )
            self.__setattr__( "minEnclo_topPlate_topPlateCut"       , CapacitorUnit.rules.minEnclosure_metcap_cut2 )
            self.__setattr__( "minEnclo_topPlateRMetal_topPlateCut" , CapacitorUnit.rules.minEnclosure_metal3_cut2 )
            self.__setattr__( "minEnclo_botPlateRMetal_botPlateCut" , CapacitorUnit.rules.minEnclosure_metal3_cut2 )
            self.__setattr__( "minEnclo_routingTrackMetal_cut"      , CapacitorUnit.rules.minEnclosure_metal3_cut2 )
        
        elif self.capacitorType == 'PIPCap':

            self.__setattr__( "minWidth_topPlate"                   , CapacitorUnit.rules.minWidth_cpoly           )
            self.__setattr__( "minWidth_botRMetal"                  , CapacitorUnit.rules.minWidth_metal1          )
            self.__setattr__( "minWidth_topRMetal"                  , CapacitorUnit.rules.minWidth_metal1          )
            self.__setattr__( "minWidth_topPlatecut"                , CapacitorUnit.rules.minWidth_cut0            )
            self.__setattr__( "minWidth_botPlatecut"                , CapacitorUnit.rules.minWidth_cut0            )
            self.__setattr__( "minWidth_routingTrackcut"            , CapacitorUnit.rules.minWidth_cut1            )

            self.__setattr__( "minSpacing_botPlate"                 , CapacitorUnit.rules.minSpacing_poly          )        
            self.__setattr__( "minSpacing_botPlateCut_topPlate"     , CapacitorUnit.rules.minSpacing_cut0_cpoly    )
            self.__setattr__( "minSpacingOnBotPlate_cut"            , CapacitorUnit.rules.minSpacing_cut0          )
            self.__setattr__( "minSpacingOnTopPlate_cut"            , CapacitorUnit.rules.minSpacing_cut0          )

            self.__setattr__( "minEnclo_botPlate_botPlateCut"       , CapacitorUnit.rules.minEnclosure_poly_cut0   )
            self.__setattr__( "minEnclo_topPlate_topPlateCut"       , CapacitorUnit.rules.minEnclosure_cpoly_cut0  )
            self.__setattr__( "minEnclo_topPlateRMetal_topPlateCut" , CapacitorUnit.rules.minEnclosure_metal1_cut1 )
            self.__setattr__( "minEnclo_botPlateRMetal_botPlateCut" , CapacitorUnit.rules.minEnclosure_metal1_cut0 )
            self.__setattr__( "minEnclo_routingTrackMetal_cut"      , CapacitorUnit.rules.minEnclosure_metal1_cut1 )

        else :
            raise Error( 1, 'setRules() : Unsupported capacitor type : %s.' % self.capacitorType ) 

        self.minheight_topPlatecut = self.minWidth_topPlatecut

        return



    ## \return capacitor type \c 'MIMCap' or \c 'PIPCap'. 
    #  \remarks \c getCapacitorType() is especially useful when an instance of \c CapacitorUnit class is called in another classes instances to identify the capacitor's type. 

    def getCapacitorType   ( self ) : return self.capacitorType



    ## \retun maximum size of capacitor's top plate. \c getMaximumCapWidth() is called to check if capacitor dimensions are within acceptable technological limits.  
    #  An exception is raised if the entered capacitor type is unknown.
    #  \remarks 1. This function is especially usefull in drawing the layout of a unity capacitor, where it is important to garantee that the capacitor size does not exeed the maximum possible value. It is also useful when drawing a matrix of capacitors to make sure that also the unity capacitor respects the maximal values specified. \remarks 2. The maximum value of the poly2 size in PIP capacitor is not specified. Thus, it is not considered in \c getMaximumCapWidth() 

    def getMaximumCapWidth ( self ) :   

        if   self.capacitorType == 'MIMCap':
            maximumCapWidth = CapacitorUnit.rules.maxWidth_metcap

        elif self.capacitorType == 'PIPCap':
            maximumCapWidth = None #CapacitorUnit.rules.maxWidth_cpoly

        else:
            raise Error( 1, 'getMaximumCapWidth() : Unsupported capacitor type : %s.' % self.capacitorType ) 

        return maximumCapWidth


    ## \return   The minimum size of the capacitor's top plate. An exception is raised if the entered capacitor type is unknown. 
    #  \remarks  This function is especially usefull in drawing the layout of a matrix of capacitors where it is important to ensure that the unity capacitor respects the minimal values specified. \remarks An exception is raised if the entered capacitor type is unknown.

    def getMinimumCapWidth ( self ) :   

        if   self.capacitorType == 'MIMCap':
            minimumCapWidth = CapacitorUnit.rules.minWidth_metcap

        elif self.capacitorType == 'PIPCap':
            minimumCapWidth = CapacitorUnit.rules.minWidth_cpoly

        else:
            raise Error( 1, 'getMinimumCapWidth() : Unsupported capacitor type : %s.' % self.capacitorType ) 

        return minimumCapWidth


    ## Loads the technology file then extracts the adequate layers according to the capacitor type (MIM or PIP). 
    #
    #  \return   a dictionary containing the layer labels as attributes and its values.   
    #  \remarks  An exception is raised if the entered capacitor type is unknown.

    def getLayers( self ):

        technology             =   DataBase.getDB().getTechnology()
        layerDict = {}

        if   self.capacitorType  == 'MIMCap':

            layerDict["topPlateLayer"      ] =   technology.getLayer( "metcap" )
            layerDict["bottomPlateLayer"   ] =   technology.getLayer( "metal2" )
            layerDict["topPlateRLayer"     ] =   technology.getLayer( "metal3" )
            layerDict["bottomPlateRLayer"  ] =   technology.getLayer( "metal3" )  
            layerDict["topBottomcutsLayer" ] =   technology.getLayer( "cut2"   )

        elif self.capacitorType == 'PIPCap':

            layerDict["topPlateLayer"      ] =   technology.getLayer( "poly2"  )
            layerDict["bottomPlateLayer"   ] =   technology.getLayer( "poly"   )
            layerDict["topPlateRLayer"     ] =   technology.getLayer( "metal1" )
            layerDict["bottomPlateRLayer"  ] =   technology.getLayer( "metal1" )  
            layerDict["topBottomcutsLayer" ] =   technology.getLayer( "cut0"   )

        else : 
            raise Error( 1, 'getLayers() : Unsupported capacitor type : %s.' % self.capacitorType ) 

        return layerDict


    ## When bonding box mode is activated, the function draws all layout physical layers of the capacitor after checking its dimensions. All functions are excecuted in a new Update Session. In the contrary, only an exact estimation of layout dimensions is given. An error is raised when dimensions reach technological limits for MIM and PIP capacitors or when \c bbMode parameters is other than \c True or \c False.
    #  
    #  \param    ( tNet , bNet )    nets of top and bottom plates, respectively
    #  \param      bbMode                            activates bonding box dimensions computing when set to \c True
    def create( self, tNet, bNet, bbMode = False ):
   
        UpdateSession.open()           

        abutmentBoxDimensions = None
        self.setRules                      ()

        if  self.__isCapacitorUnitOK__( self.capDim ) == True :        

            self.computeDimensions    ( self.capDim )
            self.drawAbutmentBox      ()
            self.device.setAbutmentBox( self.abutmentBox )

            if   bbMode == True :
                abutmentBoxDimensions = self.abutmentBoxDict        

            elif bbMode == False :
                layerDict          = self.getLayers()
                self.drawCapacitor( layerDict, tNet, bNet )

            else :raise Error(1, 'drawCapacitor(): The bonding box mode parameter, "bbMode" must be either True or False : %s.' %bbMode )

        else : raise Error(1, 'creat(): Impossible to draw the capacitor, dimensions are either too large or too small, "%s".' % self.capDim )

        UpdateSession.close()
   
        return abutmentBoxDimensions



    ## Draws all layout physicial layers of the capacitor. 
    #
    #  \param      layerDict                             a dictionary containing a description of the required physical layers according to capacitor type
    #  \param    ( tNet , bNet )    nets of top and bottom plates, respectively
    def drawCapacitor( self, layerDict, tNet, bNet ):


        self.bottomPlateBox     = self.drawOnePlate( layerDict["bottomPlateLayer"  ] , bNet                        , self.bottomPlateBoxDict        )   
        self.topPlateBox        = self.drawOnePlate( layerDict["topPlateLayer"     ] , tNet                        , self.topPlateBoxDict           )  
        self.drawBottomPlateCut                    ( layerDict["topBottomcutsLayer"] , bNet                                                         )
        self.drawTopPlateCut                       ( layerDict["topBottomcutsLayer"] , tNet                                                         )
        self.drawRoutingLayers                     ( layerDict["bottomPlateRLayer" ] , layerDict["topPlateRLayer"] , tNet                    , bNet )

        return 



    ## Computes needed parameters to draw bottom plate cuts in its exact position, including :
    #
    #  - maximum number of cuts to draw on both sides of bottom plate,
    #  - adjusted enclosure of 
    #  - abcissas of the two bottom cuts on left and right sides of bottom plate,
    #  - ordinate of the same two cuts.
    #
    #  Given parameters described above, it is possible to draw the entire lines of cuts on both sides of bottom plate using \c cutLine function. 
    def computeBottomPlateCuts( self ):

        self.bottomCutLineNumber       =   self.cutMaxNumber( self.bottomPlateBoxDict["height"], self.minheight_topPlatecut, self.minSpacingOnBotPlate_cut, self.minEnclo_botPlate_botPlateCut )
        enclosure_botPlate_botPlateCut = ( self.bottomPlateBoxDict["height"] - (self.bottomCutLineNumber * self.minheight_topPlatecut + (self.bottomCutLineNumber -1) * self.minSpacingOnBotPlate_cut) )/2

        self.cutLeftLineDict ["XMin"]  = self.topPlateBoxDict["XMin"] - self.minSpacing_botPlateCut_topPlate - self.minWidth_topPlatecut/2   
        self.cutRightLineDict["XMin"]  = self.topPlateBoxDict["XMin"] + self.topPlateBoxDict["width"] + self.minSpacing_botPlateCut_topPlate + self.minWidth_topPlatecut/2 

        self.cutLeftLineDict ["YMin"]  = self.bottomPlateBoxDict["YMin"] + self.minheight_topPlatecut/2 + enclosure_botPlate_botPlateCut
        self.cutRightLineDict["YMin"]  = self.cutLeftLineDict["YMin"]  

        return


    ## Computes needed parameters to draw top plate cuts in its exact position, including :
    #
    #  - maximum number of cuts to draw on both sides of top plate,
    #  - adjusted enclosure of 
    #  - abcissas of the two top cuts on left and right sides of top plate,
    #  - ordinate of the same two cuts.
    #
    #  Given parameters described above, it is possible to draw the entire lines of cuts on both sides of bottom plate using \c cutLine function. 
    def computeTopPlateCuts( self ):

        self.topCutLineNumber              = self.cutMaxNumber( self.topPlateBoxDict["height"], self.minheight_topPlatecut, self.minSpacingOnTopPlate_cut, self.minEnclo_topPlate_topPlateCut )
        enclosure_topPlate_topPlateCut     = ( self.topPlateBoxDict["height"] - (self.topCutLineNumber *self.minheight_topPlatecut + (self.topCutLineNumber -1)*self.minSpacingOnTopPlate_cut) )/2


        if  self.capacitorType == 'MIMCap':

            self.cut2MatrixDict[ "Width" ] = (self.topCutLineNumber - 2)*self.minWidth_topPlatecut  + (self.topCutLineNumber - 1) * self.minSpacingOnTopPlate_cut 
            cut2MatrixAttribute            = ["XMin", "YMin" ]
            topPlateBoxCordinates          = [self.topPlateBoxDict["XMin"], self.topPlateBoxDict["YMin"]]

            for i in range(2):
                topPlateCutTab                                = [ self.minWidth_topPlatecut, self.minheight_topPlatecut ]
                self.cut2MatrixDict[ cut2MatrixAttribute[i] ] =   topPlateBoxCordinates[i]  + topPlateCutTab[i] /2 + enclosure_topPlate_topPlateCut 

        elif self.capacitorType == 'PIPCap':
            self.topCutLineDict["XMin"] = self.topPlateBoxDict["XMin"] + self.topPlateBoxDict["width"] - self.minEnclo_topPlate_topPlateCut - self.minWidth_topPlatecut/2  
            self.topCutLineDict["YMin"] = self.topPlateBoxDict["YMin"] + self.minheight_topPlatecut/2       + enclosure_topPlate_topPlateCut

        else : 
            raise Error( 1, 'computeTopPlateCuts() : Unsupported capacitor type : %s. Cuts cannot be drawn. ' % self.capacitorType ) 


        return


    def computeRoutingLayersDimensions( self ):

        if self.capacitorType == 'MIMCap' : 

            self.cut2MatrixDict  ["XMax"]         =   self.cut2MatrixDict["XMin"] + self.cut2MatrixDict["Width"]             + self.minWidth_topPlatecut 
            [ topmetalXMin,topmetalXMax ]         = [ self.cut2MatrixDict["XMin"] - self.minEnclo_topPlateRMetal_topPlateCut - self.minWidth_topPlatecut/2, self.cut2MatrixDict["XMax"] + self.minEnclo_topPlateRMetal_topPlateCut + self.minWidth_topPlatecut/2]
            [ width_topMetal, width_bottomMetal ] = [ topmetalXMax - topmetalXMin, max( self.minWidth_botRMetal, (self.minWidth_botPlatecut + 2*self.minEnclo_botPlateRMetal_botPlateCut) ) ]
            topMetalXCenter                       = self.abutmentBoxDict["XMin"] + self.abutmentBoxDict["width"]/2 

        elif self.capacitorType == 'PIPCap' :

            topMetalXCenter                       = self.topCutLineDict["XMin"] 
            width_topMetal                        = max( self.minWidth_botRMetal, (self.minWidth_topPlatecut+ 2*self.minEnclo_topPlateRMetal_topPlateCut), ( self.minWidth_routingTrackcut +2*self.minEnclo_routingTrackMetal_cut) )
            width_bottomMetal                     = width_topMetal 
      
        else : 
            raise Error( 1, 'drawRoutingLayers() : Unsupported capacitor type : %s. Routing layers parameters cannot be computed. ' % self.capacitorType ) #com verirfy the two next lines are after else or before

        [ dySourceTop   , dyTargetTop    ]  = [ self.bottomPlateBoxDict["YMin"], self.topPlateBoxDict["YMin"] + self.topPlateBoxDict["height"] ]
        [ dySourceBottom, dyTargetBottom ]  = [ self.bottomPlateBoxDict["YMin"], self.bottomPlateBoxDict["YMin"]+self.bottomPlateBoxDict["height"] ] 

        self.topPlateRLayerDict             = { "YMin" : dySourceTop, "YMax" : dyTargetTop, "XCenter" : topMetalXCenter , "width" : width_topMetal }
        self.bottomPlateRLayerDict          = { "left" : {"XMin": self.cutLeftLineDict["XMin"] - ( self.minEnclo_botPlate_botPlateCut + self.minWidth_botPlatecut/2 ), "XMax": self.cutLeftLineDict["XMin"] + ( self.minEnclo_botPlate_botPlateCut + self.minWidth_botPlatecut/2 )} , "right" : {"XMin": self.cutRightLineDict["XMin"] - ( self.minEnclo_botPlate_botPlateCut + self.minWidth_botPlatecut/2 )}, "YMin": dySourceBottom, "YMax": dyTargetBottom, "width": width_bottomMetal }

        return



    def computeAbutmentBoxDimensions( self, capDim ) :


        self.enclosure_botPlate_topPlate = self.minEnclo_botPlate_botPlateCut + self.minWidth_topPlatecut + self.minSpacing_botPlateCut_topPlate
        abutmentBoxDimensions = {}
        for key in capDim.keys():
            abutmentBoxDimensions[key] = 2*self.enclosure_botPlate_topPlate + capDim[key] + 2*self.minSpacing_botPlate
        abutmentBoxDimensions["surface"] = numpy.prod(abutmentBoxDimensions.values())

        return abutmentBoxDimensions


    ##  Draws the abutment box of the capacitor in position \c <(abutmentBoxXMin, abutmentBoxYMin)>. First, the minimum enclosure of the top plate inside the bottom plate is computed. Second, using this parameters as well as the capacitor dimensions, the width and height of the abutment box are computed. The box is finally drawn.

    def drawAbutmentBox( self ):

        self.abutmentBox      = Box(self.abutmentBoxDict["XMin"],self.abutmentBoxDict["YMin"],self.abutmentBoxDict["width"]+self.abutmentBoxDict["XMin"],self.abutmentBoxDict["height"]+self.abutmentBoxDict["YMin"])

        return


    def computeOnePlateBoxDimensions( self, inputBoxDimensions, enclosure ):

        outputboxDimensions = {}
        outputboxDimensions["XMin"  ] = inputBoxDimensions["XMin"  ] +   enclosure 
        outputboxDimensions["YMin"  ] = inputBoxDimensions["YMin"  ] +   enclosure 
        outputboxDimensions["width" ] = inputBoxDimensions["width" ] - 2*enclosure 
        outputboxDimensions["height"] = inputBoxDimensions["height"] - 2*enclosure

        return outputboxDimensions



    def computeDimensions( self, capDim ):

        abutmentBoxDimensions = self.computeAbutmentBoxDimensions(capDim)
        for key in abutmentBoxDimensions.keys():
            self.abutmentBoxDict[key] = abutmentBoxDimensions[key]

        self.bottomPlateBoxDict = self.computeOnePlateBoxDimensions( self.abutmentBoxDict   , self.minSpacing_botPlate         )
        self.topPlateBoxDict    = self.computeOnePlateBoxDimensions( self.bottomPlateBoxDict, self.enclosure_botPlate_topPlate )
        self.computeBottomPlateCuts()
        self.computeTopPlateCuts()
        self.computeRoutingLayersDimensions()

        return

    ##  Draws the top or bottom plate through inflation of the Box under it. These boxes are the abutment box in the case of the bottom plate and the bottom plate's box in the case of the top plate. This function also creates a a net for the drawn plate and sets it as external.  
    #   \return The drawn box.

   #def drawOnePlate( self, layer, net, inputPlateBox, depth ):

   #    outputPlateBox = Box( inputPlateBox.getXMin(), inputPlateBox.getYMin(), inputPlateBox.getXMax(), inputPlateBox.getYMax() ).inflate( -depth ) 
   #    platePad       = Pad.create( net, layer, outputPlateBox ) 
   #    NetExternalComponents.setExternal( platePad )
   #    return outputPlateBox


    def drawOnePlate( self, layer, net, boxDimensions) : 

        outputPlateBox = Box( boxDimensions["XMin"], boxDimensions["YMin"], boxDimensions["XMin"] + boxDimensions["width"], boxDimensions["YMin"] + boxDimensions["height"] ) 
        print("outputPlate",outputPlateBox)
        print("net",net)
        platePad       = Pad.create( net, layer, outputPlateBox ) 
        NetExternalComponents.setExternal( platePad )

        return outputPlateBox


    ## Draws the required cuts to connect the bottom plate. First, the maximal possible number of cuts that can be drawn is computed. Second, using the computed number, the enclosure of this cuts in the bottom plate's layer is adjusted while the minimal enclosure is respected. Third, the relative positions of the cuts on both sides of the plate are computed. Finally, two vertical lines of cuts are drawns.
    #  \remark The relative positions describe the cordinates of the first bottom cut in every line of cuts. Then, knowing the spacing and width specifications of these cuts the rest of the line is easilly constructed.

    def drawBottomPlateCut( self, layer, bNet ):
      
        self.cutLine( bNet, layer, self.cutLeftLineDict ["XMin"], self.cutLeftLineDict ["YMin"], self.minWidth_botPlatecut, self.minheight_topPlatecut, self.minSpacingOnBotPlate_cut, self.bottomCutLineNumber , 'vertical' )
        self.cutLine( bNet, layer, self.cutRightLineDict["XMin"], self.cutRightLineDict["YMin"], self.minWidth_botPlatecut, self.minheight_topPlatecut, self.minSpacingOnBotPlate_cut, self.bottomCutLineNumber , 'vertical' )
        
        return


    ## Draws the top plate's cuts after computing the maximal number of cuts that can be placed and its exact enclosure in the top plate.  

    def drawTopPlateCut( self, layer, tNet ):

        if  self.capacitorType == 'MIMCap':
            self.cutMatrix(tNet,layer,self.cut2MatrixDict["XMin"],self.cut2MatrixDict["YMin"],self.minWidth_topPlatecut,self.minheight_topPlatecut,self.minSpacingOnTopPlate_cut,self.topCutLineNumber,self.topCutLineNumber)
 
        else : self.cutLine  (tNet,layer,self.topCutLineDict["XMin"],self.topCutLineDict["YMin"],self.minWidth_topPlatecut,self.minheight_topPlatecut,self.minSpacingOnTopPlate_cut,self.topCutLineNumber,'vertical')

        return


    ## Draws the routing layers of both bottom and top plates after computing widths and the exact position of these layers. Also computes positions if rlayers that are crucual for routing. this of putting it in a second function 

    def drawRoutingLayers( self, bottomPlateLayer, topPlateLayer, tNet, bNet ):

        Vertical.create ( tNet, topPlateLayer, self.topPlateRLayerDict["XCenter"], self.topPlateRLayerDict["width"], self.topPlateRLayerDict["YMin"], self.topPlateRLayerDict["YMax"] ) 

        cutLinesXMins = [ self.cutLeftLineDict["XMin"], self.cutRightLineDict["XMin"] ]
        for i in range(2):
            Vertical.create ( bNet, bottomPlateLayer, cutLinesXMins[i] , self.bottomPlateRLayerDict["width"], self.bottomPlateRLayerDict["YMin"], self.bottomPlateRLayerDict["YMax"] )


        return 


    ## Computes the maximal number of cuts to be placed on a layer of width \c width_layer considering specifications such as the spacing between the cuts, its width and its enclosure in the layer.

    def cutMaxNumber( self, width_layer, width_cut, spacing_cut, enclosure_cut ): 

        cutNumber = int( (width_layer - 2*enclosure_cut + spacing_cut) / (width_cut + spacing_cut) )
        if cutNumber > 0 : 
            return cutNumber
        else : raise Error (1,"cutMaxNumber() : Zero number of cuts found. Layer width is too tight." )



    ## Creates a horizontal or vertical line of contacts according to the specified direction.

    def cutLine( self, net, layer, firstCutXCenter, firstCutYCenter, width_cut, height_cut, spacing_cut, cutNumber, direction ):       
 
        for i in range( cutNumber) :
            segment = Contact.create( net, layer, firstCutXCenter + i*(width_cut + spacing_cut), firstCutYCenter, width_cut, height_cut ) if direction == 'horizontal' else Contact.create( net, layer, firstCutXCenter, firstCutYCenter + i*(height_cut + spacing_cut), width_cut, height_cut )

        return segment


    ## Creates a matrix of cuts by vertically stacking horizontal lines of identical cuts.
    #
    #  \param    net                                  net to which the cuts belong
    #  \param    layer                                cuts physical layer
    #  \param    firstCutXCenter                      center's abcissa of the bottom left cut ( that is the first cut to be drawn in the matrix ) 
    #  \param    firstCutYCenter                      center's abcissa of the bottom left cut
    #  \param    (width_cut,height_cut,spacing_cut)   cuts dimenions 
    #  \param    (cutColumnNumber,cutRowNumber)       matrix dimensions
    #
    #  \remarks  The matrix can have any dimensions zero or negative one. 

    def cutMatrix( self, net, layer, firstCutXCenter, firstCutYCenter, width_cut, height_cut, spacing_cut, cutColumnNumber, cutRowNumber ):

        for i in range( cutRowNumber):
            matrix = self.cutLine( net, layer, firstCutXCenter, firstCutYCenter + i*(spacing_cut + width_cut), width_cut, height_cut, spacing_cut, cutColumnNumber, 'horizontal' )

        return matrix



    ## \return the ordinate of the bottom plate's highest end-point ( that is equivalent to \c dySource of the bottom plate's box ) .   
    def getBottomPlateYMax         ( self ) : return self.bottomPlateBoxDict["YMin"] + self.bottomPlateBoxDict["height"]


    ## \return the abcissa of the bottom plate's left line of cuts.   
    def getBottomPlateLeftCutXMin  ( self ) : return self.cutLeftLineDict      ["XMin"   ] 


    ## \return the ordinate of the first ( or bottom) cut in the left line of cuts on the bottom plate.
    def getBottomPlateLeftCutYMin  ( self ) : return self.cutLeftLineDict      ["YMin"   ]


    ## \return the ordinate of the highest cut of the bottom plate's left line of cuts.   
    def getBottomPlateLeftCutYMax  ( self ) : return self.cutLeftLineDict      ["YMin"   ] +  ( self.topCutLineNumber - 1 )*( self.minSpacingOnBotPlate_cut + self.minWidth_botPlatecut )


    ## \return the absissa of the bottom plate's right line of cuts.  
    def getBottomPlateRightCutXMin ( self ) : return self.cutRightLineDict     ["XMin"   ]


    ## \return the ordinate of the first ( or bottom) cut in the right line of cuts on the bottom plate.
    def getBottomPlateRightCutYMin ( self ) : return self.cutRightLineDict     ["YMin"   ]


    ## \return the ordinate of the highest ( or top) cut in the right line of cuts on the bottom plate.
    def getBottomPlateRightCutYMax ( self ) : return self.cutRightLineDict     ["YMin"   ] +  ( self.bottomCutLineNumber - 1 )*( self.minSpacingOnBotPlate_cut + self.minWidth_botPlatecut )


    ## \return the center's ordinate of the bottom plate's left cut (the cut that is the first one in the line).
    def getBotPlateLeftRLayerXMax      ( self ) : return self.bottomPlateRLayerDict["left"   ]["XMax" ]


    ## \return the position of the bottom plate's right cuts on the horitontal axis (also applicable to left cuts).
    def getBottomPlateRightCutYCenter  (self ) : return (self.getBottomPlateRightCutYMax() - self.getBottomPlateRightCutYMin())/2 


     ## \return the position of the bottom plate's right cuts on the horitontal axis.
    def getBotPlateRightRLayerXMin    ( self ) : return self.bottomPlateRLayerDict ["right"  ]["XMin" ]
 

  ## \return the position of the bottom plate's left cuts on the horitontal axis.
    def getBotPlateLeftRLayerXMin    ( self ) : return self.bottomPlateRLayerDict ["left"  ]["XMin" ]
        

    ## \return the position of bottom plate's left cuts on the horitontal axis.
    def getBotPlateRLayerYMin         ( self ) : return self.bottomPlateRLayerDict ["YMin"   ]


    ## \return the position of bottom plate's left cuts on the horitontal axis.
    def getBotPlateRLayerYMax         ( self ) : return self.bottomPlateRLayerDict ["YMax"   ]


    ## \return the position of bottom plate's left cuts on the horitontal axis.
    def getBotPlateRLayerWidth        ( self ) : return self.bottomPlateRLayerDict ["width"  ]


    ## \return the position of bottom plate's left cuts on the horitontal axis.
    def getBotPlateRightRLayerXCenter ( self ) : return self.cutRightLineDict      ["XMin"   ]


    ## \return the position of bottom plate's left cuts on the horitontal axis.
    def getBotPlateLeftRLayerXCenter  ( self ) : return self.cutLeftLineDict       ["XMin"   ]


    ## \return the ordinate of the bottom end points of the top plate routing layer.
    def getTopPlateRLayerYMin         ( self ) : return  self.topPlateRLayerDict   ["YMin"   ]


    ## \return the ordinate of the higher end points of the top plate routing layer.
    def getTopPlateRLayerYMax         ( self ) : return  self.topPlateRLayerDict   ["YMax"   ]


    ## \return the width of top plate's routing layer.
    def getTopPlateRLayerWidth        ( self ) : return  self.topPlateRLayerDict   ["width"  ]


    ## \return the center's abcissa of the bottom plate routing layer.
    def getTopPlateRLayerXCenter      ( self ) : return  self.topPlateRLayerDict   ["XCenter"]


    ## \return the origin (bottom-left end point) abcissa of the top plate routing layers.
    def getTopPlateRLayerXMin         ( self ) : return  self.topPlateRLayerDict   ["XCenter"] - self.topPlateRLayerDict   ["width"]/2


    ## \return the abscissa of the bottom-right end-point of the top plate routing layer.
    def getTopPlateRLayerXMax   ( self ) : return  self.topPlateRLayerDict         ["XCenter"] + self.topPlateRLayerDict   ["width"]/2

   


def ScriptMain( **kw ):

    editor = None
    if kw.has_key('editor') and kw['editor']:
        editor = kw['editor']

    UpdateSession.open()
    device = AllianceFramework.get().createCell( 'capacitor' )
    device.setTerminal( True )

    bottomPlate_net = Net.create( device, 'bNet' )
    bottomPlate_net.setExternal( True )  
    bNet = device.getNet("bNet")

    topPlate_net    = Net.create( device, 'tNet' )
    topPlate_net.setExternal( True )  
    tNet = device.getNet("tNet")

    if editor:
        UpdateSession.close(        )
        editor.setCell     ( device ) 
        editor.fit         (        )            
        UpdateSession.open (        )     
                      
    capacitor = CapacitorUnit   ( device,500, 'MIMCap', [0,0] ) # 129.56
    surface   = capacitor.create( bNet, tNet )          
    print(surface)
                      
    AllianceFramework.get().saveCell( device, Catalog.State.Views )

    return True
            
                            
                             
                              
                              
                             
                              
                              
                              
                             
                              
                              
                              
                              
                              
                             
                              
                              
                              
                              
                              


// -*- C++ -*-
//
// This file is part of the Coriolis Project.
// Copyright (C) Laboratoire LIP6 - Departement ASIM
// Universite Pierre et Marie Curie
//
// Main contributors :
//        Christophe Alexandre   <Christophe.Alexandre@lip6.fr>
//        Sophie Belloeil             <Sophie.Belloeil@lip6.fr>
//        Hugo Clément                   <Hugo.Clement@lip6.fr>
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
//
// +-----------------------------------------------------------------+ 
// |                   C O R I O L I S                               |
// |    I s o b a r  -  Hurricane / Python Interface                 |
// |                                                                 |
// |  Author      :                    Jean-Paul Chaput              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Module  :  "./PyLayer.cpp"                                 |
// +-----------------------------------------------------------------+


#include "hurricane/isobar/PyTechnology.h"
#include "hurricane/isobar/PyLayer.h"
#include "hurricane/isobar/PyLayerMask.h"
#include "hurricane/isobar/PyBasicLayer.h"
#include "hurricane/isobar/PyBasicLayerCollection.h"


namespace  Isobar {

using namespace Hurricane;

extern "C" {


#define  METHOD_HEAD(function)   GENERIC_METHOD_HEAD(Layer,layer,function)


// +=================================================================+
// |               "PyLayer" Python Module Code Part                 |
// +=================================================================+

#if defined(__PYTHON_MODULE__)


  // +-------------------------------------------------------------+
  // |                "PyLayer" Attribute Methods                  |
  // +-------------------------------------------------------------+


# define  accessorDbuFromOptBasicLayer(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)                \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self, PyObject* args )    \
  {                                                                                     \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                                 \
                                                                                        \
    DbU::Unit  rvalue = 0;                                                              \
                                                                                        \
    HTRY                                                                                \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")                  \
                                                                                        \
    PyObject* arg0 = NULL;                                                              \
                                                                                        \
    __cs.init (#SELF_TYPE"."#FUNC_NAME"()");                                            \
    if (PyArg_ParseTuple( args, "|O&:"#SELF_TYPE"."#FUNC_NAME"()", Converter, &arg0)) { \
      if ( __cs.getObjectIds() == ":basiclayer" )                                       \
        rvalue = cobject->FUNC_NAME( PYBASICLAYER_O(arg0) );                            \
      else if  ( __cs.getObjectIds() == "" )                                            \
        rvalue = cobject->FUNC_NAME();                                                  \
      else {                                                                            \
        PyErr_SetString ( ConstructorError                                              \
                        , "invalid parameter type for "#SELF_TYPE"."#FUNC_NAME"()." );  \
        return NULL;                                                                    \
      }                                                                                 \
    } else {                                                                            \
      PyErr_SetString ( ConstructorError                                                \
                      , "Invalid number of parameters passed to "#SELF_TYPE"."#FUNC_NAME"()." ); \
      return NULL;                                                                      \
    }                                                                                   \
    HCATCH                                                                              \
                                                                                        \
    return Py_BuildValue("l",rvalue);                                                   \
  }


# define  accessorLayerFromLayer(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)                  \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self, PyObject* args )     \
  {                                                                                      \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                                  \
                                                                                         \
    Layer* rlayer = 0;                                                                   \
                                                                                         \
    HTRY                                                                                 \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")                   \
                                                                                         \
    PyObject* arg0 = NULL;                                                               \
                                                                                         \
    if (PyArg_ParseTuple( args, "O:"#SELF_TYPE"."#FUNC_NAME"()", &arg0)) {               \
      Layer* argLayer = PYLAYER_O(arg0);                                                 \
      if (argLayer == NULL) {                                                            \
        PyErr_SetString ( ConstructorError                                               \
                        , #SELF_TYPE"."#FUNC_NAME"(): Parameter is not of Layer type" ); \
        return NULL;                                                                     \
      }                                                                                  \
      rlayer = const_cast<Layer*>(cobject->FUNC_NAME( argLayer ));                       \
    } else {                                                                             \
      PyErr_SetString ( ConstructorError                                                 \
                      , #SELF_TYPE"."#FUNC_NAME"(): Parameter is not of Layer type" );   \
      return NULL;                                                                       \
    }                                                                                    \
    HCATCH                                                                               \
                                                                                         \
    return PyLayer_Link(rlayer);                                                         \
  }


# define  accessorLayerFromVoid(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)  \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self )     \
  {                                                                      \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                  \
                                                                         \
    Layer* rlayer = NULL;                                                \
                                                                         \
    HTRY                                                                 \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")   \
    rlayer = const_cast<SELF_TYPE*>(cobject->FUNC_NAME());               \
    HCATCH                                                               \
                                                                         \
    if (rlayer == NULL) Py_RETURN_NONE;                                  \
    return PyLayer_Link(rlayer);                                         \
  }


# define  accessorMaskFromVoid(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)  \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self )     \
  {                                                                      \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                  \
                                                                         \
    Layer::Mask  mask = 0;                                               \
                                                                         \
    HTRY                                                                 \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")   \
    mask = cobject->FUNC_NAME();                                         \
    HCATCH                                                               \
                                                                         \
    return PyLayerMask_Link(mask);                                       \
  }


# define  accessorLayerFromOptBool(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)              \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self, PyObject* args )    \
  {                                                                                     \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                                 \
                                                                                        \
    Layer* rlayer = NULL;                                                               \
                                                                                        \
    HTRY                                                                                \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")                  \
                                                                                        \
    PyObject* arg0       = NULL;                                                        \
    bool      useWorking = true;                                                        \
                                                                                        \
    if (PyArg_ParseTuple( args, "|O:"#SELF_TYPE"."#FUNC_NAME"()", &arg0)) {             \
      if (arg0 != NULL) {                                                               \
        useWorking = PyObject_IsTrue(arg0);                                             \
      }                                                                                 \
    } else {                                                                            \
      PyErr_SetString ( ConstructorError                                                \
                      , "Invalid number of parameters passed to "#SELF_TYPE"."#FUNC_NAME"()." );  \
      return NULL;                                                                      \
    }                                                                                   \
    rlayer = const_cast<SELF_TYPE*>(cobject->FUNC_NAME(useWorking));                    \
    HCATCH                                                                              \
                                                                                        \
    if (rlayer == NULL) Py_RETURN_NONE;                                                 \
    return PyLayer_Link(rlayer);                                                        \
  }


# define  predicateFromVoid(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)     \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self )   \
  {                                                                    \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                \
                                                                       \
    HTRY                                                               \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()") \
    if (cobject->FUNC_NAME()) Py_RETURN_TRUE;                          \
    HCATCH                                                             \
                                                                       \
    Py_RETURN_FALSE;                                                   \
  }


# define  updatorFromDbu(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)                               \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self, PyObject* args )     \
  {                                                                                      \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                                  \
                                                                                         \
    HTRY                                                                                 \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")                   \
                                                                                         \
    long dimension = 0;                                                                  \
                                                                                         \
    if (PyArg_ParseTuple( args, "l:"#SELF_TYPE"."#FUNC_NAME"()", &dimension)) {          \
      cobject->FUNC_NAME( dimension );                                                   \
    } else {                                                                             \
      PyErr_SetString ( ConstructorError                                                 \
                      , #SELF_TYPE"."#FUNC_NAME"(): Parameter is not of long (DbU) type" ); \
      return NULL;                                                                       \
    }                                                                                    \
    HCATCH                                                                               \
                                                                                         \
    Py_RETURN_NONE;                                                                      \
  }


# define  updatorFromBasicLayerDbu(FUNC_NAME,PY_SELF_TYPE,SELF_TYPE)                     \
  static PyObject* PY_SELF_TYPE##_##FUNC_NAME ( PY_SELF_TYPE* self, PyObject* args )     \
  {                                                                                      \
    trace << #PY_SELF_TYPE "_" #FUNC_NAME "()" << endl;                                  \
                                                                                         \
    HTRY                                                                                 \
    GENERIC_METHOD_HEAD(SELF_TYPE,cobject,#SELF_TYPE"."#FUNC_NAME"()")                   \
                                                                                         \
    PyObject* pyBasicLayer = NULL;                                                       \
    long      dimension    = 0;                                                          \
                                                                                         \
    if (PyArg_ParseTuple( args, "Ol:"#SELF_TYPE"."#FUNC_NAME"()", &pyBasicLayer, &dimension)) { \
      BasicLayer* layer = PYBASICLAYER_O(pyBasicLayer);                                  \
      if (layer == NULL) {                                                               \
        PyErr_SetString ( ConstructorError                                               \
                        , #SELF_TYPE"."#FUNC_NAME"(): First parameter is not of BasicLayer type" ); \
        return NULL;                                                                     \
      }                                                                                  \
      cobject->FUNC_NAME( layer, dimension );                                            \
    } else {                                                                             \
      PyErr_SetString ( ConstructorError                                                 \
                      , #SELF_TYPE"."#FUNC_NAME"(): Bad parameters types or numbers." ); \
      return NULL;                                                                       \
    }                                                                                    \
    HCATCH                                                                               \
                                                                                         \
    Py_RETURN_NONE;                                                                      \
  }


  static PyObject* PyLayer_getTechnology ( PyLayer *self ) {
    trace << "PyLayer_getTechnology ()" << endl;

    Technology* techno = NULL;

    HTRY
    METHOD_HEAD ( "Layer.getTechnology()" )
    techno = layer->getTechnology ();
    HCATCH
    
    return PyTechnology_Link(techno);
  }


  GetNameMethod               (Layer, layer)
  predicateFromLayer          (                          above            ,PyLayer,Layer)
  predicateFromLayer          (                          below            ,PyLayer,Layer)
  predicateFromLayer          (                          contains         ,PyLayer,Layer)
  predicateFromLayer          (                          intersect        ,PyLayer,Layer)
  predicateFromVoid           (                          isWorking        ,PyLayer,Layer)
  accessorDbuFromOptBasicLayer(                          getEnclosure     ,PyLayer,Layer)
  accessorDbuFromOptBasicLayer(                          getExtentionCap  ,PyLayer,Layer)
  accessorDbuFromOptBasicLayer(                          getExtentionWidth,PyLayer,Layer)
  accessorCollectionFromVoid  (                          getBasicLayers   ,PyLayer,Layer,BasicLayer)
  accessorMaskFromVoid        (                          getMask          ,PyLayer,Layer)
  accessorMaskFromVoid        (                          getExtractMask   ,PyLayer,Layer)
  accessorLayerFromVoid       (                          getBlockageLayer ,PyLayer,Layer)
  accessorLayerFromVoid       (                          getTop           ,PyLayer,Layer)
  accessorLayerFromVoid       (                          getBottom        ,PyLayer,Layer)
  accessorLayerFromLayer      (                          getOpposite      ,PyLayer,Layer)
  accessorLayerFromOptBool    (                          getMetalAbove    ,PyLayer,Layer)
  accessorLayerFromOptBool    (                          getMetalBelow    ,PyLayer,Layer)
  accessorLayerFromOptBool    (                          getCutAbove      ,PyLayer,Layer)
  accessorLayerFromOptBool    (                          getCutBelow      ,PyLayer,Layer)
  DirectGetLongAttribute      (PyLayer_getMinimalSize   ,getMinimalSize   ,PyLayer,Layer)
  DirectGetLongAttribute      (PyLayer_getMinimalSpacing,getMinimalSpacing,PyLayer,Layer)

  SetNameMethod(Layer, layer)
  updatorFromDbu          (setMinimalSize   ,PyLayer,Layer)
  updatorFromDbu          (setMinimalSpacing,PyLayer,Layer)
  updatorFromBasicLayerDbu(setEnclosure     ,PyLayer,Layer)
  updatorFromBasicLayerDbu(setExtentionCap  ,PyLayer,Layer)
  updatorFromBasicLayerDbu(setExtentionWidth,PyLayer,Layer)

  // Standart destroy (Attribute).
  DBoDestroyAttribute(PyLayer_destroy, PyLayer)


  PyMethodDef PyLayer_Methods[] =
    { { "getTechnology"       , (PyCFunction)PyLayer_getTechnology       , METH_NOARGS
                              , "Returns the technology owning the layer." }
    , { "getName"             , (PyCFunction)PyLayer_getName             , METH_NOARGS
                              , "Returns the name of the layer." }
    , { "getMask"             , (PyCFunction)PyLayer_getMask             , METH_NOARGS
                              , "Returns the mask (bits) of the layer." }
    , { "getExtractMask"      , (PyCFunction)PyLayer_getExtractMask      , METH_NOARGS
                              , "Returns the extract mask of the layer (for GDSII)." }
    , { "getMinimalSize"      , (PyCFunction)PyLayer_getMinimalSize      , METH_NOARGS
                              , "Returns the minimum width allowed for the layer." }
    , { "getMinimalSpacing"   , (PyCFunction)PyLayer_getMinimalSpacing   , METH_NOARGS
                              , "Returns the spacing allowed for the layer (edge to edge)." }
    , { "getBasicLayers"      , (PyCFunction)PyLayer_getBasicLayers      , METH_NOARGS
                              , "Returns the collection of BasicLayer the Layer is built upon." }
    , { "getBlockageLayer"    , (PyCFunction)PyLayer_getBlockageLayer    , METH_NOARGS
                              , "Returns the associated connector layer." }
    , { "getTop"              , (PyCFunction)PyLayer_getTop              , METH_NOARGS
                              , "Returns the top layer (in case of a muti-layer)." }
    , { "getBottom"           , (PyCFunction)PyLayer_getBottom           , METH_NOARGS
                              , "Returns the bottom layer (in case of a muti-layer)." }
    , { "getOpposite"         , (PyCFunction)PyLayer_getOpposite         , METH_VARARGS
                              , "Returns the layer opposite the one given." }
    , { "getMetalAbove"       , (PyCFunction)PyLayer_getMetalAbove       , METH_VARARGS
                              , "Returns the metal layer above this one." }
    , { "getMetalBelow"       , (PyCFunction)PyLayer_getMetalBelow       , METH_VARARGS
                              , "Returns the metal layer below this one." }
    , { "getCutAbove"         , (PyCFunction)PyLayer_getCutAbove         , METH_VARARGS
                              , "Returns the cut layer above this one." }
    , { "getCutBelow"         , (PyCFunction)PyLayer_getCutBelow         , METH_VARARGS
                              , "Returns the cut layer below this one." }
    , { "getEnclosure"        , (PyCFunction)PyLayer_getEnclosure        , METH_VARARGS
                              , "Returns the enclosure (global or for one BasicLayer)." }
    , { "getExtentionCap"     , (PyCFunction)PyLayer_getExtentionCap     , METH_VARARGS
                              , "Returns the extention cap (global or for one BasicLayer)." }
    , { "getExtentionWidth"   , (PyCFunction)PyLayer_getExtentionWidth   , METH_VARARGS
                              , "Returns the extention width (global or for one BasicLayer)." }
    , { "above"               , (PyCFunction)PyLayer_above               , METH_VARARGS
                              , "Tells if the layer is above the one passed as argument." }
    , { "below"               , (PyCFunction)PyLayer_below               , METH_VARARGS
                              , "Tells if the layer is below the one passed as argument." }
    , { "contains"            , (PyCFunction)PyLayer_contains            , METH_VARARGS
                              , "Tells if the layer fully contains the one passed as argument." }
    , { "intersect"           , (PyCFunction)PyLayer_intersect           , METH_VARARGS
                              , "Tells if the layer share some BasicLayer with the one passed as argument." }
    , { "isWorking"           , (PyCFunction)PyLayer_isWorking           , METH_NOARGS
                              , "Tells if the layer is the working one for this BasicLayer." }
    , { "setName"             , (PyCFunction)PyLayer_setName             , METH_VARARGS
                              , "Allows to change the layer name." }
    , { "setWorking"          , (PyCFunction)PyLayer_setName             , METH_VARARGS
                              , "Sets the layer as the working one." }
    , { "setMinimalSize"      , (PyCFunction)PyLayer_setMinimalSize      , METH_VARARGS
                              , "Sets the layer minimal size (width)." }
    , { "setMinimalSpacing"   , (PyCFunction)PyLayer_setMinimalSpacing   , METH_VARARGS
                              , "Sets the layer minimal spacing (edge to edge)." }
    , { "setEnclosure"        , (PyCFunction)PyLayer_setEnclosure        , METH_VARARGS
                              , "Sets the enclosure for the given BasicLayer sub-component." }
    , { "setExtentionCap"     , (PyCFunction)PyLayer_setExtentionCap     , METH_VARARGS
                              , "Sets the extention cap for the given BasiLayer sub-component." }
    , { "setExtentionWidth"   , (PyCFunction)PyLayer_setExtentionWidth   , METH_VARARGS
                              , "Sets the extention width for the given BasiLayer sub-component." }
    , { "destroy"             , (PyCFunction)PyLayer_destroy             , METH_NOARGS
                              , "Destroy associated hurricane object The python object remains." }
    , {NULL, NULL, 0, NULL}   /* sentinel */
    };


  // ---------------------------------------------------------------
  // PyLayer Type Methods.


  DBoDeleteMethod(Layer)
  PyTypeObjectLinkPyType(Layer)


#else  // End of Python Module Code Part.


// +=================================================================+
// |              "PyLayer" Shared Library Code Part                 |
// +=================================================================+

  // Link/Creation Method.
  DBoLinkCreateMethod(Layer)

  PyTypeRootObjectDefinitions(Layer)


  extern  void  PyLayer_postModuleInit ()
  {
    PyDict_SetItemString ( PyTypeLayer.tp_dict, "Mask", (PyObject*)&PyTypeLayerMask );
  }


#endif  // End of Shared Library Code Part.

}  // extern "C".

}  // Isobar namespace. 

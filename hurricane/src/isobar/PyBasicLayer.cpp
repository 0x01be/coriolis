
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
// |  Author      :                       Damien Dupuis              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Module  :  "./PyBasicLayer.cpp"                            |
// +-----------------------------------------------------------------+


#include "hurricane/isobar/PyBasicLayer.h"
#include "hurricane/isobar/PyMaterial.h"
#include "hurricane/isobar/PyTechnology.h"


namespace  Isobar {

using namespace Hurricane;

extern "C" {


# undef   ACCESS_OBJECT
# undef   ACCESS_CLASS
# define  ACCESS_OBJECT            _baseObject._object
# define  ACCESS_CLASS(_pyObject)  &(_pyObject->_baseObject)
# define  METHOD_HEAD(function)    GENERIC_METHOD_HEAD(BasicLayer,basicLayer,function)


// +=================================================================+
// |             "PyBasicLayer" Python Module Code Part              |
// +=================================================================+

# if defined(__PYTHON_MODULE__)


  // +-------------------------------------------------------------+
  // |             "PyBasicLayer" Attribute Methods                |
  // +-------------------------------------------------------------+


  static PyObject* PyBasicLayer_create ( PyObject*, PyObject* args )
  {
    trace << "PyBasicLayer_create()" << endl;

    BasicLayer*   basicLayer     = NULL;
    
    HTRY
    PyObject*     pyTechnology   = NULL;
    char*         name           = NULL;
    PyObject*     pyMaterial     = NULL;
    unsigned int  extractNumber  = 0;
    long          minimalSize    = 0;
    long          minimalSpacing = 0;
    
    if (PyArg_ParseTuple( args
                        , "OsO|Ill:BasicLayer.create"
                        , &pyTechnology
                        , &name
                        , &pyMaterial
                        , &extractNumber
                        , &minimalSize
                        , &minimalSpacing
                        )) {
      if (not IsPyTechnology(pyTechnology)) {
        PyErr_SetString ( ConstructorError, "Hurricane.create(): First argument is not of type Technology." );
        return NULL;
      }
      if (not IsPyMaterial(pyMaterial)) {
        PyErr_SetString ( ConstructorError, "Hurricane.create(): Third argument is not of type Material." );
        return NULL;
      }

      basicLayer = BasicLayer::create( PYTECHNOLOGY_O(pyTechnology)
                                     , Name(name)
                                     , *PYMATERIAL_O(pyMaterial)
                                     , extractNumber
                                     , minimalSize
                                     , minimalSpacing
                                     );
    } else {
      PyErr_SetString ( ConstructorError, "Bad parameters given to BasicLayer.create()." );
      return NULL;
    }
    HCATCH

    return PyBasicLayer_Link(basicLayer);
  }


  updatorFromBasicLayer    (setBlockageLayer     ,PyBasicLayer,BasicLayer)
  DirectSetLongAttribute   (PyBasicLayer_setExtractNumber,setExtractNumber,"BasicLayer.setExtractNumber",PyBasicLayer,BasicLayer)
  DirectSetCStringAttribute(PyBasicLayer_setRealName     ,setRealName     ,"BasicLayer.setRealName"     ,PyBasicLayer,BasicLayer)


  // Standart destroy (Attribute).
  DBoDestroyAttribute(PyBasicLayer_destroy, PyBasicLayer)


  // ---------------------------------------------------------------
  // PyBasicLayer Attribute Method table.

  PyMethodDef PyBasicLayer_Methods[] =
    { { "create"             , (PyCFunction)PyBasicLayer_create           , METH_VARARGS|METH_STATIC
                             , "Create a new BasicLayer." }
    , { "setBlockageLayer"   , (PyCFunction)PyBasicLayer_setBlockageLayer , METH_VARARGS
                             , "Sets the blockage layer associated to this one." }
    , { "setExtractNumber"   , (PyCFunction)PyBasicLayer_setExtractNumber , METH_VARARGS
                             , "Sets the layer extract number (for GDSII)." }
    , { "setRealName"        , (PyCFunction)PyBasicLayer_setRealName      , METH_VARARGS
                             , "Sets the real name of this layer (as seen in GDSII)." }
    , { "destroy"            , (PyCFunction)PyBasicLayer_destroy          , METH_NOARGS
                             , "destroy associated hurricane object, the python object remains." }
    , {NULL, NULL, 0, NULL}  /* sentinel */
    };


  // +-------------------------------------------------------------+
  // |               "PyHorizontal" Object Methods                 |
  // +-------------------------------------------------------------+


  DBoDeleteMethod(BasicLayer)
  PyTypeObjectLinkPyType(BasicLayer)


#else  // End of Python Module Code Part.


// +=================================================================+
// |            "PyBasicLayer" Shared Library Code Part              |
// +=================================================================+


  // Link/Creation Method.
  DBoLinkCreateMethod(BasicLayer)


  PyTypeInheritedObjectDefinitions(BasicLayer, Layer)


  extern  void  PyBasicLayer_postModuleInit ()
  {
    PyMaterial_postModuleInit ();

    PyDict_SetItemString ( PyTypeBasicLayer.tp_dict, "Material", (PyObject*)&PyTypeMaterial );
  }


#endif  // End of Shared Library Code Part.


}  // extern "C".

}  // Isobar namespace.
 

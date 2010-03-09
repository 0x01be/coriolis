
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
// |      C y c l o p  -  S i m p l e   V i e w e r                  |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./CyclopMain.cpp"                         |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x

#include  <memory>
#include  <vector>
#include  <algorithm>
using namespace std;

#include  <boost/program_options.hpp>
namespace poptions = boost::program_options;

#include  <QtGui>

#include  "hurricane/DataBase.h"
#include  "hurricane/Technology.h"
#include  "hurricane/Layer.h"
#include  "hurricane/BasicLayer.h"
#include  "hurricane/Cell.h"
#include  "hurricane/Warning.h"
#include  "hurricane/ExtensionGo.h"
#include  "hurricane/Timer.h"
#include  "hurricane/viewer/Graphics.h"
#include  "hurricane/viewer/CellWidget.h"
#include  "hurricane/viewer/HApplication.h"
using namespace Hurricane;

#include  "DemoGo.h"
#include  "Cyclop.h"
using namespace CRL;


namespace {


// -------------------------------------------------------------------
// Function  :  "printHelp()".

  void  printHelp ()
  {
    cout << endl;
    cout << "Usage: cyclop [-v|--verbose] [-V|--very-verbose] [-D|--core-dump]    \\\n"
         << "              [-l|--trace-level <traceLevel>] [-c|--cell <cellName>] \\\n"
         << endl;
    cout << "Options:\n"
         << "    o  [-v|--verbose]      : First level of verbosity.\n"
         << "    o  [-V|--very-verbose] : Second level of verbosity (very talkative).\n"
         << "    o  [-D|--core-dump]    : Enable core dumping.\n"
         << "    o  [-l|--trace-level <traceLevel>] :\n"
         << "           Sets the level of trace, trace messages with a level superior to\n"
         << "           <traceLevel> will be printed on <stderr>.\n"
         << "    o  [-c|--cell <cellName>] :\n"
         << "           The name of the Cell to load, without extention.\n"
         << endl;
  }


} // End of anonymous namespace.




// x-----------------------------------------------------------------x
// |                      Fonctions Definitions                      |
// x-----------------------------------------------------------------x


// -------------------------------------------------------------------
// Function  :  "main()".

int main ( int argc, char *argv[] )
{
  int  returnCode = 0;

  try {
    unsigned int  traceLevel;
    bool          verbose1;
    bool          verbose2;
    bool          coreDump;
    bool          textMode;

    poptions::options_description options ("Command line arguments & options");
    options.add_options()
      ( "help,h"        , "Print this help." )
      ( "verbose,v"     , poptions::bool_switch(&verbose1)->default_value(false)
                        , "First level of verbosity.")
      ( "very-verbose,V", poptions::bool_switch(&verbose2)->default_value(false)
                        , "Second level of verbosity.")
      ( "core-dump,D"   , poptions::bool_switch(&coreDump)->default_value(false)
                        , "Enable core dumping.")
      ( "text,t"        , poptions::bool_switch(&textMode)->default_value(false)
                        , "Run in pure text mode.")
      ( "trace-level,l" , poptions::value<unsigned int>(&traceLevel)->default_value(1000)
                        , "Set the level of trace, trace messages with a level superior to "
                          "<arg> will be printed on <stderr>." )
      ( "cell,c"        , poptions::value<string>()
                        , "The name of the cell to load, whithout extension." );

    poptions::variables_map arguments;
    poptions::store  ( poptions::parse_command_line(argc,argv,options), arguments );
    poptions::notify ( arguments );

    if ( arguments.count("help") ) {
      cout << options << endl;
      exit ( 0 );
    }

    System::getSystem()->setCatchCore ( not coreDump ); 
    if ( verbose1 ) mstream::enable ( mstream::VerboseLevel1 );
    if ( verbose2 ) mstream::enable ( mstream::VerboseLevel2 ); 

    dbo_ptr<AllianceFramework> af ( AllianceFramework::create() );

//     Technology* technology = DataBase::getDB()->getTechnology();
//     technology->setWorkingLayer ( "cut0" );
//     technology->setWorkingLayer ( "cut1" );
//     technology->setWorkingLayer ( "cut2" );
//     technology->setWorkingLayer ( "cut3" );
//     technology->setWorkingLayer ( "cut4" );
//     technology->setWorkingLayer ( "cut5" );
//     technology->setWorkingLayer ( "METAL1" );
//     technology->setWorkingLayer ( "METAL2" );
//     technology->setWorkingLayer ( "METAL3" );
//     technology->setWorkingLayer ( "METAL4" );
//     technology->setWorkingLayer ( "METAL5" );
//     technology->setWorkingLayer ( "METAL6" );
//     technology->setWorkingLayer ( "VIA12" );
//     technology->setWorkingLayer ( "VIA23" );
//     technology->setWorkingLayer ( "VIA34" );
//     technology->setWorkingLayer ( "VIA45" );
//     technology->setWorkingLayer ( "VIA56" );

//     Layer* metal2 = technology->getLayer("METAL2");
//     Layer* above = technology->getLayer("METAL2")->getMetalAbove();
//     cerr << above << " is above METAL2" << endl;

//     Layer* below = technology->getLayer("METAL2")->getMetalBelow();
//     cerr << below << " is below METAL2" << endl;

//     Layer* via = technology->getLayer("VIA23");
//     cerr << via->getTop() << " is top of VIA23" << endl;
//     cerr << via->getBottom() << " is bottom of VIA23" << endl;

//     cerr << "VIA23 mask: " << via->getMask() << endl;
//     cerr << technology->getViaBetween ( above, metal2 ) << " connects METAL2 & METAL3." << endl;

//     cerr << technology->getNthMetal ( 4 ) << " is the fourth metal layer." << endl;

//     Layer::Mask mask = 512;
//     forEach ( Layer*, ilayer, getRangeCollection(&(technology->_getLayerMaskMap()),mask) ) {
//       cerr << *ilayer << " Mask:" << (*ilayer)->getMask() << endl;
//     //BasicLayer* basicLayer = dynamic_cast<BasicLayer*>(*ilayer);
//     }
//     af->loadLibraryCells ( "ramlib" );

    Cell* cell = NULL;

    if ( arguments.count("cell") ) {
      cell = af->getCell (arguments["cell"].as<string>().c_str(), Catalog::State::Views );
      if (!cell) {
        cerr << af->getPrint() << endl;
        cerr << "[ERROR] Cell not found: " << arguments["cell"].as<string>().c_str() << endl;
        exit ( -45 );
      }
    }

    if ( not textMode ) {
      auto_ptr<QApplication> qa ( new HApplication(argc,argv) );
      Graphics::enable ();

//    Layer* layer = DataBase::getDB()->getTechnology()->getLayer("poly");
//    if ( cell && cell->getName() == "inv_x1" ) {
//      forEach ( Component*, icomponent, cell->getComponents(layer->getMask()) ) {
//        Occurrence occurrence ( *icomponent );
//        cyclop->select ( occurrence );
//        cerr << "Selecting: " << occurrence << endl;
//      }
//    }
//    if ( cell && cell->getName() == "inv_x1" ) {
//      Box box = cell->getAbutmentBox();
//      DemoGo::create ( cell, box.inflate(DbU::lambda(2)) );
//    }

      dbo_ptr<Cyclop> cyclop ( Cyclop::create() );
      cmess1 << cyclop->getBanner() << endl;;
      cmess2 << af->getPrint() << endl;

      cyclop->setCell ( cell );
      cyclop->show();
    //cyclop->getCellWidget()->setLayerVisible ( Name("metal1"), false );

      returnCode = qa->exec();
    }
  }
  catch ( Error& e ) {
    cerr << e.what() << endl;
    exit ( 1 );
  }
  catch ( ... ) {
    cout << "[ERROR] Abnormal termination: unmanaged exception.\n" << endl;
    exit ( 2 );
  }

  return returnCode;
}
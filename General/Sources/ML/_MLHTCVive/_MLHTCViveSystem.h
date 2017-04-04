//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  Medical
// \date    2016-07-25
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef _MLHTCVIVE_EXPORTS
  // Use the _MLHTCVIVE_EXPORT macro to export classes and functions.
  #define _MLHTCVIVE_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define _MLHTCVIVE_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif

/*
 *  pIOFile.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 14/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _PINOCCHIO_FILE_H
#define _PINOCCHIO_FILE_H

#include "pIOTypes.h"

#define PIOFileIsInvalid PIOObjectIsInvalid 

#define PIOFile_Structure_Group_Timelines "timeline"
#define PIOFile_Structure_Group_Datasets  "dataset"

#define PIOFile_Structure_Datasets_Data  "data"
#define PIOFile_Structure_Datasets_Count "count"

/// Creates new pinocchIO file. 
///
/// Creates a new pinocchIO file meant to store descriptors extracted from a given medium.
///
/// \param[in] path	Path to new file.
/// \param[in] medium	Path to medium described by this new file.
///
/// \returns	PIOFile
///		- pinocchIO file handle with read/write access rights if new file was successfully created.
///		- negative value otherwise.
///
PIOFile pioNewFile  ( const char* path, const char* medium );

/// Opens an existing pinocchIO file. 
///
/// Opens an existing pinocchIO file with read-only or read/write access rights
///
/// \param[in] path	Path to file.
/// \param[in] flag	Access rights flag
///
/// \returns	PIOFile
///		- pinocchIO file handle when successful
///		- negative value otherwise.
///
/// Depending on flag value, file handle is granted read-only (PINOCCHIO_READONLY) or read/write (PINOCCHIO_READNWRITE) access rights.
PIOFile pioOpenFile( const char* path, PIOFileRights rights );

/// Closes pinocchIO file. 
///
/// Closes a previously opened pinocchIO file.
///
/// \param[in] file	pinocchIO file handle.
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioCloseFile( PIOFile file );

#endif

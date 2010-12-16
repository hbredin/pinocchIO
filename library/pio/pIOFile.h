// 
// Copyright 2010 Herve BREDIN (bredin@limsi.fr)
// Contact: http://pinocchio.niderb.fr/
// 
// This file is part of pinocchIO.
//  
//      pinocchIO is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//  
//      pinocchIO is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//  
//      You should have received a copy of the GNU General Public License
//      along with pinocchIO. If not, see <http://www.gnu.org/licenses/>.
// 

#ifndef _PINOCCHIO_FILE_H
#define _PINOCCHIO_FILE_H

#include "pIOTypes.h"

#define PIOFileIsInvalid PIOObjectIsInvalid 
#define PIOFileIsValid   PIOObjectIsValid

#define PIOFile_Structure_Group_Timelines "timeline"
#define PIOFile_Structure_Group_Datasets  "dataset"

#define PIOFile_Structure_Datasets_Data  "data"
#define PIOFile_Structure_Datasets_Link "link"

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
int pioCloseFile( PIOFile* file );

#endif

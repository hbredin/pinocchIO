/*
 *  pIOTimeline.h
 *  pinocchIO
 *
 *  Created by Hervé BREDIN on 15/10/10.
 *  Copyright 2010 CNRS-LIMSI. All rights reserved.
 *
 */

#ifndef _PINOCCHIO_TIMELINE_H
#define _PINOCCHIO_TIMELINE_H

#include "pIOTypes.h"

#define PIOTimelineIsInvalid PIOObjectIsInvalid 
#define PIOTimelineIsValid   PIOObjectIsValid 

/// Creates new timeline in pinocchIO file. 
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
PIOTimeline pioNewTimeline(PIOFile pioFile, const char* path, const char* description,
							 int numberOfTimeRanges, PIOTimeRange* timeranges);

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
PIOTimeline pioOpenTimeline(PIOObject pioObjectInFile, const char* path);

/// Closes timeline. 
///
/// Closes a previously opened pinocchIO timeline.
///
/// \param[in] pioTimeline	pinocchIO timeline.
///
/// \returns	int
///		- positive value when successful
///		- negative value otherwise.
int pioCloseTimeline( PIOTimeline* pioTimeline );

/// \returns int
///		- number of timelines in file when successfull
///		- negative value otherwise
int pioGetListOfTimelines(PIOFile pioFile, char*** pathsToTimelines);

PIOTimeline pioGetTimeline(PIODataset pioDataset);

#endif

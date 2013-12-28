//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
//  www.kxproject.com

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#ifndef KXPROJECT_DEBUG_H
#define KXPROJECT_DEBUG_H

#define DEBUGLOGGING
#define CONSOLELOGGING
	// #define DEBUGGING		// additional code to check various buffers

#ifdef DEBUGLOGGING
	#ifdef CONSOLELOGGING
		#define debug(message...) do { IOLog(message); } while (0)
	#elif defined(KPRINTFLOGGING)
		#define debug(message...) do { kprintf(message); } while(0)
	#elif defined(FIRELOGGING)
		#define debug(message...) do { FireLog(message); } while(0)
	#elif defined USBLOGGING
		#define debug(message...) do { USBLog(DEBUG_LEVEL_DEVELOPMENT,message); } while(0)
	#else
		#define debug(message...) ;
	#endif
#else
	#define debug(message...) ;
#endif
																
#endif // KXPROJECT_DEBUG_H


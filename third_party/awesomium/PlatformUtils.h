/**
 * @file PlatformUtils.h
 *
 * @author
 *
 * This file is a part of Awesomium, a library that makes it easy for 
 * developers to embed the web in their applications.
 *
 * Website: <http://www.awesomium.com>
 * 
 * Copyright (C) 2011 Khrona. All rights reserved. Awesomium is a 
 * trademark of Khrona.
 */

#ifndef __PLATFORM_UTILS_H__
#define __PLATFORM_UTILS_H__

#if defined(__WIN32__) || defined(_WIN32)
#	if defined(OSM_NONCLIENT_BUILD)
#		define _OSMExport __declspec( dllexport )
#	else
#		define _OSMExport __declspec( dllimport )
#	endif
#pragma warning (disable : 4251)
#else
#	define _OSMExport __attribute__((visibility("default")))
#endif

#endif

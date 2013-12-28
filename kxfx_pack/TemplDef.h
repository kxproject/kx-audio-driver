// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, LeMury 2003-2004.
// All rights reserved

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

#if !defined(AFX_TEMPLDEF_H__742F3281_055B_11D4_B261_00104BB13A66__INCLUDED_)
#define AFX_TEMPLDEF_H__742F3281_055B_11D4_B261_00104BB13A66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// TemplDef.h: helper macroses for using templates with MFC
//
// Author: Yury Goltsman
// email:   ygprg@go.to
// page:    http://go.to/ygprg
// Copyright © 2000, Yury Goltsman
//
// This code provided "AS IS," without any kind of warranty.
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// version : 1.0
//

///////////////////////////////////////////////////////////////////////////////
// common definitions for any map:

// use it to specify list of arguments for class and as theTemplArgs
// e.g. BEGIN_TEMPLATE_MESSAGE_MAP(ARGS2(class base_class, int max),
//                                 CMyTClass<ARGS2(base_class, max)>,
//                                 base_class)

#define ARGS2(arg1, arg2)                     arg1, arg2
#define ARGS3(arg1, arg2, arg3)               arg1, arg2, arg3
#define ARGS4(arg1, arg2, arg3, arg4)         arg1, arg2, arg3, arg4
#define ARGS5(arg1, arg2, arg3, arg4, arg5)   arg1, arg2, arg3, arg4, arg5

///////////////////////////////////////////////////////////////////////////////
// definition for MESSAGE_MAP:

#define DECLARE_TEMPLATE_MESSAGE_MAP() DECLARE_MESSAGE_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_MESSAGE_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_MSGMAP* PASCAL theClass::_GetBaseMessageMap() \
			{ return &baseClass::messageMap; } \
	template <theTemplArgs> \
		const AFX_MSGMAP* theClass::GetMessageMap() const \
			{ return &theClass::messageMap; } \
	template <theTemplArgs> \
		AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
		{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_MESSAGE_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_MSGMAP* theClass::GetMessageMap() const \
			{ return &theClass::messageMap; } \
	template <theTemplArgs> \
		AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
		{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
		{ \

#endif

#define END_TEMPLATE_MESSAGE_MAP() END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for OLECMD_MAP:

#define DECLARE_TEMPLATE_OLECMD_MAP() DECLARE_OLECMD_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_OLECMD_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_OLECMDMAP* PASCAL theClass::_GetBaseCommandMap() \
			{ return &baseClass::commandMap; } \
	template <theTemplArgs> \
		const AFX_OLECMDMAP* theClass::GetCommandMap() const \
			{ return &theClass::commandMap; } \
	template <theTemplArgs> \
		AFX_COMDAT AFX_DATADEF const AFX_OLECMDMAP theClass::commandMap = \
		{ &theClass::_GetBaseCommandMap, &theClass::_commandEntries[0] }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_OLECMDMAP_ENTRY theClass::_commandEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_OLECMD_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_OLECMDMAP* theClass::GetCommandMap() const \
			{ return &theClass::commandMap; } \
	template <theTemplArgs> \
		AFX_COMDAT AFX_DATADEF const AFX_OLECMDMAP theClass::commandMap = \
		{ &baseClass::commandMap, &theClass::_commandEntries[0] }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_OLECMDMAP_ENTRY theClass::_commandEntries[] = \
		{ \

#endif

#define END_TEMPLATE_OLECMD_MAP() END_OLECMD_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for INTERFACE_MAP:

#define DECLARE_TEMPLATE_INTERFACE_MAP() DECLARE_INTERFACE_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_INTERFACE_MAP(theTemplArgs, theClass, theBase) \
	template <theTemplArgs> \
		const AFX_INTERFACEMAP* PASCAL theClass::_GetBaseInterfaceMap() \
			{ return &theBase::interfaceMap; } \
	template <theTemplArgs> \
		const AFX_INTERFACEMAP* theClass::GetInterfaceMap() const \
			{ return &theClass::interfaceMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_INTERFACEMAP theClass::interfaceMap = \
			{ &theClass::_GetBaseInterfaceMap, &theClass::_interfaceEntries[0], }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_INTERFACEMAP_ENTRY theClass::_interfaceEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_INTERFACE_MAP(theTemplArgs, theClass, theBase) \
	template <theTemplArgs> \
		const AFX_INTERFACEMAP* theClass::GetInterfaceMap() const \
			{ return &theClass::interfaceMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_INTERFACEMAP theClass::interfaceMap = \
			{ &theBase::interfaceMap, &theClass::_interfaceEntries[0], }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_INTERFACEMAP_ENTRY theClass::_interfaceEntries[] = \
		{ \

#endif

#define END_TEMPLATE_INTERFACE_MAP() END_INTERFACE_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for DISPATCH_MAP:

#define DECLARE_TEMPLATE_DISPATCH_MAP() DECLARE_DISPATCH_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_DISPATCH_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_DISPMAP* PASCAL theClass::_GetBaseDispatchMap() \
			{ return &baseClass::dispatchMap; } \
	template <theTemplArgs> \
		const AFX_DISPMAP* theClass::GetDispatchMap() const \
			{ return &theClass::dispatchMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DISPMAP theClass::dispatchMap = \
			{ &theClass::_GetBaseDispatchMap, &theClass::_dispatchEntries[0], \
				&theClass::_dispatchEntryCount, &theClass::_dwStockPropMask }; \
	template <theTemplArgs> \
		AFX_COMDAT UINT theClass::_dispatchEntryCount = (UINT)-1; \
	template <theTemplArgs> \
		AFX_COMDAT DWORD theClass::_dwStockPropMask = (DWORD)-1; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DISPMAP_ENTRY theClass::_dispatchEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_DISPATCH_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_DISPMAP* theClass::GetDispatchMap() const \
			{ return &theClass::dispatchMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DISPMAP theClass::dispatchMap = \
			{ &baseClass::dispatchMap, &theClass::_dispatchEntries[0], \
				&theClass::_dispatchEntryCount, &theClass::_dwStockPropMask }; \
	template <theTemplArgs> \
		AFX_COMDAT UINT theClass::_dispatchEntryCount = (UINT)-1; \
	template <theTemplArgs> \
		AFX_COMDAT DWORD theClass::_dwStockPropMask = (DWORD)-1; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DISPMAP_ENTRY theClass::_dispatchEntries[] = \
		{ \

#endif

#define END_TEMPLATE_DISPATCH_MAP() END_DISPATCH_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for CONNECTION_MAP:

#define DECLARE_TEMPLATE_CONNECTION_MAP() DECLARE_CONNECTION_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_CONNECTION_MAP(theTemplArgs, theClass, theBase) \
	template <theTemplArgs> \
		const AFX_CONNECTIONMAP* PASCAL theClass::_GetBaseConnectionMap() \
			{ return &theBase::connectionMap; } \
	template <theTemplArgs> \
		const AFX_CONNECTIONMAP* theClass::GetConnectionMap() const \
			{ return &theClass::connectionMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_CONNECTIONMAP theClass::connectionMap = \
			{ &theClass::_GetBaseConnectionMap, &theClass::_connectionEntries[0], }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_CONNECTIONMAP_ENTRY theClass::_connectionEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_CONNECTION_MAP(theTemplArgs, theClass, theBase) \
	template <theTemplArgs> \
		const AFX_CONNECTIONMAP* theClass::GetConnectionMap() const \
			{ return &theClass::connectionMap; } \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_CONNECTIONMAP theClass::connectionMap = \
			{ &(theBase::connectionMap), &theClass::_connectionEntries[0], }; \
	template <theTemplArgs> \
		AFX_COMDAT const AFX_DATADEF AFX_CONNECTIONMAP_ENTRY theClass::_connectionEntries[] = \
		{ \

#endif

#define END_TEMPLATE_CONNECTION_MAP() END_CONNECTION_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for EVENTSINK_MAP:

#define DECLARE_TEMPLATE_EVENTSINK_MAP() DECLARE_EVENTSINK_MAP()

#ifdef _AFXDLL
#define BEGIN_TEMPLATE_EVENTSINK_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP* PASCAL theClass::_GetBaseEventSinkMap() \
			{ return &baseClass::eventsinkMap; } \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP* theClass::GetEventSinkMap() const \
			{ return &theClass::eventsinkMap; } \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP theClass::eventsinkMap = \
			{ &theClass::_GetBaseEventSinkMap, &theClass::_eventsinkEntries[0], \
				&theClass::_eventsinkEntryCount }; \
	template <theTemplArgs> \
		UINT theClass::_eventsinkEntryCount = (UINT)-1; \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP_ENTRY theClass::_eventsinkEntries[] = \
		{ \

#else
#define BEGIN_TEMPLATE_EVENTSINK_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP* theClass::GetEventSinkMap() const \
			{ return &theClass::eventsinkMap; } \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP theClass::eventsinkMap = \
			{ &baseClass::eventsinkMap, &theClass::_eventsinkEntries[0], \
				&theClass::_eventsinkEntryCount }; \
	template <theTemplArgs> \
		UINT theClass::_eventsinkEntryCount = (UINT)-1; \
	template <theTemplArgs> \
		const AFX_EVENTSINKMAP_ENTRY theClass::_eventsinkEntries[] = \
		{ \

#endif

#define END_TEMPLATE_EVENTSINK_MAP() END_EVENTSINK_MAP()

///////////////////////////////////////////////////////////////////////////////
// definition for EVENT_MAP:

#define DECLARE_TEMPLATE_EVENT_MAP() DECLARE_EVENT_MAP()

#define BEGIN_TEMPLATE_EVENT_MAP(theTemplArgs, theClass, baseClass) \
	template <theTemplArgs> \
	const AFX_EVENTMAP* theClass::GetEventMap() const \
		{ return &eventMap; } \
	template <theTemplArgs> \
	const AFX_DATADEF AFX_EVENTMAP theClass::eventMap = \
		{ &(baseClass::eventMap), theClass::_eventEntries, \
			&theClass::_dwStockEventMask }; \
	template <theTemplArgs> \
	AFX_COMDAT DWORD theClass::_dwStockEventMask = (DWORD)-1; \
	template <theTemplArgs> \
	AFX_COMDAT const AFX_DATADEF AFX_EVENTMAP_ENTRY theClass::_eventEntries[] = \
	{

#define END_TEMPLATE_EVENT_MAP() END_EVENT_MAP()


#endif // !defined(AFX_TEMPLDEF_H__742F3281_055B_11D4_B261_00104BB13A66__INCLUDED_)

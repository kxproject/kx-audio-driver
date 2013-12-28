/* BlueSmoke 1010 PCI/PCIe Audio Driver
   Copyright (c) Eugene Gavrilov, 2002-2012

   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 2 of the License, or
   * (at your option) any later version.
   * 
   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, write to the Free Software
   * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#include "common.h"

/*****************************************************************************
 * CUnknown implementation
 */

/*****************************************************************************
 * CUnknown::CUnknown()
 *****************************************************************************
 * Constructor.
 */
CUnknown::CUnknown(PUNKNOWN pUnknownOuter)
:   m_lRefCount(0)
{
    if (pUnknownOuter)
    {
        m_pUnknownOuter = pUnknownOuter;
    }
    else
    {
        m_pUnknownOuter = PUNKNOWN(dynamic_cast<PNONDELEGATINGUNKNOWN>(this));
    }
}

/*****************************************************************************
 * CUnknown::~CUnknown()
 *****************************************************************************
 * Destructor.
 */
CUnknown::~CUnknown(void)
{
}




/*****************************************************************************
 * INonDelegatingUnknown implementation
 */

/*****************************************************************************
 * CUnknown::NonDelegatingAddRef()
 *****************************************************************************
 * Register a new reference to the object without delegating to the outer
 * unknown.
 */
STDMETHODIMP_(ULONG) CUnknown::NonDelegatingAddRef(void)
{
    ASSERT(m_lRefCount >= 0);

    InterlockedIncrement(&m_lRefCount);

    return ULONG(m_lRefCount);
}

/*****************************************************************************
 * CUnknown::NonDelegatingRelease()
 *****************************************************************************
 * Release a reference to the object without delegating to the outer unknown.
 */
STDMETHODIMP_(ULONG) CUnknown::NonDelegatingRelease(void)
{
    ASSERT(m_lRefCount > 0);

    if (InterlockedDecrement(&m_lRefCount) == 0)
	{
        m_lRefCount++;
        delete this;
        return 0;
	}

    return ULONG(m_lRefCount); 
}

/*****************************************************************************
 * CUnknown::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
STDMETHODIMP CUnknown::NonDelegatingQueryInterface
(
    REFIID  rIID,
    PVOID * ppVoid
)
{
    ASSERT(ppVoid);

    if (IsEqualGUIDAligned(rIID,IID_IUnknown))
    {
        *ppVoid = PVOID(PUNKNOWN(this));
    }
    else
    {
        *ppVoid = NULL;
    }
    
    if (*ppVoid)
    {
        PUNKNOWN(*ppVoid)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

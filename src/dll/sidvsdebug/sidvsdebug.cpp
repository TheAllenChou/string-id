/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include <iostream>
#include <windows.h>

#include "sid/sid.h"

#define ADDIN_API __declspec(dllexport)

typedef struct tagDEBUGHELPER
{
  DWORD dwVersion;
  HRESULT(WINAPI *ReadDebuggeeMemory)(struct tagDEBUGHELPER *pThis, DWORD dwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot);
  // from here only when dwVersion >= 0x20000
  DWORDLONG(WINAPI *GetRealAddress)(struct tagDEBUGHELPER *pThis);
  HRESULT(WINAPI *ReadDebuggeeMemoryEx)(struct tagDEBUGHELPER *pThis, DWORDLONG qwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot);
  int (WINAPI *GetProcessorType)(struct tagDEBUGHELPER *pThis);
} DEBUGHELPER;

typedef HRESULT(WINAPI *CUSTOMVIEWER)(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved);

extern "C" ADDIN_API HRESULT StringIdFormatter(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved);

ADDIN_API HRESULT StringIdFormatter(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t max, DWORD reserved)
{
  StringId c;
  DWORD nGot;
  pHelper->ReadDebuggeeMemory(pHelper, dwAddress, sizeof(StringId), &c, &nGot);
  sprintf_s(pResult, max, "StringId(value=%x)", max, nGot, dwAddress, c.GetValue());
  return S_OK;
}

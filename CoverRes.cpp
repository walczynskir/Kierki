#include "StdAfx.h"
#include "coverres.h"


static void SetResource(LPRCOVERRES a_pCoverRes, LPCVOID a_res);
static void SetStringResource(LPRCOVERRES a_pCoverRes, LPCTSTR a_psRes);
static void SetIntResource(LPRCOVERRES a_pCoverRes, LPCVOID a_pRes);
static void ReleasePath(LPRCOVERRES a_pCoverRes);
static void ReleaseRes(LPRCOVERRES a_pCoverRes);


void RCoverRes_Init(LPRCOVERRES a_pRes)
{
	ZeroMemory(a_pRes, sizeof(RCOVERRES));
}


void RCoverRes_Done(LPRCOVERRES a_pRes)
{
	ReleasePath(a_pRes);
	ReleaseRes(a_pRes);
}


DWORD RCoverRes_GetSize(const LPRCOVERRES a_pCoverRes)
{
	ASSERT(a_pCoverRes->psPath != NULL);
	return static_cast<DWORD>(
		sizeof(a_pCoverRes->bExec) + 
		(_tcslen(a_pCoverRes->psPath) + 1) * sizeof(TCHAR) +
		sizeof(a_pCoverRes->bInt) + 
		(a_pCoverRes->bInt ? sizeof(a_pCoverRes->idRes) : (_tcslen(a_pCoverRes->psRes) + 1) * sizeof(TCHAR)));
}


void RCoverRes_ToBuf(const LPRCOVERRES a_pCoverRes, LPBYTE a_pBuf, DWORD a_iSize)
{
	ASSERT(a_iSize >= RCoverRes_GetSize(a_pCoverRes));
	ASSERT(a_pBuf != NULL);
	long l_iSize = sizeof(a_pCoverRes->bExec);
	memcpy(a_pBuf, &a_pCoverRes->bExec, l_iSize);
	a_pBuf += l_iSize;
	l_iSize = (static_cast<long>(_tcslen(a_pCoverRes->psPath) + 1) * sizeof(TCHAR));
	memcpy(a_pBuf, a_pCoverRes->psPath, l_iSize);
	a_pBuf += l_iSize;
	l_iSize = sizeof(a_pCoverRes->bInt);
	memcpy(a_pBuf, &a_pCoverRes->bInt, l_iSize);
	a_pBuf += l_iSize;
	l_iSize = static_cast<long>(a_pCoverRes->bInt ? sizeof(a_pCoverRes->idRes) : ((_tcslen(a_pCoverRes->psRes) + 1) * sizeof(TCHAR)));
	LPCVOID l_p;
	if (a_pCoverRes->bInt)
	{
		l_p = &(a_pCoverRes->idRes);
	}
	else
	{
		l_p = a_pCoverRes->psRes;
	}
	memcpy(a_pBuf, l_p, l_iSize);
}


void RCoverRes_FromBuf(LPRCOVERRES a_pCoverRes, const BYTE* a_pBuf, DWORD a_iSize)
{
	//TODO seems that code is too complicated
	ASSERT(a_iSize > 0);
	ASSERT(a_pBuf != NULL);
	long l_iSize = sizeof(a_pCoverRes->bExec);
	memcpy(&a_pCoverRes->bExec, a_pBuf, l_iSize);
	a_pBuf += l_iSize;
	ReleasePath(a_pCoverRes);
	a_pCoverRes->iPathSize = static_cast<long>(_tcslen(reinterpret_cast<LPCTSTR>(a_pBuf)) + 1);
	a_pCoverRes->psPath = new TCHAR[a_pCoverRes->iPathSize];
	ASSERT(a_pCoverRes->psPath != NULL);
	_tcsncpy_s(a_pCoverRes->psPath, a_pCoverRes->iPathSize, reinterpret_cast<LPCTSTR>(a_pBuf), a_pCoverRes->iPathSize);
	l_iSize = a_pCoverRes->iPathSize * sizeof(TCHAR);
	a_pBuf += l_iSize;
	l_iSize = sizeof(a_pCoverRes->bInt);
	BOOL l_bInt;
	memcpy(&l_bInt, a_pBuf, l_iSize);
	a_pBuf += l_iSize;
	if (l_bInt)
	{
		UINT l_idRes;
		l_iSize = sizeof(l_idRes);
		memcpy(&(l_idRes), a_pBuf, l_iSize);
		SetIntResource(a_pCoverRes, reinterpret_cast<LPCVOID>(static_cast<LONG_PTR>(l_idRes)));
	}
	else
	{
		SetStringResource(a_pCoverRes, reinterpret_cast<LPCTSTR>(a_pBuf));
	}
}


void RCoverRes_Set(LPRCOVERRES a_pCoverRes, bool a_bExec, LPCTSTR a_sPath, bool a_bInt, LPCVOID a_psRes)
{
	a_pCoverRes->bExec = a_bExec;

	ReleasePath(a_pCoverRes);
	a_pCoverRes->iPathSize = static_cast<long>(_tcslen(a_sPath) + 1);
	a_pCoverRes->psPath = new TCHAR[a_pCoverRes->iPathSize];
	ASSERT(a_pCoverRes->psPath != NULL);

	_tcsncpy_s(a_pCoverRes->psPath, a_pCoverRes->iPathSize, a_sPath, _TRUNCATE);
	if (a_bInt)
	{
		SetIntResource(a_pCoverRes, a_psRes);
	}
	else
	{
		SetStringResource(a_pCoverRes, static_cast<LPCTSTR>(a_psRes));
	}
}


LPCTSTR RCoverRes_GetIntResource(const RCOVERRES* a_pCoverRes)
{
	if (a_pCoverRes->bInt)
	{
		return MAKEINTRESOURCE(a_pCoverRes->idRes);
	}
	else
	{
		return a_pCoverRes->psRes;
	}
}


void RCoverRes_SetResource(LPRCOVERRES a_pCoverRes, LPCVOID a_res)
{
	if (IS_INTRESOURCE(a_res))
	{
		SetIntResource(a_pCoverRes, a_res);
	}
	else
	{
		SetStringResource(a_pCoverRes, static_cast<LPCTSTR>(a_res));
	}
}


void SetIntResource(LPRCOVERRES a_pCoverRes, LPCVOID a_pRes)
{
	a_pCoverRes->bInt = true;
	a_pCoverRes->idRes = static_cast<UINT>(reinterpret_cast<LONG_PTR>(a_pRes));
}


void SetStringResource(LPRCOVERRES a_pCoverRes, LPCTSTR a_psRes)
{
	long l_iLen = static_cast<long>(_tcslen(a_psRes)) + 1;
	if ((a_pCoverRes->psRes == NULL) || (static_cast<long>((_tcslen(a_psRes) + 1)) > a_pCoverRes->iResSize))
	{
		ReleaseRes(a_pCoverRes);
		a_pCoverRes->psRes = new TCHAR[l_iLen];
		a_pCoverRes->iResSize = l_iLen;
	}
	a_pCoverRes->bInt = false;
	_tcsncpy_s(a_pCoverRes->psRes, l_iLen, a_psRes, _TRUNCATE);
	a_pCoverRes->psRes[l_iLen - 1] = _T('\0');
}


void ReleasePath(LPRCOVERRES a_pCoverRes)
{
	if (a_pCoverRes->psPath != NULL)
	{
		delete[] a_pCoverRes->psPath;
		a_pCoverRes->psPath = NULL;
		a_pCoverRes->iPathSize = 0;
	}
}


void ReleaseRes(LPRCOVERRES a_pCoverRes)
{
	if (a_pCoverRes->psRes != NULL)
	{
		delete[] a_pCoverRes->psRes;
		a_pCoverRes->psRes = NULL;
		a_pCoverRes->iResSize = 0;
	}
}

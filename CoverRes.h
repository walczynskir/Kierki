#pragma once


typedef struct S_COVERRES
{
	BOOL   bExec;
	LPTSTR psPath;
	long   iPathSize;
	BOOL   bInt;
	UINT   idRes;
	LPTSTR psRes;
	long   iResSize;
} RCOVERRES, *LPRCOVERRES;

void RCoverRes_Init(LPRCOVERRES a_pRes);
void RCoverRes_Done(LPRCOVERRES a_pRes);

DWORD RCoverRes_GetSize(const LPRCOVERRES a_pRes);
void RCoverRes_ToBuf(const LPRCOVERRES a_pRes, LPBYTE a_pBuf, DWORD a_iSize);
void RCoverRes_FromBuf(LPRCOVERRES a_pRes, const BYTE* a_pBuf, DWORD a_iSize);
void RCoverRes_Set(LPRCOVERRES a_pRes, bool a_bExec, LPCTSTR a_psPath, bool a_bInt, LPCVOID a_psRes);
LPCTSTR RCoverRes_GetIntResource(const RCOVERRES* a_pCoverRes);
void RCoverRes_SetResource(LPRCOVERRES a_pCoverRes, LPCVOID a_res);


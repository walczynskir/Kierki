#include "StdAfx.h"
#include "RArchive.h"


RArchive::RArchive(tfile* a_pFile, bool a_bStoring)
{
	m_pFile = a_pFile;
	m_bStoring = a_bStoring;
}

RArchive::~RArchive(void)
{
}

void RArchive::Flush()
{
	m_pFile->flush();
}

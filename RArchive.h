#pragma once
#include "RFile.h"

class RArchive
{
public:
	RArchive(tfile* a_pFile, bool a_bStoring);
	~RArchive(void);
	bool IsStoring() const { return m_bStoring; };
	void SetStoring(bool a_bStoring) { m_bStoring; };
	void Flush();

private:
	bool m_bStoring;
	tfile* m_pFile;
};

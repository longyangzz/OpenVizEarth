
//×Ô¶¨Òå
#include "GeneratorContext.h"
#include "FileSplitStrategy.h"

using namespace DC::Pyra;

GeneratorContext::GeneratorContext(const QString& inFile, const LevelType& level, const QDir& outDir, bool isAll/* = true*/)
	: m_inFile(inFile)
	, m_level(level)
	, m_splitStrategy(nullptr)
	, m_outDir(outDir)
	, m_allspit(isAll)
{

}

GeneratorContext::~GeneratorContext()
{

}

ErrorType GeneratorContext::Generate()
{
	if (!m_splitStrategy)
	{
		return eError;
	}

	if (!m_outDir.exists())
	{
		return eError;
	}

	ErrorType isSplit = m_splitStrategy->Spit(m_inFile, m_level, m_outDir, m_allspit);

	return isSplit;
}

void GeneratorContext::SetSplitStrategy(FileSplitStrategy* stratery)
{
	m_splitStrategy = stratery;
}
#ifndef SPLITFACTORY_H
#define SPLITFACTORY_H

#include "QString"
#include "SPliter.h"
namespace DC
{
	namespace Pyra
	{
		class SplitFactory
		{
		public:
			SplitFactory(const QString& suffix);
			~SplitFactory();

			//返回一个分块器
			SPliter* CreateSpliter();
		private:
			QString m_suffix;
		};

	}
}

#endif // SPLITFACTORY_H

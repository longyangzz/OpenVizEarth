#ifndef GEOMETRY_H
#define GEOMETRY_H

namespace DC
{
	namespace Pyra
	{
		class Quadnode;
		class Geometry
		{
		public:
			Geometry(Quadnode* node);
			~Geometry();

			virtual void Draw() = 0;

			//获取关联的节点
			Quadnode* GetNode() const;
		private:
			//关联的节点对象，主要是从中获取数据
			Quadnode* m_assNode;
		};


	}
}

#endif // GEOMETRY_H

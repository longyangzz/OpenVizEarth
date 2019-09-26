#include "Geometry.h"

using namespace DC::Pyra;

Geometry::Geometry(Quadnode* node)
{
	m_assNode = node;
}

Geometry::~Geometry()
{

}

Quadnode* Geometry::GetNode() const
{
	return m_assNode;
}
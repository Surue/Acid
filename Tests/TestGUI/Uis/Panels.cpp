#include "Panels.hpp"

namespace test
{
Panels::Panels(UiObject *parent) :
	UiObject(parent, UiBound::Screen),
	m_background(this, UiBound::Screen, Image2d::Create("Guis/White.png")),
	m_hierarchy(Hierarchy(this)),
	m_inspector(Inspector(this))
{
}

void Panels::UpdateObject()
{
}
}

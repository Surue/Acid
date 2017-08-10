#include "vertexdata.h"

namespace flounder
{
	const int vertexdata::NO_INDEX = -1;

	vertexdata::vertexdata(const int index, vector3 position)
	{
		m_position = position;

		m_textureIndex = NO_INDEX;
		m_normalIndex = NO_INDEX;

		m_duplicateVertex = nullptr;

		m_index = index;
		m_length = position.length();

		m_tangents = std::vector<vector3*>();
		m_averagedTangent = vector3();
	}

	vertexdata::~vertexdata()
	{
		//	delete m_duplicateVertex;
	}

	void vertexdata::addTangent(vector3 *tangent)
	{
		m_tangents.push_back(tangent);
	}

	void vertexdata::averageTangents()
	{
		if (m_tangents.empty())
		{
			return;
		}

		for (std::vector<vector3*>::iterator it = m_tangents.begin(); it < m_tangents.end(); it++)
		{
			vector3::add(m_averagedTangent, **it, &m_averagedTangent);
		}

		if (m_averagedTangent.length() > 0.0f)
		{
			m_averagedTangent.normalize();
		}
	}

	bool vertexdata::isSet()
	{
		return (m_textureIndex != NO_INDEX) && (m_normalIndex != NO_INDEX);
	}

	bool vertexdata::hasSameTextureAndNormal(const int textureIndexOther, const int normalIndexOther)
	{
		return textureIndexOther == m_textureIndex && normalIndexOther == m_normalIndex;
	}
}

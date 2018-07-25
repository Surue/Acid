#include "Resources.hpp"

#include <algorithm>

namespace acid
{
	Resources::Resources() :
		m_resources(std::vector<std::shared_ptr<IResource>>()),
		m_timerPurge(Timer(5.0f))
	{
	}

	Resources::~Resources()
	{
	}

	void Resources::Update()
	{
		if (m_timerPurge.IsPassedTime())
		{
			m_timerPurge.ResetStartTime();

			for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
			{
				if ((*it).use_count() <= 1)
				{
					fprintf(stdout, "Resource '%s' erased\n", (*it)->GetName().c_str());
					m_resources.erase(it);
				}
			}
		}
	}

	std::shared_ptr<IResource> Resources::Get(const std::string &filename)
	{
		for (auto &resource : m_resources)
		{
			if (resource != nullptr && resource->GetName() == filename)
			{
				return resource;
			}
		}

		return nullptr;
	}

	void Resources::Add(std::shared_ptr<IResource> resource)
	{
		if (std::find(m_resources.begin(), m_resources.end(), resource) != m_resources.end())
		{
			return;
		}

		m_resources.emplace_back(resource);
	}

	bool Resources::Remove(std::shared_ptr<IResource> resource)
	{
		for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
		{
			if (*it == resource)
			{
				m_resources.erase(it);
				return true;
			}
		}

		return false;
	}

	bool Resources::Remove(const std::string &filename)
	{
		for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
		{
			if ((*it)->GetName() == filename)
			{
				m_resources.erase(it);
				return true;
			}
		}

		return false;
	}
}

#include "sphere.hpp"

namespace Flounder
{
	sphere::sphere() :
		icollider(),
		m_radius(1.0f),
		m_position(new Vector3())
	{
	}

	sphere::sphere(const float &radius, const Vector3 &position) :
		icollider(),
		m_radius(radius),
		m_position(new Vector3(position))
	{
	}

	sphere::sphere(const sphere &source) :
		icollider(),
		m_radius(source.m_radius),
		m_position(new Vector3(*source.m_position))
	{
	}

	sphere::~sphere()
	{
		delete m_position;
	}

	icollider *sphere::update(const Vector3 &position, const Vector3 &rotation, const float &scale, icollider *destination)
	{
		if (destination == nullptr)
		{
			destination = new sphere();
		}

		sphere *source = dynamic_cast<sphere*>(destination);

		source->m_radius = m_radius * scale;
		source->m_position->Set(position);

		return source;
	}

	Vector3 *sphere::resolveCollision(const icollider &other, const Vector3 &positionDelta, Vector3 *destination)
	{
		if (destination == nullptr)
		{
			destination = new Vector3();
		}

		const sphere &sphere2 = dynamic_cast<const sphere&>(other);
		float d = sphere2.m_radius + m_radius;

		float xDif = m_position->m_x - sphere2.m_position->m_x;
		float yDif = m_position->m_y - sphere2.m_position->m_y;
		float zDif = m_position->m_z - sphere2.m_position->m_z;
		float distance = xDif * xDif + yDif * yDif + zDif * zDif;
		// TODO: Resolve!
		return destination;
	}

	intersect *sphere::intersects(const icollider &other)
	{
		/*if (dynamic_cast<aabb*>(other) != 0)
		{
			AABB *aabb = dynamic_cast<AABB*>(other);

			float distanceSquared = m_radius * m_radius;

			if (m_position->x < aabb->getMinExtents()->x)
			{
				distanceSquared -= pow(m_position->x - aabb->getMinExtents()->x, 2);
			}
			else if (m_position->x > aabb->getMaxExtents()->x)
			{
				distanceSquared -= pow(m_position->x - aabb->getMaxExtents()->x, 2);
			}

			if (m_position->y < aabb->getMinExtents()->x)
			{
				distanceSquared -= pow(m_position->y - aabb->getMinExtents()->y, 2);
			}
			else if (m_position->x > aabb->getMaxExtents()->x)
			{
				distanceSquared -= pow(m_position->y - aabb->getMaxExtents()->y, 2);
			}

			if (m_position->z < aabb->getMinExtents()->x)
			{
				distanceSquared -= pow(m_position->z - aabb->getMinExtents()->z, 2);
			}
			else if (m_position->z > aabb->getMaxExtents()->x)
			{
				distanceSquared -= pow(m_position->z - aabb->getMaxExtents()->z, 2);
			}

			return new intersect(distanceSquared > 0.0f, dynamic_cast<float>(sqrt(distanceSquared)));
		}
		else */
		const sphere &sphere2 = dynamic_cast<const sphere&>(other);

		float d = sphere2.m_radius + m_radius;

		float xDif = m_position->m_x - sphere2.m_position->m_x;
		float yDif = m_position->m_y - sphere2.m_position->m_y;
		float zDif = m_position->m_z - sphere2.m_position->m_z;
		float distance = xDif * xDif + yDif * yDif + zDif * zDif;

		bool intersects = d * d > distance;
		return new intersect(intersects, (d * d) - distance);
	}

	intersect *sphere::intersects(const ray &ray)
	{
		Vector3 *L = Vector3::Subtract(*ray.m_origin, *m_position, nullptr);

		float a = Vector3::Dot(*ray.m_currentRay, *ray.m_currentRay);
		float b = 2.0f * (Vector3::Dot(*ray.m_currentRay, *L));
		float c = (Vector3::Dot(*L, *L)) - (m_radius * m_radius);

		float disc = b * b - 4.0f * a * c;

		delete L;

		if (disc < 0.0f)
		{
			return new intersect(false, -1.0f);
		}

		float distSqrt = sqrt(disc);
		float q;

		if (b < 0.0f)
		{
			q = (-b - distSqrt) / 2.0f;
		}
		else
		{
			q = (-b + distSqrt) / 2.0f;
		}

		float t0 = q / a;
		float t1 = c / q;

		if (t0 > t1)
		{
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}

		if (t1 < 0.0f)
		{
			return new intersect(false, -1.0f);
		}

		float t;

		if (t0 < 0.0f)
		{
			t = t1;
		}
		else
		{
			t = t0;
		}

		return new intersect(true, t);
	}

	bool sphere::inFrustum(const frustum &frustum)
	{
		return frustum.sphereInFrustum(m_position->m_x, m_position->m_y, m_position->m_z, m_radius);
	}

	bool sphere::contains(const icollider &other)
	{
		const sphere &sphere2 = dynamic_cast<const sphere&>(other);

		return
			sphere2.m_position->m_x + sphere2.m_radius - 1.0f <= m_position->m_x + m_radius - 1.0f &&
			sphere2.m_position->m_x - sphere2.m_radius + m_radius >= m_position->m_x - m_radius + 1.0f &&
			sphere2.m_position->m_y + sphere2.m_radius - 1.0f <= m_position->m_y + m_radius - 1.0f &&
			sphere2.m_position->m_y - sphere2.m_radius + 1.0f >= m_position->m_y - m_radius + 1.0f &&
			sphere2.m_position->m_z + sphere2.m_radius - 1.0f <= m_position->m_z + m_radius - 1.0f &&
			sphere2.m_position->m_z - sphere2.m_radius + 1.0f >= m_position->m_z - m_radius + 1.0f;
	}

	bool sphere::contains(const Vector3 &point)
	{
		return Vector3::GetDistanceSquared(*m_position, point) <= m_radius * m_radius;
	}
}
#version 430 core

#define	SEARCHSTACKSIZE 24
#define RAYSTACKSIZE 4

const float INFINITY = 1.0f / 0.0f;
const float FLT_EPSILON = 0.0001;

///////////////////////////////////////////////////////////////////////////////

struct		Aabb
{
	vec4	bottom;
	vec4	top;
};

struct		AabbNode
{
	vec3	bottom;
	int		data;
	vec3	top;
	int		children;
};

struct		Camera
{
	vec4	position;
	vec4	forward;
	vec4	right;
	vec4	up;
	vec2	half_resolution;
};

struct		Light
{
	vec4	position;
	vec4	color;
};

struct		Material
{
	vec4	color;
	float	refraction;
	float	reflection;
	float	shining;
};

///////////////////////////////////////////////////////////////////////////////

struct		Ray
{
	vec4	origin;
	vec4	direction;
};

struct		Impact
{
	vec4	normal;
	float	near;
	float	far;
	int		index;
};

///////////////////////////////////////////////////////////////////////////////

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
out vec4 color;

layout (std140) uniform _camera
{
    Camera camera;
};

layout (std430) readonly buffer _nodes
{
    AabbNode nodes[];
};

layout (std430) readonly buffer _materials
{
    Material materials[];
};

uniform uint lights_number;

layout (std430) readonly buffer _lights
{
    Light lights[];
};

///////////////////////////////////////////////////////////////////////////////

bool			intersect_rayaabb(in const Ray ray, in const int aabbidx, out float tnear, out float tfar)
{
	const vec3	tbot = (nodes[aabbidx].bottom - ray.origin.xyz) * ray.direction.xyz;
	const vec3	ttop = (nodes[aabbidx].top - ray.origin.xyz) * ray.direction.xyz;
	const vec3	tmin = min(tbot, ttop);
	const vec3 	tmax = max(tbot, ttop);

	tnear = max(max(tmin.x, tmin.y), tmin.z);
	tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (tfar >= 0.0f && tnear <= tfar);
}

bool			compute_rayaabb_normal(in const Ray ray, in const int aabbidx, out vec4 normal)
{
	const vec3 	tbot = (nodes[aabbidx].bottom - ray.origin.xyz) * ray.direction.xyz;
	const vec3	ttop = (nodes[aabbidx].top - ray.origin.xyz) * ray.direction.xyz;
	const vec3 	tmin = min(tbot, ttop);

	if (tmin.x > tmin.y)
	{
		if (tmin.x > tmin.z)
			normal = vec4(ray.direction.x >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f);
		else
			normal = vec4(0.0f, 0.0f, ray.direction.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	else
	{
		if (tmin.y > tmin.z)
			normal = vec4(0.0f, ray.direction.y >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
		else
			normal = vec4(0.0f, 0.0f, ray.direction.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	return (true);
}

bool			find_closest(in const Ray ray, inout Impact data)
{
	const int	idx = data.index;
	const Ray	invray = { ray.origin, 1.0f / ray.direction };
	int			stack[SEARCHSTACKSIZE];
	float		near_stack[SEARCHSTACKSIZE];
	int			top = 0;

	stack[top] = 0;
	near_stack[top] = 0.0f;
	do
	{
		if (near_stack[top] < data.near)
		{
			const int	index = stack[top];

			if (nodes[index].children == -1)
			{
				if (index != idx)
				{
					data.index = index;
					data.near = near_stack[top];
				}
			}
			else
			{
				float	lnear;
				float	rnear;
				float	far;

				const int	left = nodes[index].children;
				const int	right = left + 1;
				const bool	ltrue = intersect_rayaabb(invray, left, lnear, far);
				const bool	rtrue = intersect_rayaabb(invray, right, rnear, far);

				if (lnear > rnear)
				{
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
				}
				else
				{
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
				}
			}
		}
	}
	while (--top >= 0);

	return (data.index != -1 ? compute_rayaabb_normal(invray, data.index, data.normal) : false);
}

float		find_closest_s(in const Ray ray, in const int value, out int idx)
{
	float	near = INFINITY;
	idx = -1;

	const Ray	invray = { ray.origin, 1.0f / ray.direction };
	int			stack[SEARCHSTACKSIZE];
	float		near_stack[SEARCHSTACKSIZE];
	int			top = 0;

	stack[top] = 0;
	near_stack[top] = 0.0f;
	do
	{
		if (near_stack[top] < near)
		{
			const int	index = stack[top];

			if (nodes[index].children == -1)
			{
				idx = index;
				near = near_stack[top];
			}
			else
			{
				float	lnear;
				float	rnear;
				float	far;

				const int	left = nodes[index].children;
				const int	right = left + 1;
				const bool	ltrue = nodes[left].children == -1 && nodes[left].data == value ? false : intersect_rayaabb(invray, left, lnear, far);
				const bool	rtrue = nodes[right].children == -1 && nodes[right].data == value ? false : intersect_rayaabb(invray, right, rnear, far);

				if (lnear > rnear)
				{
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
				}
				else
				{
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
				}
			}
		}
	}
	while (--top >= 0);

	if (idx == -1)
	{
		float	far;
		intersect_rayaabb(invray, 0, far, near);
	}
	return (near);
}

float			find_closest_o(in const Ray ray, in const int idx)
{
	const Ray	invray = { ray.origin, 1.0f / ray.direction };
	int			stack[SEARCHSTACKSIZE];
	int			top = 0;
	float		near = 2.0f;

	stack[top] = 0;
	do
	{
		const int	index = stack[top];
		float		far;

		float		tnear;
		if (intersect_rayaabb(invray, index, tnear, far) && tnear < near)
		{
			if (nodes[index].children == -1)
			{
				if (index != idx)
				{
					near = tnear;
				}
			}
			else
			{
				stack[top++] = nodes[index].children;
				stack[top++] = nodes[index].children + 1;
			}
		}
	}
	while (--top >= 0);

	return (near);
}

bool			compute_shadow(in const Ray ray, in const float distance, in const int idx, inout vec4 color)
{
	const Ray	invray = { ray.origin, 1.0f / ray.direction };
	int			stack[SEARCHSTACKSIZE];
	int			top = 0;

	stack[top] = 0;
	do
	{
		const int	index = stack[top];
		float		near;
		float		far;

		if (intersect_rayaabb(invray, index, near, far) && near < distance)
		{
			if (nodes[index].children == -1)
			{
				if (index != idx)
				{
					const int	mtlindex = nodes[index].data;
					if (materials[mtlindex].color.w != 0.0f)
					{
						const float coef = materials[mtlindex].color.w * (1.0f - (far - near));
						color *= coef + (1.0f - coef) * materials[mtlindex].color;
					}
					else
						return (false);
				}
			}
			else
			{
				stack[top++] = nodes[index].children;
				stack[top++] = nodes[index].children + 1;
			}
		}
	}
	while (--top >= 0);

	return (true);
}

float		compute_aocclusion(in const vec4 origin, in const vec4 normal)
{
	float	aocclusion = 8.0f;

	Aabb	aabb;
	int		stack[SEARCHSTACKSIZE];
	int		top = 0;

	aabb.bottom = (origin - vec4(1.0f, 1.0f, 1.0f, 0.0f)) + normal * (1.0f + FLT_EPSILON);
	aabb.top = aabb.bottom + vec4(2.0f, 2.0f, 2.0f, 0.0f);

	stack[top] = 0;
	do
	{
		const int	index = stack[top];

		if (nodes[index].bottom.x < aabb.top.x && nodes[index].bottom.y < aabb.top.y && nodes[index].bottom.z < aabb.top.z &&
			nodes[index].top.x > aabb.bottom.x && nodes[index].top.y > aabb.bottom.y && nodes[index].top.z > aabb.bottom.z)
		{
			if (nodes[index].children == -1)
			{
				const vec3	a = min(nodes[index].top, aabb.top.xyz) - max(nodes[index].bottom, aabb.bottom.xyz);
				aocclusion -= a.x * a.y * a.z * (1.0f - materials[nodes[index].data].color.w);
			}
			else
			{
				stack[top++] = nodes[index].children;
				stack[top++] = nodes[index].children + 1;
			}
		}
	}
	while (--top >= 0);

	return (max(aocclusion / 8.0f, 0.0f) * 0.5f + 0.5f);
}

void		main()
{
	float	coefstack[RAYSTACKSIZE];
	Ray		raystack[RAYSTACKSIZE];
	int		avoidstack[RAYSTACKSIZE];
	int		top = 0;
	int		maxray = RAYSTACKSIZE;

	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	coefstack[top] = 1.0f;
	raystack[top].origin = camera.position;
	raystack[top].direction = normalize(camera.forward + camera.right * (gl_FragCoord.x - camera.half_resolution.x) + camera.up * (gl_FragCoord.y - camera.half_resolution.y));
	avoidstack[top] = -1;

	do
	{
		const Ray	ray = raystack[top];
		Impact		impact;

		impact.index = avoidstack[top];
		impact.near = INFINITY;
		if (find_closest(ray, impact))
		{
			vec4	accumulated_color = vec4(0.4f, 0.4f, 0.5f, 0.25f);
			Ray		shadowray;

			shadowray.origin = ray.origin + ray.direction * impact.near;

			for (uint i = 0; i < lights_number; ++i)
			{
				shadowray.direction = lights[i].position - shadowray.origin;
				const float	distance = length(shadowray.direction);
				shadowray.direction /= distance;

				const float	received_power = dot(impact.normal, shadowray.direction) * (lights[i].color.w / distance);
				vec4		tcolor = lights[i].color;
				if (received_power > FLT_EPSILON && compute_shadow(shadowray, distance, impact.index, tcolor))
				{
					accumulated_color.xyz += tcolor.xyz * received_power;
					accumulated_color.w += received_power;
				}
			}

			const float aocclusion = compute_aocclusion(shadowray.origin, impact.normal);
			const int	mtlindex = nodes[impact.index].data;
			const float	coef = coefstack[top];

			color += materials[mtlindex].color * accumulated_color * (1.0f - materials[mtlindex].reflection) * (1.0f - materials[mtlindex].color.w) * aocclusion * coefstack[top];
		
			if ((coefstack[top] = coef * materials[mtlindex].reflection) > FLT_EPSILON && maxray != 0)
			{
				--maxray;
				raystack[top].origin = shadowray.origin;
				raystack[top].direction = ray.direction - impact.normal * dot(ray.direction, impact.normal) * 2.0f;
				avoidstack[top++] = impact.index;
			}

			if ((coefstack[top] = coef * materials[mtlindex].color.w) > FLT_EPSILON && maxray != 0)
			{
				const float	n = 1.0f / materials[mtlindex].refraction;
				const float c = -dot(impact.normal, ray.direction);
				const float s = 1.0f - n * n * (1.0f - c * c);

				if (s > 0.0f)
				{
					Ray	refractionray;
					refractionray.origin = shadowray.origin;
					refractionray.direction = ray.direction * n + impact.normal * (n * c - s);
					int	idx;
					refractionray.origin += refractionray.direction * find_closest_s(refractionray, mtlindex, idx);
					refractionray.direction = -refractionray.direction;
					impact.near = INFINITY;
					impact.index = idx;
					find_closest(refractionray, impact);
					
					const float	no = materials[mtlindex].refraction;
					const float co = -dot(impact.normal, -refractionray.direction);
					const float so = 1.0f - no * no * (1.0f - co * co);
					if (s > 0.0f)
					{
						--maxray;
						raystack[top].origin = refractionray.origin + refractionray.direction * impact.near;
						raystack[top].direction = -refractionray.direction * no + -impact.normal * (no * co - so);
						avoidstack[top++] = impact.index;
					}
					//else inner reflection
				}
				//else outter reflection
			}
		}
	}
	while (--top >= 0);
	//float	l = dot(color.xyz, (float3)(0.2126f, 0.7152f, 0.0722f));
	//l = l / (l + 0.1f);
	//color *= l;
	color.w = 1.0f;
}

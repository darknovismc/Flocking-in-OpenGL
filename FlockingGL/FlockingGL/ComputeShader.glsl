#version 440 core
#extension GL_ARB_compute_variable_group_size :enable

layout(local_size_variable) in;
layout (std140 , binding=0) 
buffer Pos 
{
	vec4 Positions[];
};
layout (std140 , binding=1)
buffer Vel 
{
	vec4 Velocities[];
};
uniform float u_deltaTime;

void limit(inout vec3 value,float LIM)
{
	float len = length(value);
	if(len>LIM)
		value = (value/len)*LIM;
}

void limitVel(inout vec3 value,float LIM1,float LIM2)
{
	float len = length(value);
	if(len<LIM1)
		value = (value/len)*LIM1;
    else if(len>LIM2)
		value = (value/len)*LIM2;
}

void main()
{
	uint index = gl_GlobalInvocationID.x;
	vec3 pos = Positions[index].xyz;
	vec3 vel = Velocities[index].xyz;
	pos+=vel*u_deltaTime;
	if(pos.x > 1.0f)
	{
		vel.x=-abs(vel.x);
		pos.x=1.0f;
	}
	else if(pos.x < -1.0f)
	{
		vel.x=abs(vel.x);
		pos.x=-1.0f;
	}
	if(pos.y > 1.0f)
	{
		vel.y=-abs(vel.y);
		pos.y=1.0f;
	}
	else if(pos.y < -1.0f)
	{
		vel.y=abs(vel.y);
		pos.y=-1.0f;
	}
	if(pos.z > 1.0f)
	{
		vel.z=-abs(vel.z);
		pos.z=1.0f;
	}
	else if(pos.z < -1.0f)
	{
		vel.z=abs(vel.z);
		pos.z=-1.0f;
	}

	vec3 sumVel =vec3(0,0,0);
	vec3 sumPos =vec3(0,0,0);
	vec3 sumForce =vec3(0,0,0);
	vec3 diff;
	float num=0;
	for(int i =0;i<gl_NumWorkGroups.x*100;i++)
	{
		if(i!=index)
		{
			float dist = length(Positions[i].xyz-pos);
			if(dist<0.1f && dist>0)
			{
				sumVel += Velocities[i].xyz;
				sumPos += Positions[i].xyz;
				diff = pos - Positions[i].xyz;
				sumForce+= diff/dist;
				num++;
			}
		}
	}
	//alignment
	if(num>0)
		sumVel /= num;
	limit(sumVel,0.5f);
	vel+=(sumVel-vel)*u_deltaTime;

	//cohesion
	if(num>0)
		sumPos /= num;
	sumPos-=pos;
	limit(sumPos,0.2f);
	vel+=(sumPos-vel)*u_deltaTime;

	//separation
	if(num>0)
		sumForce /= num;
	limit(sumForce,0.2f);
	vel+=(sumForce-vel)*u_deltaTime;

	limitVel(vel,0.2f,2.0f);
	
	Positions[index].xyz = pos;
	Velocities[index].xyz = vel;
}
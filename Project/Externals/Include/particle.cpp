//
//  Particle.cpp
//  opengl
//
//  Created by app05 on 15-5-20.
//  Copyright (c) 2015年 app05. All rights reserved.
//

#include "Particle.h"

// 构造函数 //
CParticle::CParticle()
{
	data = NULL;
	numparticle = 0;
}

// 析构函数 //
CParticle::~CParticle()
{
	delete[]data;
	data = NULL;
}

// 创建一个包含num个元素的粒子数组 //
int CParticle::Create(long num)
{
	// 删除粒子数组 //
	if (data)
		delete[]data;

	// 创建数组 //
	if (data = new Particle[num])
	{
		memset(data, 0, sizeof(Particle)*numparticle);
		numparticle = num;

		// 返回粒子个数 //
		return numparticle;
	}
	return 0;
}


// 设置和获取位置Position的函数实现 //
int CParticle::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	for (int index = 0; index < numparticle; ++index)
	{
		data[index].x = x;
		data[index].y = y;
		data[index].z = z;
	}
	return true;
}

int CParticle::SetPosition(GLint index, GLfloat x, GLfloat y, GLfloat z)
{
	if (index >= 0 && index < numparticle)
	{
		data[index].x = x;
		data[index].y = y;
		data[index].z = z;
		return true;
	}
	return false;
}

int CParticle::GetPosition(GLint index, GLfloat &x, GLfloat &y, GLfloat &z)
{
	if (index >= 0 && index < numparticle)
	{
		x = data[index].x;
		y = data[index].y;
		z = data[index].z;
		return true;
	}
	return false;
}
// 设置和获取加速度Acceleration的函数实现 //
int CParticle::SetAcceleration(GLfloat ax, GLfloat ay, GLfloat az)
{
	for (int index = 0; index < numparticle; ++index)
	{
		data[index].ax = ax;
		data[index].ay = ay;
		data[index].az = az;
	}
	return true;
}

int CParticle::SetAcceleration(GLint index, GLfloat ax, GLfloat ay, GLfloat az)
{
	if (index >= 0 && index < numparticle)
	{
		data[index].ax = ax;
		data[index].ay = ay;
		data[index].az = az;
		return true;
	}
	return false;
}


int CParticle::GetAcceletation(GLint index, GLfloat &ax, GLfloat &ay, GLfloat &az)
{
	if (index >= 0 && index < numparticle)
	{
		ax = data[index].ax;
		ay = data[index].ay;
		az = data[index].az;
		return true;
	}
	return false;
}



// Velocity函数的实现 //
int CParticle::SetVelocity(GLfloat vx, GLfloat vy, GLfloat vz)
{
	for (int index = 0; index < numparticle; ++index)
	{
		data[index].vx = vx;
		data[index].vy = vy;
		data[index].vz = vz;
	}
	return true;
}

int CParticle::SetVelocity(GLint index, GLfloat vx, GLfloat vy, GLfloat vz)
{
	if (index >= 0 && index < numparticle)
	{
		data[index].vx = vx;
		data[index].vy = vy;
		data[index].vz = vz;
		return true;
	}
	return false;
}

int CParticle::GetVelocity(GLint index, GLfloat &vx, GLfloat &vy, GLfloat &vz)
{
	if (index >= 0 && index < numparticle)
	{
		vx = data[index].vx;
		vy = data[index].vy;
		vz = data[index].vz;
		return true;
	}
	return false;
}


// 获取粒子的所有属性 //
int CParticle::GetAll(int index,       //< 粒子的颜色 
	GLfloat &x, GLfloat &y, GLfloat &z,     //< 位置 
	GLfloat &vx, GLfloat &vy, GLfloat &vz,  //< 速度 
	GLfloat &ax, GLfloat &ay, GLfloat &az  //< 加速度 

)
{
	if (index >= 0 && index < numparticle)
	{
		x = data[index].x;
		y = data[index].y;
		z = data[index].z;
		vx = data[index].vx;
		vy = data[index].vy;
		vz = data[index].vz;
		ax = data[index].ax;
		ay = data[index].ay;
		az = data[index].az;
		return true;
	}
	return false;
}

// 设置粒子的所有属性 //
int CParticle::SetAll(int index,
	GLfloat x, GLfloat y, GLfloat z,        // 位置 
	GLfloat vx, GLfloat vy, GLfloat vz, // 速度 
	GLfloat ax, GLfloat ay, GLfloat az // 加速度 
)
{
	if (index >= 0 && index < numparticle)
	{
		data[index].x = x;
		data[index].y = y;
		data[index].z = z;
		data[index].vx = vx;
		data[index].vy = vy;
		data[index].vz = vz;
		data[index].ax = ax;
		data[index].ay = ay;
		data[index].az = az;
		return true;
	}
	return false;
}

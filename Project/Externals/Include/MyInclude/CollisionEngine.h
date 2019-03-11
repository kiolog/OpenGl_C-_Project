#pragma once

#ifndef COLLISIONENGINE_H
#define COLLISIONENGINE_H

#include"../Include.h"
#include<vector>
#include<string>
#include "GameObject.h"
#include <limits>
using namespace std;
using namespace glm;

class Face {
public:
	Face(vec3 _v3Point1,vec3 _v3Point2,vec3 _v3Point3) {
		m_v3Point1 = _v3Point1;
		m_v3Point2 = _v3Point2;
		m_v3Point3 = _v3Point3;

		m_v3Normal = normalize(cross(m_v3Point2 - m_v3Point1, m_v3Point3 - m_v3Point1));
	}
	vec3 m_v3Normal;
	vec3 m_v3Point1;
	vec3 m_v3Point2;
	vec3 m_v3Point3;
};

bool UpdateSimplexAndCheckOverlap(vector<vec3> &_ListTracePoint)
{
	bool bReturnValue = true;

	vector<vec3> ListTempPoint = _ListTracePoint;

	vec3 v3FaceNormal_DAB = cross(_ListTracePoint[1] - _ListTracePoint[3], _ListTracePoint[0] - _ListTracePoint[3]);
	vec3 v3FaceNormal_DBC = cross(_ListTracePoint[2] - _ListTracePoint[3], _ListTracePoint[1] - _ListTracePoint[3]);
	vec3 v3FaceNormal_DCA = cross(_ListTracePoint[0] - _ListTracePoint[3], _ListTracePoint[2] - _ListTracePoint[3]);

	vec3 v3DirectionTowardOrigin = -_ListTracePoint[3];
	if (dot(v3FaceNormal_DAB, v3DirectionTowardOrigin) > 0)
	{
		_ListTracePoint[0] = ListTempPoint[3];
		_ListTracePoint[1] = ListTempPoint[0];
		_ListTracePoint[2] = ListTempPoint[1];
		bReturnValue = false;
	}
	if (dot(v3FaceNormal_DBC, v3DirectionTowardOrigin) > 0)
	{
		_ListTracePoint[0] = ListTempPoint[3];
		_ListTracePoint[1] = ListTempPoint[1];
		_ListTracePoint[2] = ListTempPoint[2];
		bReturnValue = false;
	}
	if (dot(v3FaceNormal_DCA, v3DirectionTowardOrigin) > 0)
	{
		_ListTracePoint[0] = ListTempPoint[3];
		_ListTracePoint[1] = ListTempPoint[2];
		_ListTracePoint[2] = ListTempPoint[0];
		bReturnValue = false;
	}
	
	return bReturnValue;
}
vec3 GetDirectionTowardOrigin(vector<vec3> &_ListTracePoint)
{
	vec3 v3Direction = cross(_ListTracePoint[1] - _ListTracePoint[0], _ListTracePoint[2] - _ListTracePoint[0]);
	if (dot(v3Direction, -_ListTracePoint[0]) > 0)
	{
		vec3 v3TempPoint = _ListTracePoint[2];
		_ListTracePoint[2] = _ListTracePoint[1];
		_ListTracePoint[1] = v3TempPoint;
	}
	else
	{
		v3Direction = -v3Direction;
	}
	return v3Direction;
}
vec3 GetPointInDirection(vector<vec3> _ListVertex, vec3 _v3Direction)
{
	vec3 v3ReturnValue = vec3();
	float fMaxValue = FLT_MIN;
	int iListCount = _ListVertex.size();
	for (int i = 0; i < iListCount; ++i)
	{
		vec3 v3Position = _ListVertex[i];
		float fDotValue = dot(v3Position, _v3Direction);
		if (fDotValue > fMaxValue)
		{
			v3ReturnValue = v3Position;
			fMaxValue = fDotValue;
		}

	}
	return v3ReturnValue;
}
vec3 GetSupportPoint(vec3 _v3SourcePosiiton, vector<vec3> _ListSourceVertex, vec3 _v3TargetPosition, vector<vec3> _ListTargetVertex, vec3 _v3Direction)
{
	return (_v3SourcePosiiton + GetPointInDirection(_ListSourceVertex, _v3Direction)) - (_v3TargetPosition + GetPointInDirection(_ListTargetVertex, -_v3Direction));
}
bool GetGJKResult(vec3 _v3SourcePosition, vector<vec3> _ListSourceVertex, vec3 _v3TargetPosition, vector<vec3> _ListTargetVertex)
{
	bool bReturnValue = false;
	vector<vec3> ListTracePoint(4);
	vec3 v3TraceDirection = _v3TargetPosition - _v3SourcePosition;
	ListTracePoint[0] = GetSupportPoint(_v3SourcePosition, _ListSourceVertex, _v3TargetPosition, _ListTargetVertex, v3TraceDirection);
	/*Debug.Log("SourcePosition : " + _v3SourcePosition);
	Debug.Log("TargetPosition : " + _v3TargetPosition);
	Debug.Log("InitDirection : " + v3TraceDirection);
	Debug.Log("First SupportPoint : " + ListTracePoint[0]);*/
	v3TraceDirection = -ListTracePoint[0];
	ListTracePoint[1] = GetSupportPoint(_v3SourcePosition, _ListSourceVertex, _v3TargetPosition, _ListTargetVertex, v3TraceDirection);
	if (dot(ListTracePoint[1], v3TraceDirection) >= 0)
	{
		//Debug.Log("Enter");
		vec3 v3PointZero = ListTracePoint[0];
		vec3 v3PointOne = ListTracePoint[1];
		v3TraceDirection = cross(cross(v3PointZero - v3PointOne, -v3PointOne), v3PointZero - v3PointOne);
		if (v3TraceDirection == vec3())
		{
			vec3 v3ForwardVector = v3PointOne - v3PointZero;
			v3TraceDirection = vec3(-v3ForwardVector.y, v3ForwardVector.x, v3ForwardVector.z);
		}
		ListTracePoint[2] = GetSupportPoint(_v3SourcePosition, _ListSourceVertex, _v3TargetPosition, _ListTargetVertex, v3TraceDirection);
		/*Debug.Log("First Vertex : " + ListTracePoint[0]);
		Debug.Log("Second Vertex : " + ListTracePoint[1]);
		Debug.Log("TraceDirection : " + v3TraceDirection);*/

		if (dot(ListTracePoint[2], v3TraceDirection) >= 0)
		{
			while (true)
			{
				v3TraceDirection = GetDirectionTowardOrigin(ListTracePoint);
				vec3 v3SupportPoint = GetSupportPoint(_v3SourcePosition, _ListSourceVertex, _v3TargetPosition, _ListTargetVertex, v3TraceDirection);
				if (dot(v3TraceDirection, v3SupportPoint) > 0)
				{
					//Debug.Log("Calculate");
					ListTracePoint[3] = v3SupportPoint;
					bReturnValue = UpdateSimplexAndCheckOverlap(ListTracePoint);
					//Simplex contains the origin
					if (bReturnValue)
					{
						bReturnValue = true;
						break;
					}
				}
				else
				{
					bReturnValue = false;
					break;
				}
			}
		}
	}
	else if (dot(ListTracePoint[1], v3TraceDirection) == 0)
	{
		bReturnValue = true;
	}
	return bReturnValue;
}


#endif
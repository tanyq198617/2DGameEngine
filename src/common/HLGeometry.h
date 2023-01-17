//
//  HLGeometry.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-18.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLGEOMETRY_H__
#define __HLGEOMETRY_H__

#include "HLMacros.h"
#include <math.h>

NS_HL_BEGIN

class HLPoint
{
public:
    float x;
    float y;
public:
    HLPoint(float x, float y):x(x),y(y){}
    HLPoint():x(0),y(0){}
    
    HLPoint operator+(const HLPoint &point) const
    {
        return HLPoint(x+point.x, y+point.y);
    }
    
    HLPoint operator+(const float &s) const
    {
        return HLPoint(x+s, y+s);
    }
    
    HLPoint operator-(const HLPoint &point) const
    {
        return HLPoint(x-point.x, y-point.y);
    }
    
    HLPoint operator-() const
    {
        return HLPoint(-x, -y);
    }
    
    HLPoint operator*(const float &s) const
    {
        return HLPoint(x*s, y*s);
    }
    
    HLPoint operator/(const float &s) const
    {
        return HLPoint(x/s, y/s);
    }
    
    HLPoint & operator*=(const float &s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    
    HLPoint & operator/=(const float &s)
    {
        x /= s;
        y /= s;
        return *this;
    }
    
    HLPoint & operator+=(const HLPoint &point)
    {
        x += point.x;
        y += point.y;
        return *this;
    }
    
    HLPoint & operator+=(const float &s)
    {
        x += s;
        y += s;
        return *this;
    }
    
    bool operator==(const HLPoint &point) const
    {
        return x==point.x && y==point.y;
    }
    
    bool operator!=(const HLPoint &point) const
    {
        return !(*this == point);
    }
    
    bool isFuzzyEqualToPoint(const HLPoint &point, float var) const
    {
        if (x - var <= point.x && point.x <= x + var) 
            if (y - var <= point.y && point.y <= y + var) 
                return true;
        return false;
    }
    
    float dot(const HLPoint &point) const
    {
        return x*point.x+y*point.y;
    }
    
    float cross(const HLPoint &point) const
    {
        return x*point.y-y*point.x;
    }
    
    HLPoint perp() const
    {
        return HLPoint(-y, x);
    }
    
    HLPoint rperp() const
    {
        return HLPoint(y, -x);
    }
    
    HLPoint project(const HLPoint &point) const
    {
        return point*(dot(point)/point.dot(point));
    }
    
    HLPoint rotate(const HLPoint &point) const
    {
        return HLPoint(x*point.x-y*point.y, x*point.y+y*point.x);
    }
    
    HLPoint unrotate(const HLPoint &point) const
    {
        return HLPoint(x*point.x+y*point.y, x*point.y-y*point.x);
    }
    
    float length() const
    {
        return sqrtf(dot(*this));
    }
    
    HLPoint normalize() const
    {
        return (*this)*(1.0f/length());
    }
    
    static float distance(const HLPoint &v1, const HLPoint &v2)
    {
        return (v1-v2).length();
    }
    
    static HLPoint midpoint(const HLPoint &v1, const HLPoint &v2)
    {
        return (v1+v2)/2;
    }
    
    static float angle(const HLPoint& a, const HLPoint& b)
    {
        float angle = acosf(a.normalize().dot(b.normalize()));
        if (fabs(angle) < FLT_EPSILON) {
            return 0.f;
        }
        return angle;
    }
};

class HLSize
{
public:
    float width;
    float height;
public:
    HLSize():width(0),height(0){};
    HLSize(float width, float height):width(width), height(height) {}
    
    bool operator==(const HLSize &size) const
    {
        return width==size.width && height==size.height;
    }
    
    bool operator!=(const HLSize &size) const
    {
        return !(*this == size);
    }
    
    bool operator>(const HLSize &size) const
    {
        return width*height > size.width*size.height;
    }
    
    bool operator<(const HLSize &size) const
    {
        return width*height < size.width*size.height;
    }
    
    HLSize operator*(const float &s) const
    {
        return HLSize(width*s, height*s);
    }
        
    HLSize operator/(const float &s) const
    {
        return HLSize(width/s, height/s);
    }
    
    HLSize & operator*=(const float &s) 
    {
        width *= s;
        height *= s;
        return *this;
    }
};

class HLRect
{
public:
    HLPoint origin;
    HLSize size;
public:
    HLRect():origin(0,0), size(0,0) {}
    HLRect(float x, float y, float width, float height):origin(x, y), size(width, height) {}
    HLRect(HLPoint origin, HLSize size):origin(origin), size(size) {}
    
    bool operator==(const HLRect &rect)
    {
        return origin==rect.origin && size==rect.size;
    }
    
    bool operator!=(const HLRect &rect)
    {
        return !(*this==rect);
    }
    
    bool containsPoint(const HLPoint &point)
    {
        if (point.x >= origin.x && point.x <= (origin.x+size.width)
            && point.y >= origin.y && point.y <= (origin.y+size.height)) {
            return true;
        }
        return false;
    }
    
    bool intersectsRect(const HLRect &rect)
    {
        return !((origin.x+size.width) < rect.origin.x ||
                 rect.origin.x+rect.size.width < origin.x ||
                 origin.y+size.height < rect.origin.y ||
                 rect.origin.y+rect.size.height < origin.y);
    }
};

const HLPoint HLPointZero(0, 0);

const HLSize HLSizeZero(0, 0);

const HLRect HLRectZero(0,0,0,0);

class HLPoint3D
{
public:
    float x;
    float y;
    float z;
public:
    HLPoint3D(float x, float y, float z):x(x),y(y),z(z){}
    HLPoint3D():x(0),y(0),z(0){}
    
    HLPoint3D& operator=(const HLPoint3D& point)
    {
        x = point.x;
        y = point.y;
        z = point.z;
        return *this;
    }
    
    HLPoint3D& operator=(const float f)
    {
        x = f;
        y = f;
        z = f;
        return *this;
    }
    
    bool operator==(const HLPoint3D &point) const
    {
        return x==point.x && y==point.y&&z==point.z;
    }
    
    bool operator!=(const HLPoint3D &point) const
    {
        return !(*this == point);
    }
    
    HLPoint3D operator+(const HLPoint3D& point) const
    {
        return HLPoint3D(x+point.x, y+point.y, z+point.z);
    }
    
    HLPoint3D operator-(const HLPoint3D& point) const
    {
        return HLPoint3D(x-point.x, y-point.y, z-point.z);
    }
    
    HLPoint3D operator/(const float &f) const
    {
        return HLPoint3D(x/f, y/f, z/f);
    }
    
    HLPoint3D operator/(const HLPoint3D &point) const
    {
        return HLPoint3D(x/point.x, y/point.y, z/point.z);
    }
    
    HLPoint3D operator*(const float &f) const
    {
        return HLPoint3D(x*f, y*f, z*f);
    }
    
    HLPoint3D operator*(const HLPoint3D &point) const
    {
        return HLPoint3D(x*point.x, y*point.y, z*point.z);
    }
    
    const HLPoint3D& operator+() const
    {
        return *this;
    }
    
    HLPoint3D operator-() const
    {
        return HLPoint3D(-x, -y, -z);
    }
    
    inline friend HLPoint3D operator*(const float f, const HLPoint3D& point)
    {
        return HLPoint3D(f*point.x, f*point.y, f*point.z);
    }
    
    inline friend HLPoint3D operator/(const float f, const HLPoint3D& point)
    {
        return HLPoint3D(f/point.x, f/point.y, f/point.z);
    }
    
    HLPoint3D& operator+=(const HLPoint3D& point)
    {
        x += point.x;
        y += point.y;
        z += point.z;
        
        return *this;
    }
    
    HLPoint3D& operator+=(const float f)
    {
        x += f;
        y += f;
        z += f;
        
        return *this;
    }
    
    HLPoint3D& operator-=(const HLPoint3D& point)
    {
        x -= point.x;
        y -= point.y;
        z -= point.z;
        
        return *this;
    }
    
    HLPoint3D& operator-=(const float f)
    {
        x -= f;
        y -= f;
        z -= f;
        
        return *this;
    }
    
    HLPoint3D& operator*=(const HLPoint3D& point)
    {
        x *= point.x;
        y *= point.y;
        z *= point.z;
        
        return *this;
    }
    
    HLPoint3D& operator*=(const float f)
    {
        x *= f;
        y *= f;
        z *= f;
        
        return *this;
    }
    
    HLPoint3D& operator/=(const HLPoint3D& point)
    {
        x /= point.x;
        y /= point.y;
        z /= point.z;
        
        return *this;
    }
    
    HLPoint3D& operator/=(const float f)
    {
        x /= f;
        y /= f;
        z /= f;
        
        return *this;
    }
    
    float length() const
    {
        return sqrtf(x*x + y*y + z*z);
    }
    
    float squaredLength() const
    {
        return x*x + y*y + z*z;
    }
    
    float distance(const HLPoint3D& point) const
    {
        return (*this - point).length();
    }
    
    float squaredDistance(const HLPoint3D& point) const
    {
        return (*this - point).squaredLength();
    }
    
    float dotProduct(const HLPoint3D& point) const
    {
        return x*point.x + y*point.y + z*point.z;
    }
    
    float absDotProduct(const HLPoint3D& point) const
    {
        return fabsf(x*point.x) + fabsf(y*point.y) + fabsf(z*point.z);
    }
    
    HLPoint3D normalize() const
    {
        float len = length();
        if (len > 0)
            return (*this)*(1.0f/length());
        return *this;
    }
    
    HLPoint3D crossProduct(const HLPoint3D point) const
    {
        return HLPoint3D(
            y*point.z - z*point.y,
            z*point.x - x*point.z,
            x*point.y - y*point.x
        );
    }
    
    HLPoint3D midPoint(const HLPoint3D& point) const
    {
        return HLPoint3D(
            (x+point.x)*0.5f,
            (y+point.y)*0.5f,
            (z+point.z)*0.5f
        );
    }
};
        
const HLPoint3D HLPoint3DZero(0, 0, 0);


NS_HL_END

#endif

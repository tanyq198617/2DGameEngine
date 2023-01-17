//
//  HLPolygonHitTestComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-2-6.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLPolygonHitTestComponent.h"
#include "HLUtil.h"
#include "HLResourceManager.h"

NS_HL_BEGIN

INIT_FAMILYID(HLPolygonHitTestComponent)

PROPERTIES_INITIAL(HLPolygonHitTestComponent)
REGISTER_PROPERTY(HLPolygonHitTestComponent, std::string, outlinefile)
REGISTER_PROPERTY(HLPolygonHitTestComponent, std::vector<HLPoint>, outlinepoints)

void HLPolygonHitTestComponent::onActive()
{
    mEntity->onHitTest = newDelegate(this, &HLPolygonHitTestComponent::hitTest);
}

void HLPolygonHitTestComponent::onDeactive()
{
    if (mEntity->onHitTest == newDelegate(this, &HLPolygonHitTestComponent::hitTest))
    {
        mEntity->onHitTest.clear();
    }
    
}

void HLPolygonHitTestComponent::set_outlinefile(std::string file)
{
//    char path[MAX_PATH];
//    get_full_path(file.c_str(), path);
//    HLFileData data(path, "rb");
    HLFileData* data = HLResourceManager::getSingleton()->getFileData(file.c_str());
    HLASSERT(data, "%s file read error", file.c_str());
    int32_t count = *(reinterpret_cast<int32_t*>(data->buffer));
    float * ptr = reinterpret_cast<float*>(data->buffer + sizeof(int32_t));
    m_outlinepoints.clear();
    m_outlinepoints.reserve(count);
    for (int i = 0; i < count; i++)
    {
        float x = *ptr++;
        float y = *ptr++;
        m_outlinepoints.push_back(HLPoint(x, y));
    }
    delete data;
    m_outlinefile = file;
}

std::string HLPolygonHitTestComponent::get_outlinefile()
{
    return m_outlinefile;
}

void HLPolygonHitTestComponent::hitTest(kmMat4* transform, const HLPoint& point, bool* ret)
{
    if (m_outlinepoints.size() == 0)
    {
        *ret = false;
		return;
    }
    kmMat4 transform1;
    kmMat4Inverse(&transform1, transform);
    float localx = (float)((double)transform1.mat[0]*point.x+(double)transform1.mat[4]*point.y+transform1.mat[12]);
    float localy = (float)((double)transform1.mat[1]*point.x+(double)transform1.mat[5]*point.y+transform1.mat[13]);
    
    *ret = pnpoly((int)m_outlinepoints.size(), &m_outlinepoints[0], HLPoint(localx, localy));
}

NS_HL_END

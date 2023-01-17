//
//  HLGUIViewComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/15/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLGUIViewComponent.h"
#include "kazmath/mat4.h"
#include "HLTransform2DComponent.h"

NS_HL_BEGIN

INIT_FAMILYID(HLGUIViewComponent)

PROPERTIES_INITIAL(HLGUIViewComponent)
REGISTER_PROPERTY(HLGUIViewComponent, gui::HLView*, view)

void HLGUIViewComponent::onActive()
{
    mEntity->onDraw += newDelegate(this, &HLGUIViewComponent::onDraw);
    mEntity->onTouchEvent += newDelegate(this, &HLGUIViewComponent::onTouches);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    mEntity->onMouseEvent += newDelegate(this, &HLGUIViewComponent::onMouseEvent);
#endif
    mEntity->onHitTest = newDelegate(this, &HLGUIViewComponent::hitTest);
}

void HLGUIViewComponent::onDeactive()
{
    mEntity->onDraw -= newDelegate(this, &HLGUIViewComponent::onDraw);
    mEntity->onTouchEvent -= newDelegate(this, &HLGUIViewComponent::onTouches);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    mEntity->onMouseEvent -= newDelegate(this, &HLGUIViewComponent::onMouseEvent);
#endif
    if (mEntity->onHitTest == newDelegate(this, &HLGUIViewComponent::hitTest))
    {
        mEntity->onHitTest.clear();
    }
    
}

void HLGUIViewComponent::hitTest(kmMat4* transform, const HLPoint& p, bool* ret)
{
    if (!m_view)
    {
        *ret = false;
        return ;
    }
    if (mEntity->hasComponent<HLTransform2DComponent>())
    {
        kmMat4Assign(mViewContainer->getTransform(), transform);
    }
    *ret = mViewOnTouch = m_view->hitTest(p);
}

gui::HLView* HLGUIViewComponent::get_view()
{
    return m_view;
}

void HLGUIViewComponent::set_view(gui::HLView* view)
{
    if (m_view && m_view != view)
    {
        mViewContainer->removeSubview(m_view);
    }
    m_view = view;
    mViewContainer->addSubview(m_view);
    m_view->setPosition(HLPointZero);
}

void HLGUIViewComponent::onDraw()
{
    if (!m_view)
    {
        return;
    }
    if (mEntity->hasComponent<HLTransform2DComponent>())
    {
        HLSize size = mEntity->getProperty<HLSize>("size");
        if (mViewContainer->getSize() != size)
        {
            mViewContainer->setSize(size);
            m_view->setSize(size);
        }
        kmMat4Assign(mViewContainer->getTransform(), mEntity->getComponent<HLTransform2DComponent>()->nodeToWorldTransform());
    }
    m_view->visit();
}

void HLGUIViewComponent::onTouches(hoolai::HLTouchEvent* event)
{
    if (mViewOnTouch)
    {
        mViewOnTouch->handleTouchEvent(event);
    }
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void HLGUIViewComponent::onMouseEvent(hoolai::HLMouseEvent* event)
{
    if (mViewOnTouch)
    {
        mViewOnTouch->handleMouseEvent(event);
    }
}
#endif

NS_HL_END

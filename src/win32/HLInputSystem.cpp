#include "HLEditBox.h"
using namespace hoolai::gui;
using namespace hoolai;

static HLEditBox* __sysHLEditBox = NULL;

void __activeTextInputWithEditBox(HLEditBox* editBox)
{
    std::list<HLView*> mSubViews = editBox->getParentView()->getSubviews();
    std::list<HLView*>::const_iterator itr = mSubViews.begin();
    
    if(__sysHLEditBox)
    {
        __sysHLEditBox->resignActive();
    }
    while (itr != mSubViews.end())
    {
        if(*itr)
        {
            if ((*itr) == editBox)
            {
                __sysHLEditBox = editBox;
            }
        }
        ++itr;
    }
}

void __deactiveTextInput()
{
    __sysHLEditBox = NULL;
}

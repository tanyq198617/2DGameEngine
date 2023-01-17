//
//  AppDelegate.hpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __APPDELEGATE_HPP__
#define __APPDELEGATE_HPP__

#include "HLApplication.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLSceneManager.h"
#include "HLEntitySystem.h"
#include "HLTransform2DComponent.h"
#include "HLSpriteComponent.h"
#include "HLTexture.h"
#include "HLColorComponent.h"
#include "HLSpriteFrame.h"
#include "HLAnimateSystem.h"
#include "HLTouchDispatcher.h"
#include "HLParticleExampleComponent.h"
#include "JSComponent.h"
#include "JSScriptingCore.h"
#include "HLProgram.h"
#include "HLFileUtil.h"
#include "HLResourceManager.h"

#include "HLTimer.h"

#include "HLGUIManager.h"
#include "HLLabel.h"
#include "HLRichLabel.h"
#include "HLImageView.h"
#include "HLButton.h"
#include "HLScrollView.h"
#include "HLAlertView.h"
#include "HLTableView.h"
#include "HLTableViewCell.h"
#include "HLSmartLabel.h"
#include "HLSkeletonComponent.h"
#include "HLRenderTexture.h"
#include "HLDrawingPrimitives.h"
#include "HLProgressRadialView.h"

#include "HLViewLoader.h"

#include "HLParticleExampleComponent.h"

#include "HLNotificationCenter.h"

#include <string>
#include <map>

//#include "HttpTest.h"
#include "HLEditBox.h"
#include "rapidjson/document.h"

RegJSTypeFunc jsTypeRegFuncs[] =
{
    NULL,
};

/*
struct mesh
{
    unsigned int nude_objNumVerts;
    float *nude_objVerts;
    unsigned int num_vertIndexes;
    int *vertIndexes;
    unsigned int vao;
    unsigned int vbo;
};

class TestComponent : public hoolai::HLComponent
{
    PROPERTIES_DECLARE
private:
    hoolai::HLPoint touchPoint;
    struct mesh * meshes;
    int num_meshes;
    
public:
    virtual bool canHandleEvent(hoolai::HLComponentEvent event)
    {
        if (event == hoolai::kHLComponentEventTouch)
        {
            return true;
        }
        if (event == hoolai::kHLComponentEventHitTest)
        {
            return true;
        }
        //        if (event == hoolai::kHLComponetEventUpdate)
        //        {
        //            return true;
        //        }
        if (event == hoolai::kHLComponentEventDraw)
        {
            return true;
        }
        return false;
    }
    
    virtual void onAddedToEntity(hoolai::HLEntity* entity)
    {
        entity->setUserInteractionEnabled(true);
        char path[256];
        hoolai::get_full_path("nude_obj.obj", path);
        hoolai::HLFileData data(path, "r");
        char *line = strtok( ( char * )data.buffer, "\n" );
        num_meshes = 0;
        meshes = NULL;
        char last;
        while (line)
        {
            float x,y,z;
            int vertex_index[3];
            int uv_index[3];
            int normal_index[3];
            if( sscanf( line, "v %f %f %f", &x, &y, &z ) == 3 )
			{
                if (last != 'v')
                {
                    num_meshes++;
                    meshes = (struct mesh*)realloc(meshes, sizeof(struct mesh)*num_meshes);
                    meshes[num_meshes-1].nude_objNumVerts = 0;
                    meshes[num_meshes-1].nude_objVerts = NULL;
                    meshes[num_meshes-1].vertIndexes = NULL;
                    meshes[num_meshes-1].num_vertIndexes = 0;
                }
                meshes[num_meshes-1].nude_objVerts = (float*)realloc(meshes[num_meshes-1].nude_objVerts, sizeof(float)*3*(meshes[num_meshes-1].nude_objNumVerts+1));
                meshes[num_meshes-1].nude_objVerts[meshes[num_meshes-1].nude_objNumVerts*3] = x;
                meshes[num_meshes-1].nude_objVerts[meshes[num_meshes-1].nude_objNumVerts*3+1] = y;
                meshes[num_meshes-1].nude_objVerts[meshes[num_meshes-1].nude_objNumVerts*3+2] = z;
                meshes[num_meshes-1].nude_objNumVerts++;
                //                nude_objVerts = (float*)realloc(nude_objVerts, sizeof(float)*3*(nude_objNumVerts+1));
                //                nude_objVerts[nude_objNumVerts*3] = x;
                //                nude_objVerts[nude_objNumVerts*3+1] = y;
                //                nude_objVerts[nude_objNumVerts*3+2] = z;
                //                nude_objNumVerts++;
            }
            else if (sscanf( line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_index[ 0 ],
                            &uv_index    [ 0 ],
                            &normal_index[ 0 ],
                            &vertex_index[ 1 ],
                            &uv_index    [ 1 ],
                            &normal_index[ 1 ],
                            &vertex_index[ 2 ],
                            &uv_index    [ 2 ],
                            &normal_index[ 2 ] ) == 9)
            {
                meshes[num_meshes-1].vertIndexes = (int*)realloc(meshes[num_meshes-1].vertIndexes, sizeof(int)*3*(meshes[num_meshes-1].num_vertIndexes+1));
                meshes[num_meshes-1].vertIndexes[meshes[num_meshes-1].num_vertIndexes*3] = vertex_index[0];
                meshes[num_meshes-1].vertIndexes[meshes[num_meshes-1].num_vertIndexes*3+1] = vertex_index[1];
                meshes[num_meshes-1].vertIndexes[meshes[num_meshes-1].num_vertIndexes*3+2] = vertex_index[2];
                meshes[num_meshes-1].num_vertIndexes++;
            }
        next_line:
            last = line[0];
            line = strtok( NULL, "\n" );
        }
        
        for (int i=0; i<num_meshes; i++)
        {
            glGenVertexArrays(1, &meshes[i].vao);
            glBindVertexArray(meshes[i].vao);
            
            glGenBuffers( 1, &meshes[i].vbo );
            
            glBindBuffer( GL_ARRAY_BUFFER, meshes[i].vbo );
            
            glBufferData( GL_ARRAY_BUFFER,
                         meshes[i].nude_objNumVerts*3,
                         meshes[i].nude_objVerts,
                         GL_STATIC_DRAW );
            
            free( meshes[i].nude_objVerts );
            
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
        }
    }
    
    virtual void onRemovedFromEntity(hoolai::HLEntity* entity)
    {
        
    }
    
    virtual hoolai::event_handler_func getEventHandler(hoolai::HLComponentEvent event)
    {
        hoolai::event_handler_func func;
        if (event == hoolai::kHLComponentEventTouch)
        {
            func.touchfunc = (hoolai::touchhandler_pointer)&TestComponent::onTouches;
        }
        if (event == hoolai::kHLComponentEventHitTest)
        {
            func.hittestfunc = (hoolai::hittest_pointer)&TestComponent::hittest;
        }
        if (event == hoolai::kHLComponentEventUpdate)
        {
            func.updatefunc = (hoolai::updatefunc_pointer)&TestComponent::update;
        }
        if (event == hoolai::kHLComponentEventDraw)
        {
            func.drawfunc = (hoolai::drawfunc_pointer)&TestComponent::draw;
        }
        return func;
    }
    
    void draw(hoolai::HLEntity* e)
    {
        hoolai::HLProgram *program = hoolai::HLProgramManager::currentManager()->getProgram(kShaderPositionColor);
        program->use();
        program->setUniformForModelViewProjectionMatrix();
        
        hoolai::hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        
        glCullFace( GL_FRONT );
        for (int i=0;i<num_meshes;i++) {
            GLubyte colors[meshes[i].nude_objNumVerts*sizeof(GLubyte)*4];
            memset(colors, 255, meshes[i].nude_objNumVerts*sizeof(GLubyte)*4);
            
            glEnableVertexAttribArray( hoolai::kVertexAttrib_Position );
            glEnableVertexAttribArray( hoolai::kVertexAttrib_Color );
            glVertexAttribPointer(hoolai::kVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, meshes[i].nude_objVerts);
            glVertexAttribPointer(hoolai::kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)colors);
            glDrawArrays(GL_TRIANGLES, 0, meshes[i].nude_objNumVerts);
        }
        glCullFace( GL_BACK );
        for (int i=0;i<num_meshes;i++) {
            GLubyte colors[meshes[i].nude_objNumVerts*sizeof(GLubyte)*4];
            memset(colors, 255, meshes[i].nude_objNumVerts*sizeof(GLubyte)*4);
            
            glEnableVertexAttribArray( hoolai::kVertexAttrib_Position );
            glEnableVertexAttribArray( hoolai::kVertexAttrib_Color );
            glVertexAttribPointer(hoolai::kVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, meshes[i].nude_objVerts);
            glVertexAttribPointer(hoolai::kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)colors);
            glDrawArrays(GL_TRIANGLES, 0, meshes[i].nude_objNumVerts);
        }
        
    }
    
    void update(float dt)
    {
        //        hoolai::HLEntity * entity = hoolai::HLEntityManager::sharedEntityManager()->getEntity("main_camera");
        //        hoolai::HLPoint3D pos = entity->getProperty<hoolai::HLPoint3D>("position");
        //        pos.z += dt;
        //        entity->setProperty("position", pos);
    }
    
    bool hittest(hoolai::HLEntity*, kmMat4* transform, hoolai::HLPoint)
    {
        return true;
    }
    
    void onTouches(hoolai::HLTouchEvent* event)
    {
        if (event->type == hoolai::kHLTouchesBegin)
        {
            touchPoint = event->touches[0].location;
        }
        else if (event->type == hoolai::kHLTouchesMove)
        {
            float rotationX = mEntity->getProperty<float>("rotationX");
            float rotationZ = mEntity->getProperty<float>("rotationZ");
            
            rotationZ += (touchPoint.x - event->touches[0].location.x);
            rotationX += -(touchPoint.y - event->touches[0].location.y);
            
            mEntity->setProperty("rotationX", rotationX);
            mEntity->setProperty("rotationZ", rotationZ);
            
            touchPoint = event->touches[0].location;
        }
    }
    
    
    static const hoolai::FamilyId familyId;
};

INIT_FAMILYID(TestComponent)
PROPERTIES_INITIAL(TestComponent)
*/
int countNumberToReload=17;
hoolai::gui::HLTableView *__tableView=NULL;

void buttonClicked(hoolai::gui::HLButton* sender)
{
    printf("button clicked\n");
    hoolai::HLNotificationCenter::defaultCenter()->postNotification("notifytest", NULL);
//    hoolai::gui::HLAlertView * alert = hoolai::gui::HLAlertView::newAlert("Test", "button clicked!", "Cancel", NULL);
//    alert->show();
    
//    countNumberToReload++;
//    __tableView->reloadData();
}

void richlabelDelegate(hoolai::gui::HLRichLabel*, std::string action, std::map<std::string, std::string> params)
{
    printf("%s %s\n", action.c_str(), params["a"].c_str());
}

void scrollViewDidScroll(hoolai::gui::HLScrollView* scrollView)
{
    printf("scrollViewDidScroll\n");
}

void handleTouch(hoolai::gui::HLView* view, hoolai::HLTouchEvent* touch)
{
    hoolai::gui::HLViewDragSource * dragsource = new hoolai::gui::HLViewDragSource();
    dragsource->setDraggingView(view);
    dragsource->doDragDrop(touch->touches[0].location);
}

void handleDragEvent(hoolai::gui::HLView* view, const hoolai::gui::HLViewDraggingEvent& event, hoolai::gui::HLViewDragSource* dragSource, bool* ret)
{
    hoolai::gui::HLLabel *label = (hoolai::gui::HLLabel *)view;
    if (event.type == hoolai::gui::kHLViewDraggingEntered)
    {
        hoolai::color4B color;
        color.r = 0;
        color.g = 255;
        color.b = 255;
        color.a = 255;
        view->setBackgroundColor(color);
    }
    else if (event.type == hoolai::gui::kHLViewDraggingExited)
    {
        hoolai::color4B color;
        color.r = 255;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        view->setBackgroundColor(color);
    }
    else if (event.type == hoolai::gui::kHLViewDraggingDropped)
    {
        hoolai::color4B color;
        color.r = 255;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        view->setBackgroundColor(color);
        
      hoolai::gui::HLLabel* label2 =(hoolai::gui::HLLabel*)dragSource->getSourceView();
        label->setText(label2->getText());
    }
    else if (event.type == hoolai::gui::kHLViewDraggingAccepted)
    {
        
    }
    else if (event.type == hoolai::gui::kHLViewDraggingCanceled)
    {
        
    }
    *ret = true;
}

hoolai::gui::HLProgressRadialView* progressview;

#include "HLHttpDownloadManager.h"

void downloadprogress(std::string str, float progress, float speed)
{
    HLLOG("downloaded_size==>%.2f%%, download_speed==>%.2f k/s\n", progress*100, speed/1024);
}

void setProgress(hoolai::HLTimer* timer, float dt)
{
    float progress = progressview->getProgress();
    progress += 0.01;
    if (progress > 1)
    {
        progress = 0;
    }
    progressview->setProgress(progress);
}

class ApplicationDelegate:public hoolai::HLApplicationDelegate
{
public:
    virtual void applicationDidFinishLaunching()
    {
        hoolai::HLSceneManager *sceneMgr = new hoolai::HLSceneManager();
        
#if GAME_PROJECTION == PROJECTION_2D
        hoolai::HLDirector2D* director = hoolai::HLDirector2D::getSingleton();
#else
        hoolai::HLDirector3D* director = hoolai::HLDirector3D::sharedDirector();
#endif
        
        director->setDesignWinSize(hoolai::HLSize(320, 0));
        hoolai::HLResourceManager::getSingleton()->setResourceScale(director->getWinSizeScale());
        
        director->setDisplayStats(true);
        
        hoolai::HLResourceManager::getSingleton()->initialize();
        
        hoolai::HLFont::addFont("MSYH.hlf");
        hoolai::HLFont::getFont()->setRenderThresHold(-0.02);
        
        hoolai::color4B color = {255, 0, 0, 255};
        
        
#if defined(TARGET_MACOS) && defined(DEBUG)
        hoolai::JSScriptingCore::getSingleton()->enableDebugger("hltest");
#endif
        /*hoolai::HLHttpDownloadManager::getSingleton()->addDownloadTask("http://wtuscdn.kabam.com/test/patch_240_20150308_kabam_android.pak", "", hoolai::newDelegate(downloadprogress));*/
        
//        hoolai::gui::HLLabel *label_1 = new hoolai::gui::HLLabel(NULL, hoolai::HLRect(0,0,200,50));
//        label_1->setTextStroke(true);
//        label_1->setTextAlignment(hoolai::gui::kTextHAlignmentCenter | hoolai::gui::kTextVAlignmentBottom);
//        hoolai::color4B color;
//        color.r = color.g = color.b = color.a = 255;
//        label_1->setBackgroundColor(color);
//        label_1->setFontSize(12);
//        hoolai::HLEntity * entity = sceneMgr->getEntityManager()->createEntity("gui", "HLTransform2DComponent", "HLGUIViewComponent", NULL);
//        entity->setProperty("size", hoolai::HLSize(200, 50));
//        entity->setProperty("position", hoolai::HLPoint(100, 100));
//        entity->setProperty("view", label_1);
//        entity->setUserInteractionEnabled(true);
//        sceneMgr->addEntity(entity);

        
//        hoolai::HLEntity* entity = sceneMgr->getEntityManager()->createEntity("entity", "HLTransform2DComponent", "HLSpriteComponent", NULL);
//        entity->setProperty("size", hoolai::HLSize(100, 125));
//        entity->setProperty("position", hoolai::HLPoint(100, 100));
//        entity->setProperty("texture", hoolai::HLTexture::getTexture("1308451173.jpg"));
//        entity->setProperty("scaleX", 0.5f);
//        entity->setProperty("skewY", 30.f);
//        sceneMgr->addEntity(entity);
        
//        hoolai::HLEntity* entity = sceneMgr->getEntityManager()->createEntity("entity1", "HLTransform2DComponent", NULL);
//        entity->setUserInteractionEnabled(true);
//        entity->setProperty("size", hoolai::HLSize(320, 480));
//        hoolai::JSComponent * comp = new hoolai::JSComponent("test.js");
//        entity->addComponent(comp);
//        sceneMgr->addEntity(entity);
//
//        entity = sceneMgr->getEntityManager()->createEntity("entity2", "HLTransform2DComponent", NULL);
//        comp = new hoolai::JSComponent("test3.js");
//        entity->addComponent(comp);
        
      
        hoolai::gui::HLLabel *label = new hoolai::gui::HLLabel(NULL, hoolai::HLRect(0,0,200,50));
        label->setTextStroke(true);
        label->setTextAlignment(hoolai::gui::kTextHAlignmentCenter | hoolai::gui::kTextVAlignmentBottom);
        label->setFontSize(12);
        label->setText("Test HLLabel in Entity\n水平居中\n垂直靠下");
        hoolai::HLEntity * entity_1 = sceneMgr->getEntityManager()->createEntity("gui", "HLTransform2DComponent", "HLGUIViewComponent", NULL);
        entity_1->setProperty("size", hoolai::HLSize(200, 50));
        entity_1->setProperty("position", hoolai::HLPoint(100, 100));
        entity_1->setProperty("view", label);
        entity_1->setUserInteractionEnabled(true);
        sceneMgr->addEntity(entity_1);
    
	 //testanim  testJsGuiComponent test
//	 hoolai::HLEntity * entity = sceneMgr->getEntityManager()->createEntity("controller", "HLTransform2DComponent", NULL);
//        entity->setProperty("size", hoolai::HLSize(320, 480));
//	 hoolai::JSComponent * comp = new hoolai::JSComponent("testJsGuiComponent.js");
//        comp->setProperty("testproperty", "testJsGuiComponent====");
//        entity->addComponent(comp);
//        
//        sceneMgr->addEntity(entity);
//
//        for (int i = 0; i < 20; i++)
//        {
//		entity = sceneMgr->getEntityManager()->createAnonymousEntity("HLTransform2DComponent", "HLSkeletonComponent", NULL);
//		entity->setProperty("position", hoolai::HLPoint(100+i*10, 100));
//		sceneMgr->addEntity(entity);
//		entity->setProperty("skelfile", std::string("demo.bin"));
//		entity->getComponent<hoolai::HLSkeletonComponent>()->play("stand", false, true, 0, 1);
//        }

//		hoolai::HLSceneManager *sceneMgr1 = new hoolai::HLSceneManager();
//		entity = sceneMgr1->getEntityManager()->createEntity("tttttt", "HLTransform2DComponent", "HLSpriteComponent", NULL);
//		entity->setProperty("size", hoolai::HLSize(100, 100));
//		//        entity->setProperty("anchorPoint", hoolai::HLPoint(1000, 1000));
//		//        entity->setProperty("position", hoolai::HLPoint(-1000, -1000));
//		hoolai::HLTexture* tex = hoolai::HLTexture::getTexture("at01.jpg");
//		entity->setProperty("texture", tex);
//		sceneMgr1->addEntity(entity);
        
        // render to texture
//        hoolai::HLRenderTexture renderTexture(200/director->getWinSizeScale(), 100/director->getWinSizeScale());
//        renderTexture.beginWith2DProjection();
//        // save clear color
//        GLfloat    clearColor[4];
//        glGetFloatv(GL_COLOR_CLEAR_VALUE,clearColor);
//        
//        glClearColor(1, 0, 0, 1);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // restore clear color
//        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
//        sceneMgr1->draw();
//        hoolai::drawColor4F(0, 1, 0, 1);
//        hoolai::drawLine(hoolai::HLPointZero, hoolai::HLPoint(100, 100));
//       renderTexture.renderView(label);
//        
//        renderTexture.end();
//        
//        entity = sceneMgr->getEntityManager()->createEntity("tttttt1", "HLTransform2DComponent", "HLSpriteComponent", "HLBlendFuncComponent", NULL);
//        entity->setProperty("position", hoolai::HLPoint(0, 100));
//        entity->setProperty<GLenum>("sfactor", GL_ONE);
//        entity->setProperty<GLenum>("dfactor", GL_NONE);
//        entity->setProperty("size", hoolai::HLSize(200, 100));
//        entity->setProperty("scaleY", -1.f);
//        renderTexture.getSpriteFrame()->setDisplayFrameForEntity(entity);
//        sceneMgr->addEntity(entity);
        
//        hoolai::HLEntity* clippingEntity = sceneMgr->getEntityManager()->createEntity("testclip", "HLTransform2DComponent", "HLClippingComponent", NULL);
//        clippingEntity->setProperty("position", hoolai::HLPoint(100, 100));
//        hoolai::HLEntity* entity = sceneMgr->getEntityManager()->createEntity("mask", "HLTransform2DComponent", "HLSpriteComponent", "HLShaderComponent", NULL);
//        hoolai::HLProgram* program = hoolai::HLProgramManager::currentManager()->getProgram(hoolai::kShaderPositionTextureColorAlphaTest);
//        entity->setProperty("shaderProgram", program);
//        program->getUniform(hoolai::kUniformAlphaThreshold).setUniform1f(0.05f);
//        entity->setProperty("size", hoolai::HLSize(128, 128));
//        entity->setProperty("texture", hoolai::HLTexture::getTexture("ChatBubbleGreen.png"));
//        clippingEntity->setProperty("stencil", entity);
//        
//        entity = sceneMgr->getEntityManager()->createEntity("tttttt1", "HLTransform2DComponent", "HLSpriteComponent", NULL);
//        entity->setProperty("size", hoolai::HLSize(128, 128));
//        entity->setProperty("texture", hoolai::HLTexture::getTexture("grass.png"));
//        clippingEntity->getComponent<hoolai::HLTransform2DComponent>()->addChild(entity);
//        
//        sceneMgr->addEntity(clippingEntity);
        
        hoolai::gui::HLRootWidget* rootwidget = hoolai::gui::HLGUIManager::getSingleton()->getRootWidget();
        hoolai::gui::HLButton* btn = new hoolai::gui::HLButton(rootwidget, hoolai::HLRect(150, 20, 100, 20));
        btn->setNormalTitle("I am a button");
        btn->setNormalBackgroundImage(hoolai::HLTexture::getTexture("fogmask.png"));
        btn->onButtonClick = hoolai::newDelegate(buttonClicked);
        
        hoolai::gui::HLLabel* dragsource = new hoolai::gui::HLLabel(rootwidget, hoolai::HLRect(0,0,50,50));
        dragsource->setTextStroke(true);
        dragsource->setBackgroundColor(color);
        dragsource->setFontSize(12);
        dragsource->setText("Test Drag Me");
        dragsource->touchHandler = hoolai::newDelegate(handleTouch);
        dragsource->setUserInteractionEnabled(true);
//        
        hoolai::gui::HLRichLabel* testlabel = new hoolai::gui::HLRichLabel(rootwidget, hoolai::HLRect(0,100,140,200));
        testlabel->setTextStroke(true);
        hoolai::color4B color_1 = {0, 0, 0, 150};
        testlabel->setTextStrokeColor(color_1);
        testlabel->setLinkTextColor(hoolai::HLBlueColor4B);
        testlabel->setBackgroundColor(hoolai::HLRedColor4B);
        testlabel->setTextAlignment(hoolai::gui::kTextHAlignmentLeft|hoolai::gui::kTextVAlignmentTop);
        testlabel->setFontSize(11);
        testlabel->setText("<font color='#0254fb' size='10'>恢复</font>  <font size='9'>2级</font><br/><font size='2'> </font><br/>冷却时间：15秒<br/><font size='2'> </font><br/>怒气消耗：35<br/><font size='2'> </font><br/><font color='#eaff00'>己单体<img src='h0.png,h1.png,h2.png'/>血量<img src='faces.json:00/1.png,01/1.png,02/1.png'/>百分比最低的对象恢复魔法伤害65%+200血量     <br/>QTE：为目标附加减伤效果<img src='faces.json:03/1.png'/>，承受伤害量降低10%，持续2回合</font>");
        testlabel->setUserInteractionEnabled(true);
        testlabel->delegate = hoolai::newDelegate(richlabelDelegate);
        //testlabel->startShowAnimation(8);
        
//        hoolai::gui::HLLabel* droptarget = new hoolai::gui::HLLabel(rootwidget, hoolai::HLRect(100,100,50,50));
//        droptarget->setTextStroke(true);
//        droptarget->setBackgroundColor(color);
//        droptarget->setFontSize(12);
//        droptarget->setText("Drop Here");
//        droptarget->draggingEventHandler = hoolai::newDelegate(handleDragEvent);
//        droptarget->setUserInteractionEnabled(true);
//        
//        hoolai::gui::HLEditBox* editBox = new hoolai::gui::HLEditBox(rootwidget, hoolai::HLRect(10, 300, 100, 20));
//        //        editBox->setSecureInput(true);
//        color.r = color.g = color.b = color.a = 255;
//        editBox->setBackgroundColor(color);
//        
//        progressview = new hoolai::gui::HLProgressRadialView(rootwidget, hoolai::HLRect(0,0,50,50));
//        
//        hoolai::HLTimer *timer = new hoolai::HLTimer(0.2, true);
//        timer->delegate = hoolai::newDelegate(setProgress);
        
//        hoolai::gui::HLImageView* imgview = new hoolai::gui::HLImageView(rootwidget, hoolai::HLRect(0, 100, 50, 50));
//        imgview->setImage(hoolai::HLTexture::getTexture("fogmask.png"));
//        
//#if defined(TARGET_ANDROID)
//        imgview = new hoolai::gui::HLImageView(rootwidget, hoolai::HLRect(0, 100, 256, 256));
//        imgview->setImage(hoolai::HLTexture::getTexture("back_image.pkm"));
//#endif
        
//        entity = sceneMgr->getEntityManager()->createEntity("winanim", "HLTransform2DComponent", NULL);
//        entity->setProperty("position", hoolai::HLPoint(160, 480));
//        entity->addComponent(hoolai::HLParticleExampleComponent::getSnowParticleComponent());
//        entity->setProperty("texture", hoolai::HLTexture::getTexture("h0.png"));
//        sceneMgr->addEntity(entity);
        
        
        
//        imgview->setRotation(45);
        
        

//        sceneMgr->addEntity(entity);
        
//        entity = sceneMgr->getEntityManager()->createEntity("camera", "HLTransform2DComponent", "HLCamera2DComponent", NULL);
//        entity->setProperty("position", hoolai::HLPoint(0, 0));
//        entity->setProperty("active", true);
//        
//        sceneMgr->addEntity(entity);
//        comp->release();
        
        
        //        hoolai::HLEntity * entity = hoolai::HLEntityManager::sharedEntityManager()->createEntity("test", "HLTransform3DComponent", NULL);
        //        entity->setProperty("position", hoolai::HLPoint3D(0, 0, 0));
        //        entity->setProperty("scale", 0.1f);
        ////        entity->setProperty("rotationX", 90.0f);
        //        TestComponent *comp = new TestComponent();
        //        entity->addComponent(comp);
        //        comp->autorelease();
        //        scene->addEntity(entity);
        //
        //        entity = hoolai::HLEntityManager::sharedEntityManager()->createEntity("main_camera", "HLTransform3DComponent", "HLCameraComponent");
        //        entity->setProperty("position", hoolai::HLPoint3D(0, -6, 1.35f));
        //        entity->setProperty("center", hoolai::HLPoint3D(0, -5, 1.35f));
        //        entity->setProperty("up", hoolai::HLPoint3D(0, 0, 1));
        //        scene->addEntity(entity);
        //        scene->setActiveCamera(entity);
        
//        hoolai::HLEntity * entity = sceneMgr->getEntityManager()->createEntity("winanim", "HLTransform2DComponent", "HLSpriteComponent", NULL);
//        entity->setProperty("position", hoolai::HLPoint(160, 100));
//        entity->setProperty("size", hoolai::HLSize(194, 212));
//        entity->setProperty("anchorPoint", hoolai::HLSize(97, 106));
//        hoolai::HLArray * arr = hoolai::HLSpriteFrame::spriteFrameArrayFromFile("shengli.json");
//        hoolai::HLSpriteFrameAnimation *animation = new hoolai::HLSpriteFrameAnimation(1, arr);
//        animation->autorelease();
//        hoolai::HLRepeateAnimation *anim1 = new hoolai::HLRepeateAnimation(animation, 0);
//        anim1->run(entity);
//        anim1->release();
//        sceneMgr->addEntity(entity);
//        
//        hoolai::HLTweenSystem::startAnimations("move&rot_demo", 2, 0);
//        hoolai::HLTweenSystem::setAnimationCurve(hoolai::HLAnimationCurveEaseInOut);
//        hoolai::HLTweenSystem::setAutoReverse(true);
//        entity->setProperty("position", hoolai::HLPoint(160, 200));
//        entity->setProperty("rotation", 180.f);
//        hoolai::HLTweenSystem::commitAnimations();
        
         director->runWithSceneManager(sceneMgr);
        
//        hoolai::JSScriptingCore::getSingleton()->runScript("main.js");
        
//        HttpTest::test();
    }
    
    virtual void applicationDidEnterBackground()
    {
        
    }
    
    virtual void applicationWillEnterForeground()
    {
        
    }
    
    virtual void applicationWillTerminate()
    {
        
    }
};

#endif

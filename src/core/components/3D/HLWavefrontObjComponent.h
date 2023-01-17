//
//  HLWavefrontObjComponent.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __HLWAVEFRONTOBJCOMPONENT_H__
#define __HLWAVEFRONTOBJCOMPONENT_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "HLEntitySystem.h"
#include "kazmath/mat4.h"
#include "kazmath/vec4.h"
#include "kazmath/vec2.h"
#include "HLProgram.h"
#include "HLTexture.h"
#include "HLFileUtil.h"

NS_HL_BEGIN

//! OBJMATERIAL draw callback function prototype.
typedef void( MATERIALDRAWCALLBACK( void * ) );

//! Wavefront OBJMATERIAL structure definition.
typedef struct
{
	//! The material name.
	char					name[ MAX_CHAR ];				// newmtl
	
	//! The ambient color of the material.
	kmVec4					ambient;						// Ka
	
	//! The diffuse color of the material.
	kmVec4					diffuse;						// Kd
    
	//! The specular color of the material.
	kmVec4					specular;						// Ks
	
	//! The transmission filter of the material.
	kmVec3					transmission_filter;			// Tf
    
	//! The illumination model of the material.
	int						illumination_model;				// illum
	
	//! The material dissolve factor (aka Alpha).
	float					dissolve;						// d
	
	//! The specular exponent of the material (aka Hardness or Shiness).
	float					specular_exponent;				// Ns
    
	//! The optical density of the material.
	float					optical_density;				// Ni
	
	//! The ambient texture channel filename.
	char					map_ambient[ MAX_CHAR ];		// map_Ka
	
	//! The diffuse texture channel filename.
	char					map_diffuse[ MAX_CHAR ];		// map_Kd
	
	//! The specular texture channel filename.
	char					map_specular[ MAX_CHAR ];		// map_Ks
    
	//! The translucency texture channel filename.
	char					map_translucency[ MAX_CHAR ];	// map_Tr
    
	//! The displacement map texture channel filename.
	char					map_disp[ MAX_CHAR ];			// disp or map_disp
    
	//! The bump map (aka Normal Map) texture channel filename.
	char					map_bump[ MAX_CHAR ];			// bump or map_bump	
	
	//! The ambient TEXTURE pointer. 
	HLTexture					*texture_ambient;
    
	//! The diffuse TEXTURE pointer. 
	HLTexture					*texture_diffuse;
    
	//! The specular TEXTURE pointer. 
	HLTexture					*texture_specular;
    
	//! The translucency TEXTURE pointer. 
	HLTexture					*texture_translucency;
    
	//! The displacement TEXTURE pointer. 
	HLTexture					*texture_disp;
    
	//! The bumpmap TEXTURE pointer. 
	HLTexture					*texture_bump;
    
	//! The shader PROGRAM to use when drawing the material.
	HLProgram					*program;
	
	//! The material draw callback function pointer to use.
	MATERIALDRAWCALLBACK	*materialdrawcallback;
	
} OBJMATERIAL;

//! Structure definition to hold all the vertex and UV index for a specific triangle.
typedef struct
{
	int vertex_index[ 3 ];
	
	int uv_index[ 3 ];
    
} OBJTRIANGLEINDEX;


//! Structure definition that represent one triangle list contained inside an OBJMESH.
typedef struct
{
	//! The number of triangle index.
	unsigned int	 n_objtriangleindex;
	
	//! Triangle arrady that contain each triangle index data.
	OBJTRIANGLEINDEX *objtriangleindex;
    
	//! Flag to determine if the OBJTRIANGLELIST is using UVs.
	unsigned char	 useuvs;
	
	//! The number of indice required to draw the triangle list.
	unsigned short	 n_indice_array;
	
	//! Array of indices.
	unsigned short   *indice_array;
	
	//! Pointer to an OBJMATERIAL to use when drawing the OBJTRIANGLELIST. 
	OBJMATERIAL		 *objmaterial;
	
	//! The drawing mode to use. (Default: GL_TRIANGLES)
	int				 mode;
	
	//! The internal VBO id maintained by GLES to draw the OBJTRIANGLELIST.
	unsigned int	 vbo;
    
} OBJTRIANGLELIST;

//! Structure definition for a unique vertex data indices.
typedef struct
{
	//! The vertex index of the OBJVERTEXDATA.
	int vertex_index;
	
	//! The uv index of the OBJVERTEXDATA.
	int uv_index;
    
} OBJVERTEXDATA;

//! Structure definition to represent a single object entry contained in a Wavefront OBJ file.
typedef struct
{
	//! The name of the object.
	char			name[ MAX_CHAR ];  // o
	
	//! Flag to determine if the object is visible for drawing.
	unsigned char	visible;
    
	//! The object group (if any).
	char			group[ MAX_CHAR ]; // g
    
	//! The number of OBJVERTEXDATA for this mesh.
	unsigned short	n_objvertexdata;
	
	//! Array of OBJVERTEXDATA to be able to construct the mesh.
	OBJVERTEXDATA	*objvertexdata;
	
	//! The number of triangle list for this OBJMESH.
	unsigned char	n_objtrianglelist;
	
	//! Array of triangle list, basically one list for each OBJMATERIAL required to draw the OBJMESH.
	OBJTRIANGLELIST	*objtrianglelist;
	
	//! The current OBJMATERIAL pointer that the OBJ_draw_mesh function is handling (ideal for external callbacks).
	OBJMATERIAL		*current_material;
	
	//! The XYZ location in worldspace of the OBJMESH.
	kmVec3			location;
    
	//! The XYZ euler angle rotation of the OBJMESH.
	kmVec3			rotation;
	
	//! The XYZ scale vector for the current OBJMESH.
	kmVec3			scale;
	
	//! The bottom left XYZ coordinate in worldspace of the OBJMESH bounding box.
	kmVec3			min;
	
	//! The upper right XYZ coordinate in worldspace of the OBJMESH bounding box.
	kmVec3			max;
	
	//! The XYZ dimension of the bounding box.
	kmVec3			dimension;
	
	//! The bounding sphere radius.
	float			radius;	
    
	//! The distance of the OBJMESH with the viewer.
	float			distance;
	
	//! The vertex buffer VBO id maintained by GLES.
	unsigned int	vbo;
	
	//! The stride size in bytes to determine where the next chunk of data in the vertex buffer is located.
	unsigned int	stride;
	
	//! The total size of the vertex data array.
	unsigned int	size;
	
	//! The VBO offsets.
	unsigned int	offset[ 5 ];
	
	//! The VAO id maintained by GLES.
	unsigned int	vao;	
	
	//! Determine if the OBJMESH is using vertex or face normals.
	unsigned char	use_smooth_normals;
    
} OBJMESH;

typedef struct
{
	//! The texture path (By default relative to the location of the .mtl file).
	char			texture_path[ MAX_PATH ];
    
	//! The shader program path (By default relative to the location of the .mtl file).
	char			program_path[ MAX_PATH ];
	
	//! The number of OBJMESH.
	unsigned int	n_objmesh;
	
	//! Array of OBJMESH for each object entry of the .obj file.
	OBJMESH			*objmesh;
	
	//! The number of OBJMATERIAL.
	unsigned int	n_objmaterial;
	
	//! Array of OBJMATERIAL for each material entry inside a .mtl file.
	OBJMATERIAL		*objmaterial;
	
	//! The number of texture that the current OBJ is using.
	unsigned int	n_texture;
	
	//! Array of unique TEXTURE entry used by the OBJ.
	HLTexture			**texture;
    
	//! The number of shader PROGRAM the OBJ is using.
	unsigned int	n_program;
    
	//! Array of unique PROGRAM entry used by the OBJ.
	HLProgram			**program;
	
	//! The number of indexed vertex (one unique entry for each vertex/normal/fnormal/tangent)
	unsigned int	n_indexed_vertex;
	
	//! Array of indexed vertices.
	kmVec3			*indexed_vertex;	// v
    
	//! Array of indexed normal.
	kmVec3			*indexed_normal;
    
	//! Array of indexed face normals.
	kmVec3			*indexed_fnormal;
    
	//! Array of indexed tangents.
	kmVec3			*indexed_tangent;
    
	//! Number of indexed UVs.
	unsigned int	n_indexed_uv;
	
	//! Array of indexed UVs.
	kmVec2			*indexed_uv;		// vt
    
} OBJ;

class HLWavefrontObjComponent : public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLWavefrontObjComponent, std::string, objfile)
    
public:
    HLWavefrontObjComponent():obj(NULL)
    {
//        program = HLProgramManager::sharedProgramManager()->getProgram("wavefrontprograme", HLString::stringWithContentsOfFile("vertex.glsl")->_value.c_str(), HLString::stringWithContentsOfFile("fragment.glsl")->_value.c_str());
//        attr_position = glGetAttribLocation(program->getProgram(), kAttributeNamePosition);
//        attr_normal = glGetAttribLocation(program->getProgram(), "a_normal");
//        attr_texcoord = glGetAttribLocation(program->getProgram(), kAttributeNameTexCoord);
    }
    ~HLWavefrontObjComponent()
    {
        free_obj();
    }
private:
    OBJ *obj;
//    HLProgram *program;
    
    unsigned int attr_position;
    unsigned int attr_normal;
    unsigned int attr_texcoord;
    
    OBJMATERIAL * get_material(const char* name, unsigned char exact_name);
    unsigned char load_mtl(const char* file);
    int get_program_index( char *filename );
    void add_program( char *filename );
    void add_texture( char *filename );
    void load_obj();
    void free_obj();
    void build_mesh(unsigned int index);
    void build_material(unsigned int index);
    void set_attributes_mesh(unsigned int mesh_index);
    void drawMesh(int index);
    
    void drawMeshWithMaterial(int i);
    void free_mesh_vertex_data(unsigned int mesh_index)
    {
        unsigned int i = 0;
        
        OBJMESH *objmesh = &obj->objmesh[ mesh_index ];
        
        if( objmesh->objvertexdata )
        {
            free( objmesh->objvertexdata );
            objmesh->objvertexdata = NULL;
        }
        
        objmesh->n_objvertexdata = 0;
        
        while( i != objmesh->n_objtrianglelist )
        {
            free( objmesh->objtrianglelist[ i ].objtriangleindex );
            objmesh->objtrianglelist[ i ].objtriangleindex = NULL;
            
            objmesh->objtrianglelist[ i ].n_objtriangleindex = 0;
            
            free( objmesh->objtrianglelist[ i ].indice_array );
            objmesh->objtrianglelist[ i ].indice_array = NULL;
            
            ++i;
        }
    }
    
public:
    
    virtual bool canHandleEvent(HLComponentEvent event)
    {
        return event==kHLComponentEventDraw;
    }
    
    virtual event_handler_func getEventHandler(HLComponentEvent event)
    {
        event_handler_func func;
        if (event == kHLComponentEventDraw) 
        {
            func.drawfunc = (drawfunc_pointer)&HLWavefrontObjComponent::draw;
        }
        return func;
    }
    
    void draw(HLEntity*);
    
    static const FamilyId familyId;
};

NS_HL_END

#endif

#endif

//
//  HLWavefrontObjComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-8-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLWavefrontObjComponent.h"
#include "HLDirector3D.h"

#if GAME_PROJECTION == PROJECTION_3D

#include "mat3.h"

using namespace std;

NS_HL_BEGIN

INIT_FAMILYID(HLWavefrontObjComponent)

PROPERTIES_INITIAL(HLWavefrontObjComponent)
REGISTER_PROPERTY(HLWavefrontObjComponent, std::string, objfile)

void OBJMESH_add_vertex_data( OBJMESH		  *objmesh,
                             OBJTRIANGLELIST *objtrianglelist,
                             int			  vertex_index,
                             int			  uv_index )
{
	unsigned short index = 0;
	
	while( index != objmesh->n_objvertexdata )
	{
		if( vertex_index == objmesh->objvertexdata[ index ].vertex_index )
		{
			if( uv_index == -1 ) goto add_index_to_triangle_list;
			
			else if( uv_index == objmesh->objvertexdata[ index ].uv_index ) goto add_index_to_triangle_list;
		}
		
		++index;
	}
	
	index = objmesh->n_objvertexdata;
	
	++objmesh->n_objvertexdata;
	
	objmesh->objvertexdata = ( OBJVERTEXDATA * ) realloc( objmesh->objvertexdata, 
                                                         objmesh->n_objvertexdata *
                                                         sizeof( OBJVERTEXDATA ) );
	
	objmesh->objvertexdata[ index ].vertex_index = vertex_index;
	objmesh->objvertexdata[ index ].uv_index	   = uv_index;
    
    
add_index_to_triangle_list:
	
	++objtrianglelist->n_indice_array;
	
	objtrianglelist->indice_array = ( unsigned short * ) realloc( objtrianglelist->indice_array,
                                                                 objtrianglelist->n_indice_array *
                                                                 sizeof( unsigned short ) );
    
	objtrianglelist->indice_array[ objtrianglelist->n_indice_array - 1 ] = index;	
}

void HLWavefrontObjComponent::load_obj()
{
    if (obj) 
    {
        free_obj();
    }
    char path[PATH_MAX];
    get_full_path(m_objfile.c_str(), path);
    HLFileData filedata(path, "r");
    if (filedata.size <= 0) {
        return;
    }
    
    obj = ( OBJ * ) calloc( 1, sizeof( OBJ ) );
    
    char name  [ MAX_CHAR ] = {""},	
    group [ MAX_CHAR ] = {""},
    usemtl[ MAX_CHAR ] = {""},
    str   [ MAX_PATH ] = {""},
    last  = 0,
    *line = strtok( ( char * )filedata.buffer, "\n" );
    
    unsigned char use_smooth_normals;
    
    OBJMESH *objmesh = NULL;
    
    OBJTRIANGLELIST *objtrianglelist = NULL;
    
    kmVec3 v;
    
    while( line )
    {	
        if( !line[ 0 ] || line[ 0 ] == '#' ) goto next_obj_line;
        
        else if( line[ 0 ] == 'f' && line[ 1 ] == ' ' )
        {
            unsigned char useuvs;
            
            int vertex_index[ 3 ] = { 0, 0, 0 },
            normal_index[ 3 ] = { 0, 0, 0 },
            uv_index	[ 3 ] = { 0, 0, 0 },
            triangle_index;
            
            if( sscanf( line, "f %d %d %d", &vertex_index[ 0 ],
                       &vertex_index[ 1 ],
                       &vertex_index[ 2 ] ) == 3 )
            { useuvs = 0; }
            
            else if( sscanf( line, "f %d//%d %d//%d %d//%d", &vertex_index[ 0 ],
                            &normal_index[ 0 ],
                            &vertex_index[ 1 ],
                            &normal_index[ 1 ],
                            &vertex_index[ 2 ],
                            &normal_index[ 2 ] ) == 6 )
            { useuvs = 0; }
            
            else if( sscanf( line, "f %d/%d %d/%d %d/%d", &vertex_index[ 0 ],
                            &uv_index    [ 0 ],
                            &vertex_index[ 1 ],
                            &uv_index    [ 1 ],
                            &vertex_index[ 2 ],
                            &uv_index    [ 2 ] ) == 6 )
            { useuvs = 1; }
            
            else
            {
                sscanf( line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_index[ 0 ],
                       &uv_index    [ 0 ],
                       &normal_index[ 0 ],
                       &vertex_index[ 1 ],
                       &uv_index    [ 1 ],
                       &normal_index[ 1 ],
                       &vertex_index[ 2 ],
                       &uv_index    [ 2 ],
                       &normal_index[ 2 ] );
                useuvs = 1;
            }
            
            
            if( last != 'f' )
            {
                ++obj->n_objmesh;
                
                obj->objmesh = ( OBJMESH * ) realloc( obj->objmesh,
                                                      obj->n_objmesh *
                                                      sizeof( OBJMESH ) );
                
                objmesh = &obj->objmesh[ obj->n_objmesh - 1 ];
                
                memset( objmesh, 0, sizeof( OBJMESH ) );
                
                objmesh->scale.x  =
                objmesh->scale.y  =
                objmesh->scale.z  =
                objmesh->distance = 1.0f;
                objmesh->visible  = 1;
                
                if( name[ 0 ] ) strcpy( objmesh->name, name );
                
                else if( usemtl[ 0 ] ) strcpy( objmesh->name, usemtl );
                
                if( group[ 0 ] ) strcpy( objmesh->group, group );
                
                objmesh->use_smooth_normals = use_smooth_normals;
                
                ++objmesh->n_objtrianglelist;
                
                objmesh->objtrianglelist = ( OBJTRIANGLELIST * ) realloc( objmesh->objtrianglelist, 
                                                                         objmesh->n_objtrianglelist *
                                                                         sizeof( OBJTRIANGLELIST ) );
                
                objtrianglelist = &objmesh->objtrianglelist[ objmesh->n_objtrianglelist - 1 ];
                
                memset( objtrianglelist,
                       0,
                       sizeof( OBJTRIANGLELIST ) );
                
                objtrianglelist->mode = GL_TRIANGLES;
                
                if( useuvs ) objtrianglelist->useuvs = useuvs;
                
                
                if( usemtl[ 0 ] ) objtrianglelist->objmaterial = get_material( usemtl, 1 );
                
                name  [ 0 ] = 0;
                usemtl[ 0 ] = 0;
            }
            
            --vertex_index[ 0 ];
            --vertex_index[ 1 ];
            --vertex_index[ 2 ];
            
            --uv_index[ 0 ];
            --uv_index[ 1 ];
            --uv_index[ 2 ];
            
            
            OBJMESH_add_vertex_data( objmesh,
                                    objtrianglelist,
                                    vertex_index[ 0 ], 
                                    uv_index    [ 0 ] );
            
            OBJMESH_add_vertex_data( objmesh,
                                    objtrianglelist,
                                    vertex_index[ 1 ], 
                                    uv_index    [ 1 ] );
            
            OBJMESH_add_vertex_data( objmesh,
                                    objtrianglelist,
                                    vertex_index[ 2 ], 
                                    uv_index    [ 2 ] );
            
            
            triangle_index = objtrianglelist->n_objtriangleindex;
            
            ++objtrianglelist->n_objtriangleindex;
            
            objtrianglelist->objtriangleindex = ( OBJTRIANGLEINDEX * ) realloc( objtrianglelist->objtriangleindex, 
                                                                               objtrianglelist->n_objtriangleindex * sizeof( OBJTRIANGLEINDEX ) );
            
            objtrianglelist->objtriangleindex[ triangle_index ].vertex_index[ 0 ] = vertex_index[ 0 ];
            objtrianglelist->objtriangleindex[ triangle_index ].vertex_index[ 1 ] = vertex_index[ 1 ];
            objtrianglelist->objtriangleindex[ triangle_index ].vertex_index[ 2 ] = vertex_index[ 2 ];
            
            objtrianglelist->objtriangleindex[ triangle_index ].uv_index[ 0 ] = uv_index[ 0 ];
            objtrianglelist->objtriangleindex[ triangle_index ].uv_index[ 1 ] = uv_index[ 1 ];
            objtrianglelist->objtriangleindex[ triangle_index ].uv_index[ 2 ] = uv_index[ 2 ];
        }			
        
        else if( sscanf( line, "v %f %f %f", &v.x, &v.y, &v.z ) == 3 )
        {
            // Vertex
            ++obj->n_indexed_vertex;
            
            obj->indexed_vertex = ( kmVec3 * ) realloc( obj->indexed_vertex,
                                                        obj->n_indexed_vertex * 
                                                        sizeof( kmVec3 ) );
            
            memcpy( &obj->indexed_vertex[ obj->n_indexed_vertex - 1 ],
                   &v,
                   sizeof( kmVec3 ) );
            
            
            // Normal
            obj->indexed_normal = ( kmVec3 * ) realloc( obj->indexed_normal,
                                                        obj->n_indexed_vertex * 
                                                        sizeof( kmVec3 ) );
            
            obj->indexed_fnormal = ( kmVec3 * ) realloc( obj->indexed_fnormal,
                                                         obj->n_indexed_vertex * 
                                                         sizeof( kmVec3 ) );
            
            memset( &obj->indexed_normal[ obj->n_indexed_vertex - 1 ],
                   0,
                   sizeof( kmVec3 ) );
			
			
            memset( &obj->indexed_fnormal[ obj->n_indexed_vertex - 1 ],
                   0,
                   sizeof( kmVec3 ) );			
			
			
            // Tangent
            obj->indexed_tangent = ( kmVec3 * ) realloc( obj->indexed_tangent,
                                                         obj->n_indexed_vertex * 
                                                         sizeof( kmVec3 ) );
            
            memset( &obj->indexed_tangent[ obj->n_indexed_vertex - 1 ],
                   0,
                   sizeof( kmVec3 ) );
        }
        
        // Drop the normals.
        else if( sscanf( line, "vn %f %f %f", &v.x, &v.y, &v.z ) == 3 ) goto next_obj_line;
        
        else if( sscanf( line, "vt %f %f", &v.x, &v.y ) == 2 )
        {
            ++obj->n_indexed_uv;
            
            obj->indexed_uv = ( kmVec2 * ) realloc( obj->indexed_uv,
                                                    obj->n_indexed_uv * 
                                                    sizeof( kmVec2 ) );
            v.y = 1.0f - v.y;
            
            memcpy( &obj->indexed_uv[ obj->n_indexed_uv - 1 ],
                   &v,
                   sizeof( kmVec2 ) );
        }			
        
        else if( line[ 0 ] == 'v' && line[ 1 ] == 'n' ) goto next_obj_line;
        
        else if( sscanf( line, "usemtl %s", str ) == 1 ) strcpy( usemtl, str );
        
        else if( sscanf( line, "o %s", str ) == 1 ) strcpy( name, str );
        
        else if( sscanf( line, "g %s", str ) == 1 ) strcpy( group, str );
        
        else if( sscanf( line, "s %s", str ) == 1 )
        {
            use_smooth_normals = 1;
            
            if( strstr( str, "off" ) || strstr( str, "0" ) )
            { use_smooth_normals = 0; }
        }
        
        else if( sscanf( line, "mtllib %s", str ) == 1 )
        {
            int position = ( unsigned char * )line - filedata.buffer + strlen( line ) + 1;
            
            load_mtl( str );
            
            line = strtok( ( char * )&filedata.buffer[ position ], "\n" );
            continue;
        }
        
    next_obj_line:
        
        last = line[ 0 ];
        line = strtok( NULL, "\n" );
    }
    
    // Build Normals and Tangent
	{
		unsigned int i, j, k,index;
		
		i = 0;
		while( i != obj->n_objmesh )
		{
			OBJMESH *objmesh = &obj->objmesh[ i ];
            
			// Accumulate Normals and Tangent
			j = 0;
			while( j != objmesh->n_objtrianglelist )
			{
				OBJTRIANGLELIST *objtrianglelist = &objmesh->objtrianglelist[ j ];
                
				k = 0;
				while( k != objtrianglelist->n_objtriangleindex )
				{
					kmVec3 v1,
                    v2,
                    normal;
                    
                    kmVec3Subtract(&v1, 
                                   &obj->indexed_vertex[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ], 
                                   &obj->indexed_vertex[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ]);
                    
					kmVec3Subtract( &v2,
                              &obj->indexed_vertex[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ],
                              &obj->indexed_vertex[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ] );
                    
                    
					kmVec3Cross( &normal, &v1, &v2 );
                    
					kmVec3Normalize( &normal, &normal );
					
					
					// Face normals
					memcpy( &obj->indexed_fnormal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ],
                           &normal,
                           sizeof( kmVec3 ) );
                    
					memcpy( &obj->indexed_fnormal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ],
                           &normal,
                           sizeof( kmVec3 ) );
                    
					memcpy( &obj->indexed_fnormal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ],
                           &normal,
                           sizeof( kmVec3 ) );
					
					
					// Smooth normals
					kmVec3Add( &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ], 
                             &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ],
                             &normal );
                    
					kmVec3Add( &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ], 
                             &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ],
                             &normal );
                    
					kmVec3Add( &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ], 
                             &obj->indexed_normal[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ],
                             &normal );
                    
                    
					if( objtrianglelist->useuvs )
					{
						kmVec3 tangent;
						
						kmVec2 uv1, uv2;
                        
						float c;
						
						kmVec2Subtract( &uv1,
                                  &obj->indexed_uv[ objtrianglelist->objtriangleindex[ k ].uv_index[ 2 ] ],
                                  &obj->indexed_uv[ objtrianglelist->objtriangleindex[ k ].uv_index[ 0 ] ] );
                        
						kmVec2Subtract( &uv2,
                                  &obj->indexed_uv[ objtrianglelist->objtriangleindex[ k ].uv_index[ 1 ] ],
                                  &obj->indexed_uv[ objtrianglelist->objtriangleindex[ k ].uv_index[ 0 ] ] );
						
                        
						c = 1.0f / ( uv1.x * uv2.y - uv2.x * uv1.y );
						
						tangent.x = ( v1.x * uv2.y + v2.x * uv1.y ) * c;
						tangent.y = ( v1.y * uv2.y + v2.y * uv1.y ) * c;
						tangent.z = ( v1.z * uv2.y + v2.z * uv1.y ) * c;
                        
                        
						kmVec3Add( &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ], 
                                 &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 0 ] ],
                                 &tangent );
                        
						kmVec3Add( &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ], 
                                 &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 1 ] ],
                                 &tangent );
                        
						kmVec3Add( &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ], 
                                 &obj->indexed_tangent[ objtrianglelist->objtriangleindex[ k ].vertex_index[ 2 ] ],
                                 &tangent );			
					}
                    
					++k;
				}
                
				++j;
			}
            
			++i;
		}
		
        
		// Normalize Normals & Tangent
		i = 0;
		while( i != obj->n_objmesh )
		{
			j = 0;
			while( j != obj->objmesh[ i ].n_objvertexdata )
			{
				index = obj->objmesh[ i ].objvertexdata[ j ].vertex_index;
				
				// Average smooth normals.
				kmVec3Normalize( &obj->indexed_normal[ index ],
                               &obj->indexed_normal[ index ] );
				
				if( obj->objmesh[ i ].objvertexdata[ j ].uv_index != -1 )
				{
					kmVec3Normalize( &obj->indexed_tangent[ index ],
                                   &obj->indexed_tangent[ index ] );
				}
                
				++j;
			}
			
			++i;
		}	
	}
}

void HLWavefrontObjComponent::free_obj()
{
    if (obj) 
    {
        unsigned int i,
        j;
        
        if( obj->indexed_vertex )
        {
            free( obj->indexed_vertex );
            obj->indexed_vertex = NULL;
        }
        
        if( obj->indexed_normal )
        {
            free( obj->indexed_normal );
            obj->indexed_normal = NULL;
        }
        
        if( obj->indexed_fnormal )
        {
            free( obj->indexed_fnormal );
            obj->indexed_fnormal = NULL;
        }
        
        if( obj->indexed_tangent )
        {
            free( obj->indexed_tangent );
            obj->indexed_tangent = NULL;
        }
        
        if( obj->indexed_uv )
        {
            free( obj->indexed_uv );
            obj->indexed_uv = NULL; 
        }
        
        obj->n_indexed_vertex = 
        obj->n_indexed_uv	  = 0;
        
        i = 0;
        while( i != obj->n_objmesh )
        {
            if( obj->objmesh[ i ].vao ) glDeleteVertexArrays( 1, &obj->objmesh[ i ].vao );
            
            if( obj->objmesh[ i ].vbo ) glDeleteBuffers( 1, &obj->objmesh[ i ].vbo );
            
            free_mesh_vertex_data( i );
            
            if( obj->objmesh[ i ].objtrianglelist )
            {
                j = 0;
                while( j != obj->objmesh[ i ].n_objtrianglelist )
                {
                    glDeleteBuffers( 1, &obj->objmesh[ i ].objtrianglelist[ j ].vbo );
                    ++j;
                }
                
                free( obj->objmesh[ i ].objtrianglelist );
                obj->objmesh[ i ].objtrianglelist = NULL;
            }
            
            ++i;
        }
        
        free( obj->objmesh );
        obj->objmesh = NULL;
        
        
        free( obj->objmaterial );
        obj->objmaterial = NULL;
        
        
        i = 0;
        //            while( i != obj->n_program )
        //            {
        //                if( obj->program[ i ]->vertex_shader   ) obj->program[ i ]->vertex_shader   = SHADER_free( obj->program[ i ]->vertex_shader   );
        //                if( obj->program[ i ]->fragment_shader ) obj->program[ i ]->fragment_shader = SHADER_free( obj->program[ i ]->fragment_shader );
        //                
        //                obj->program[ i ] = PROGRAM_free( obj->program[ i ] );
        //                ++i;
        //            }
        
        //            if( obj->program )
        //            {
        //                free( obj->program );
        //                obj->program = NULL;
        //            }
        
        
        i = 0;
        while( i != obj->n_texture )
        {
            obj->texture[i]->release();
            ++i;
        }
        
        if( obj->texture )
        {
            free( obj->texture );
            obj->texture = NULL;
        }
        
        free(obj);
        obj = NULL;
    }
}

/*!
 Calculate the bound of a mesh. This function will update the min and max bounding box value, update the
 mesh dimension as well as the OBJMESH sphere radius.
 
 \param[in] obj A valid OBJ structure pointer.
 \param[in] mesh_index The mesh index in the OBJ OBJMESH database.
 */
void OBJ_update_bound_mesh( OBJ *obj, unsigned int mesh_index )
{
	unsigned int i, index;
	
	OBJMESH *objmesh = &obj->objmesh[ mesh_index ];
    
	// Get the mesh min and max.
	objmesh->min.x =
	objmesh->min.y =
	objmesh->min.z = 99999.999f;
    
	objmesh->max.x =
	objmesh->max.y =
	objmesh->max.z = -99999.999f;
	
	
	i = 0;
	while( i != objmesh->n_objvertexdata )
	{
		index = objmesh->objvertexdata[ i ].vertex_index;
		
		if( obj->indexed_vertex[ index ].x < objmesh->min.x ) objmesh->min.x = obj->indexed_vertex[ index ].x;
		if( obj->indexed_vertex[ index ].y < objmesh->min.y ) objmesh->min.y = obj->indexed_vertex[ index ].y;
		if( obj->indexed_vertex[ index ].z < objmesh->min.z ) objmesh->min.z = obj->indexed_vertex[ index ].z;
        
		if( obj->indexed_vertex[ index ].x > objmesh->max.x ) objmesh->max.x = obj->indexed_vertex[ index ].x;
		if( obj->indexed_vertex[ index ].y > objmesh->max.y ) objmesh->max.y = obj->indexed_vertex[ index ].y;
		if( obj->indexed_vertex[ index ].z > objmesh->max.z ) objmesh->max.z = obj->indexed_vertex[ index ].z;
        
		++i;
	}
    
    
	// Mesh location
	objmesh->location.x = (objmesh->min.x + objmesh->max.x) * 0.5f;
    objmesh->location.y = (objmesh->min.y + objmesh->max.y) * 0.5f;
    objmesh->location.z = (objmesh->min.z + objmesh->max.z) * 0.5f;
    
    
	// Mesh dimension
    kmVec3Subtract(&objmesh->dimension, &objmesh->max,  &objmesh->min);
    
    
	// Bounding sphere radius
	objmesh->radius = objmesh->dimension.x >= objmesh->dimension.y ?
    objmesh->dimension.x:
    objmesh->dimension.y;
	
	objmesh->radius = objmesh->radius >= objmesh->dimension.z ?
    objmesh->radius * 0.5f:
    objmesh->dimension.z * 0.5f;
	
	/*
     objmesh->radius = vec3_dist( &objmesh->min,
     &objmesh->max ) * 0.5f;
     
     */
}

/*!
 Build the vertex data array buffer VBO for a specific OBJMESH index.
 
 \param[in] obj A valid OBJ structure pointer.
 \param[in] mesh_index The mesh index in the OBJ OBJMESH database.
 */
void OBJ_build_vbo_mesh( OBJ *obj, unsigned int mesh_index )
{
	// Build the VBO for the vertex data
	unsigned int i,
    index,
    offset;
	
	OBJMESH *objmesh = &obj->objmesh[ mesh_index ];
	
	objmesh->stride  = sizeof( kmVec3 ); // Vertex
	objmesh->stride += sizeof( kmVec3 ); // Normals
	objmesh->stride += sizeof( kmVec3 ); // Face Normals
    
	if( objmesh->objvertexdata[ 0 ].uv_index != -1 )
	{
		objmesh->stride += sizeof( kmVec3 ); // Tangent
		objmesh->stride += sizeof( kmVec2 ); // Uv
	}
	
	objmesh->size = objmesh->n_objvertexdata * objmesh->stride;
	
	unsigned char *vertex_array = ( unsigned char * ) malloc( objmesh->size ),
    *vertex_start = vertex_array;
    
	i = 0;
	while( i != objmesh->n_objvertexdata )
	{ 
		index = objmesh->objvertexdata[ i ].vertex_index;
		
		memcpy( vertex_array,
               &obj->indexed_vertex[ index ],
               sizeof( kmVec3 ) );
        
		// Center the pivot
		kmVec3Subtract( ( kmVec3 * )vertex_array,
                  ( kmVec3 * )vertex_array,
                  &objmesh->location );
        
		vertex_array += sizeof( kmVec3 );
        
        
		memcpy( vertex_array,
               &obj->indexed_normal[ index ],
               sizeof( kmVec3 ) );
		
		vertex_array += sizeof( kmVec3 );
        
        
		memcpy( vertex_array,
               &obj->indexed_fnormal[ index ],
               sizeof( kmVec3 ) );
		
		vertex_array += sizeof( kmVec3 );
        
        
		if( objmesh->objvertexdata[ 0 ].uv_index != -1 )
		{
			memcpy( vertex_array,
                   &obj->indexed_uv[ objmesh->objvertexdata[ i ].uv_index ],
                   sizeof( kmVec2 ) );
            
			vertex_array += sizeof( kmVec2 );
			
			memcpy( vertex_array,
                   &obj->indexed_tangent[ index ],
                   sizeof( kmVec3 ) );
            
			vertex_array += sizeof( kmVec3 );				
		}
        
		++i;
	}
	
	
	glGenBuffers( 1, &objmesh->vbo );
	
	glBindBuffer( GL_ARRAY_BUFFER, objmesh->vbo );
	
	glBufferData( GL_ARRAY_BUFFER,
                 objmesh->size,
                 vertex_start,
                 GL_STATIC_DRAW );	
	
	free( vertex_start );
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
	objmesh->offset[ 0 ] = 0;
    
	offset = sizeof( kmVec3 );
    
	objmesh->offset[ 1 ] = offset;
    
	offset += sizeof( kmVec3 );
    
	objmesh->offset[ 2 ] = offset;
    
	offset += sizeof( kmVec3 );
    
	
	if( objmesh->objvertexdata[ 0 ].uv_index != -1 )
	{
		objmesh->offset[ 3 ] = offset;
		
		offset += sizeof( kmVec2 );
        
		objmesh->offset[ 4 ] = offset;
	}
    
	
	i = 0;
	while( i != objmesh->n_objtrianglelist )
	{
		glGenBuffers( 1, &objmesh->objtrianglelist[ i ].vbo );
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[ i ].vbo );
		
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                     objmesh->objtrianglelist[ i ].n_indice_array * sizeof( unsigned short ),
                     objmesh->objtrianglelist[ i ].indice_array,
                     GL_STATIC_DRAW );
		++i;
	}
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

/*!
 Set the vertex attributes for a specific OBJMESH index.
 
 \param[in] obj A valid OBJ structure pointer.
 \param[in] mesh_index The mesh index in the OBJ OBJMESH database.
 */
void HLWavefrontObjComponent::set_attributes_mesh(unsigned int mesh_index)
{
	OBJMESH *objmesh = &obj->objmesh[ mesh_index ];
    
	glBindBuffer( GL_ARRAY_BUFFER, objmesh->vbo );
    
	glEnableVertexAttribArray( 0 );
	
	glVertexAttribPointer( 0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          objmesh->stride,
                          (void *)NULL );
    
	glEnableVertexAttribArray( 1 );
    
	glVertexAttribPointer( 1,
                          3,
                          GL_FLOAT,
                          GL_FALSE, 
                          objmesh->stride,
                          ((char *)NULL + objmesh->offset[ 1 ]));
    
//	glEnableVertexAttribArray( 4 );
//    
//	glVertexAttribPointer( 4,
//                          3,
//                          GL_FLOAT,
//                          GL_FALSE, 
//                          objmesh->stride,
//                          ((char *)NULL + objmesh->offset[ 2 ]));
    
    
	if( objmesh->offset[ 3 ] != -1 )
	{
		glEnableVertexAttribArray( 2 );
        
		glVertexAttribPointer( 2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              objmesh->stride,
                              ((char *)NULL + objmesh->offset[ 3 ]));
//        
//		glEnableVertexAttribArray( 3 );
//        
//		glVertexAttribPointer( 3,
//                              3,
//                              GL_FLOAT,
//                              GL_FALSE, 
//                              objmesh->stride,
//                              ((char *)NULL + objmesh->offset[ 4 ]));
	}
}

void HLWavefrontObjComponent::build_mesh(unsigned int index)
{
    OBJMESH *objmesh = &obj->objmesh[index];
    
    OBJ_update_bound_mesh(obj, index);
    
    OBJ_build_vbo_mesh( obj, index );
	
    
	glGenVertexArrays( 1, &objmesh->vao );
	CHECK_GL_ERROR();
	glBindVertexArray( objmesh->vao );
	CHECK_GL_ERROR();
	
	set_attributes_mesh(index);
	
	if( objmesh->n_objtrianglelist == 1 )
	{ glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[ 0 ].vbo ); }
	
	glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void HLWavefrontObjComponent::build_material(unsigned int material_index)
{
//    int index;
	
	OBJMATERIAL *objmaterial = &obj->objmaterial[material_index];
    
	char ext[ MAX_CHAR ] = {""};
    
    
	if( objmaterial->map_ambient[ 0 ] )
	{
//		get_file_extension( objmaterial->map_ambient, ext, 1 );
//        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_ambient );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_ambient = HLTexture::textureInCache(objmaterial->map_ambient);//OBJ_get_texture_index( obj, objmaterial->map_ambient );
			
//			if( index != -1 ) objmaterial->texture_ambient = obj->texture[ index ];
//		}
	}
    
    
	if( objmaterial->map_diffuse[ 0 ] )
	{
		get_file_extension( objmaterial->map_diffuse, ext, 1 );
        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_diffuse );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_diffuse = HLTexture::textureInCache(objmaterial->map_diffuse);
//			index = OBJ_get_texture_index( obj, objmaterial->map_diffuse );
//			
//			if( index != -1 ) objmaterial->texture_diffuse = obj->texture[ index ];
//		}
	}
    
    
	if( objmaterial->map_specular[ 0 ] )
	{
//		get_file_extension( objmaterial->map_specular, ext, 1 );
//        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_specular );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_specular = HLTexture::textureInCache(objmaterial->map_specular);
//			index = OBJ_get_texture_index( obj, objmaterial->map_specular );
//			
//			if( index != -1 ) objmaterial->texture_specular = obj->texture[ index ];
//		}
	}
    
    
	if( objmaterial->map_translucency[ 0 ] )
	{
//		get_file_extension( objmaterial->map_translucency, ext, 1 );
//        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_translucency );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_translucency = HLTexture::textureInCache(objmaterial->map_translucency);
//			index = OBJ_get_texture_index( obj, objmaterial->map_translucency );
//			
//			if( index != -1 ) objmaterial->texture_translucency = obj->texture[ index ];
//		}
	}
    
    
	if( objmaterial->map_disp[ 0 ] )
	{
//		get_file_extension( objmaterial->map_disp, ext, 1 );
//        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_disp );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_disp = HLTexture::textureInCache(objmaterial->map_disp);
//			index = OBJ_get_texture_index( obj, objmaterial->map_disp );
//			
//			if( index != -1 ) objmaterial->texture_disp = obj->texture[ index ];
//		}
	}
    
    
	if( objmaterial->map_bump[ 0 ] )
	{
//		get_file_extension( objmaterial->map_bump, ext, 1 );
//        
//		if( !strcmp( ext, "GFX" ) )
//		{
//			index = OBJ_get_program_index( obj, objmaterial->map_bump );
//			
//			if( index != -1 ) objmaterial->program = obj->program[ index ];
//		}
//		else
//		{
        objmaterial->texture_bump = HLTexture::textureInCache(objmaterial->map_bump);
//			index = OBJ_get_texture_index( obj, objmaterial->map_bump );
//			
//			if( index != -1 ) objmaterial->texture_bump = obj->texture[ index ];
//		}
	}
}

void HLWavefrontObjComponent::set_objfile(string file)
{
    m_objfile = file;
    
    load_obj();
    
    if (!obj) 
    {
        return;
    }
    
    for (int i = 0; i < obj->n_objmesh; i++) 
    {
        build_mesh(i);
        free_mesh_vertex_data(i);
        CHECK_GL_ERROR();
    }
    
    for (int i = 0; i < obj->n_objmaterial; i++) 
    {
        build_material(i);
        CHECK_GL_ERROR();
        
        HLProgram *pro;
        
         printf("%.2f %d\n", obj->objmaterial[i].dissolve, obj->objmaterial[i].illumination_model);
        
        if (obj->objmaterial[i].dissolve == 1.0f && !obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("solid_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        else if (obj->objmaterial[i].dissolve == 1.0f && obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("light_solid_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        else if (!obj->objmaterial[i].dissolve && !obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("alphatest_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        else if (!obj->objmaterial[i].dissolve && obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("light_alphatest_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        else if ((obj->objmaterial[i].dissolve > 0 && obj->objmaterial[i].dissolve < 1) && !obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("transparent_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        else if ((obj->objmaterial[i].dissolve > 0 && obj->objmaterial[i].dissolve < 1) && obj->objmaterial[i].illumination_model && (pro = HLProgramManager::sharedProgramManager()->getProgram("light_transparent_program")) != NULL)
        {
            obj->objmaterial[i].program = pro;
            continue;
        }
        
        HLString *vert = HLString::stringWithContentsOfFile("vertex.glsl");
        HLString *frag = HLString::stringWithContentsOfFile("fragment.glsl");
        
        if (obj->objmaterial[i].dissolve == 1.0f && !obj->objmaterial[i].illumination_model) 
        {
            frag->_value = "#define SOLID_OBJECT\n"+frag->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("solid_program", vert->_value.c_str(), frag->_value.c_str());

        }
        else if (obj->objmaterial[i].dissolve == 1.0f && obj->objmaterial[i].illumination_model) 
        {
            frag->_value = "#define SOLID_OBJECT\n"+frag->_value;
            frag->_value = "#define LIGHTING_SHADER\n"+frag->_value;
            vert->_value = "#define LIGHTING_SHADER\n"+vert->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("light_solid_program", vert->_value.c_str(), frag->_value.c_str());
            
        }
        else if (!obj->objmaterial[i].dissolve && !obj->objmaterial[i].illumination_model)
        {
            frag->_value = "#define ALPHA_TESTED_OBJECT\n"+frag->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("alphatest_program", vert->_value.c_str(), frag->_value.c_str());
        }
        else if (!obj->objmaterial[i].dissolve && obj->objmaterial[i].illumination_model)
        {
            frag->_value = "#define ALPHA_TESTED_OBJECT\n"+frag->_value;
            frag->_value = "#define LIGHTING_SHADER\n"+frag->_value;
            vert->_value = "#define LIGHTING_SHADER\n"+vert->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("light_alphatest_program", vert->_value.c_str(), frag->_value.c_str());
        }
        else if (!obj->objmaterial[i].illumination_model)
        {
            frag->_value = "#define TRANSPARENT_OBJECT\n"+frag->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("transparent_program", vert->_value.c_str(), frag->_value.c_str());
        }
        else 
        {
            frag->_value = "#define TRANSPARENT_OBJECT\n"+frag->_value;
            frag->_value = "#define LIGHTING_SHADER\n"+frag->_value;
            vert->_value = "#define LIGHTING_SHADER\n"+vert->_value;
            pro = HLProgramManager::sharedProgramManager()->getProgram("light_transparent_program", vert->_value.c_str(), frag->_value.c_str());
        } 
        
        pro->addAttribute("a_position", 0);
        pro->addAttribute("a_normal", 1);
        pro->addAttribute("a_texCoord", 2);
        pro->link();
        
        CHECK_GL_ERROR();
        obj->objmaterial[i].program = pro;
    }
}

OBJMATERIAL * HLWavefrontObjComponent::get_material(const char* name, unsigned char exact_name)
{
    unsigned int i = 0;
	
	while( i != obj->n_objmaterial )
	{
		if( exact_name )
		{ if( !strcmp( obj->objmaterial[ i ].name, name ) ) return &obj->objmaterial[ i ]; }
		
		else
		{ if( strstr( obj->objmaterial[ i ].name, name ) ) return &obj->objmaterial[ i ]; }
        
		++i;
	}
	
	return NULL;
}

unsigned char HLWavefrontObjComponent::load_mtl(const char* file)
{
    char path[PATH_MAX];
    get_full_path_from_relative_file(file, m_objfile.c_str(), path);
    HLFileData filedata(path, "r");
    
    OBJMATERIAL *objmaterial = NULL;
    
	if( filedata.size <= 0 ) return 0;
    
    get_file_path(path, obj->texture_path);
    get_file_path(path, obj->program_path);
    
    char *line = strtok( ( char * )filedata.buffer, "\n" ),
    str[ MAX_PATH ] = {""};
    
	kmVec3 v;
    
	while( line )
	{
		if( !line[ 0 ] || line[ 0 ] == '#' ) goto next_mat_line;
		
		else if( sscanf( line, "newmtl %s", str ) == 1 )
		{
			++obj->n_objmaterial;
			
			obj->objmaterial = ( OBJMATERIAL * ) realloc( obj->objmaterial, 
                                                         obj->n_objmaterial *
                                                         sizeof( OBJMATERIAL ) );
			
			objmaterial = &obj->objmaterial[ obj->n_objmaterial - 1 ];
			
			memset( objmaterial, 0, sizeof( OBJMATERIAL ) );
            
			strcpy( objmaterial->name, str );
		}
        
		else if( sscanf( line, "Ka %f %f %f", &v.x, &v.y, &v.z ) == 3 )
		{ memcpy( &objmaterial->ambient, &v, sizeof( kmVec3 ) ); }
        
		else if( sscanf( line, "Kd %f %f %f", &v.x, &v.y, &v.z ) == 3 )
		{ memcpy( &objmaterial->diffuse, &v, sizeof( kmVec3 ) ); }
        
		else if( sscanf( line, "Ks %f %f %f", &v.x, &v.y, &v.z ) == 3 )
		{ memcpy( &objmaterial->specular, &v, sizeof( kmVec3 ) ); }
        
		else if( sscanf( line, "Tf %f %f %f", &v.x, &v.y, &v.z ) == 3 )
		{ memcpy( &objmaterial->transmission_filter, &v, sizeof( kmVec3 ) ); }
        
		else if( sscanf( line, "illum %f", &v.x ) == 1 )
		{ objmaterial->illumination_model = ( int )v.x; }
        
		else if( sscanf( line, "d %f", &v.x ) == 1 )
		{
			objmaterial->ambient.w  = v.x;
			objmaterial->diffuse.w  = v.x;
			objmaterial->specular.w = v.x;
			objmaterial->dissolve   = v.x;
		}
        
		else if( sscanf( line, "Ns %f", &v.x ) == 1 )
		{ objmaterial->specular_exponent = v.x; }
        
		else if( sscanf( line, "Ni %f", &v.x ) == 1 )
		{ objmaterial->optical_density = v.x; }
        
		else if( sscanf( line, "map_Ka %s", str ) == 1 )
		{
			get_file_name( str, objmaterial->map_ambient );
			
			get_file_extension( objmaterial->map_ambient, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_ambient );
			
			else add_texture( objmaterial->map_ambient );
		}
        
		else if( sscanf( line, "map_Kd %s", str ) == 1 )
		{
			get_file_name( str, objmaterial->map_diffuse );
			
			get_file_extension( objmaterial->map_diffuse, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_diffuse );
			
			else add_texture( objmaterial->map_diffuse );
            
		}
        
		else if( sscanf( line, "map_Ks %s", str ) == 1 )
		{
			get_file_name( str, objmaterial->map_specular );
			
			get_file_extension( objmaterial->map_specular, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_specular );
			
			else add_texture( objmaterial->map_specular );
		}
        
		else if( sscanf( line, "map_Tr %s", str ) == 1 )
		{
			get_file_name( str, objmaterial->map_translucency );
			
			get_file_extension( objmaterial->map_translucency, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_translucency );
			
			else add_texture( objmaterial->map_translucency );
		}
        
		else if( sscanf( line, "map_disp %s", str ) == 1 ||
                sscanf( line, "map_Disp %s", str ) == 1 ||
                sscanf( line, "disp %s"    , str ) == 1 )
		{
			get_file_name( str, objmaterial->map_disp );
			
			get_file_extension( objmaterial->map_disp, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_disp );
			
			else add_texture( objmaterial->map_disp );
            
		}
        
		else if( sscanf( line, "map_bump %s", str ) == 1 ||
                sscanf( line, "map_Bump %s", str ) == 1 ||
                sscanf( line, "bump %s"	, str ) == 1 )
		{
			get_file_name( str, objmaterial->map_bump );
			
			get_file_extension( objmaterial->map_bump, str, 1 );
			
			if( !strcmp( str, "GFX" ) ) add_program( objmaterial->map_bump );
			
			else add_texture(objmaterial->map_bump );
		}
        
    next_mat_line:
		
        line = strtok( NULL, "\n" );
	}
    
    
	return 1;
}

int HLWavefrontObjComponent::get_program_index( char *filename )
{
    return 0;
}

void HLWavefrontObjComponent::add_program( char *filename )
{
    
}

void HLWavefrontObjComponent::add_texture(char *filename)
{
    HLTexture *tex = HLTexture::getTexture(filename);
    tex->retain();
    TexParams param = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    tex->setTexParameters(param);
    for (int i = 0; i < obj->n_texture; i++) 
    {
        if (tex == obj->texture[i]) 
        {
            tex->release();
            return;
        }
    }
    
    ++obj->n_texture;
    obj->texture = (HLTexture **) realloc(obj->texture, obj->n_texture*sizeof(HLTexture *));
    obj->texture[obj->n_texture - 1] = HLTexture::getTexture(filename);
    obj->texture[obj->n_texture - 1]->retain();
}

string HLWavefrontObjComponent::get_objfile()
{
    return m_objfile;
}

/*!
 Set all the necessary GLES machine state to draw a specific OBJMATERIAL.
 
 \param[in] objmaterial A valid OBJMATERIAL structure pointer.
 */
void OBJ_draw_material( OBJMATERIAL *objmaterial )
{
	if( objmaterial )
	{
//		if( objmaterial->program ) PROGRAM_draw( objmaterial->program );
        if (objmaterial->program) 
        {
            objmaterial->program->use();
            
            kmMat4 matrixP;
            kmMat4 matrixMV;
            kmMat4 tmp;
            kmMat3 matrixN;
            
            kmGLGetMatrix(KM_GL_PROJECTION, &matrixP );
            kmGLGetMatrix(KM_GL_MODELVIEW, &matrixMV );
            
            glUniform1i(glGetUniformLocation(objmaterial->program->getProgram(), "u_diffuse"), 1);
            if (objmaterial->illumination_model) 
            {
                kmMat4Inverse(&tmp, &matrixMV);
                kmMat4Transpose(&tmp, &tmp);
                kmMat4ExtractRotation(&matrixN, &tmp);
                glUniformMatrix4fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_ProjectionMatrix"), 1, GL_FALSE, matrixP.mat);
                glUniformMatrix4fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_ModelViewMatrix"), 1, GL_FALSE, matrixMV.mat);
                glUniform1f(glGetUniformLocation(objmaterial->program->getProgram(), "u_dissolve"), objmaterial->dissolve);
                glUniform3fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_ambientColor"), 1, (float*)&objmaterial->ambient);
                glUniform3fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_diffuseColor"), 1, (float*)&objmaterial->diffuse);
                glUniform3fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_specularColor"), 1, (float*)&objmaterial->specular);
                glUniform1f(glGetUniformLocation(objmaterial->program->getProgram(), "u_shininess"), objmaterial->specular_exponent * 0.128f);
                glUniformMatrix3fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_NormalMatrix"), 1, GL_FALSE, matrixN.mat);
                
                kmVec3 position = {0.0f, -3.0f, -10.0f};
                kmVec3 eyeposition;
                
                kmMat4 cameraMatrix = HLDirector3D::sharedDirector()->getRunningScene()->currentCameraMatrix();
                kmVec3TransformNormal(&eyeposition, &position, &cameraMatrix);
                glUniform3fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_lightPosition"), 1, (float *)&eyeposition);
            }
            else 
            {
                kmMat4Multiply(&tmp, &matrixP, &matrixMV);
                
                glUniformMatrix4fv(glGetUniformLocation(objmaterial->program->getProgram(), "u_MVPMatrix"), (GLsizei)1, GL_FALSE, tmp.mat);
            }
        }
        
		if( objmaterial->texture_ambient )
		{
			hlglActiveTexture( GL_TEXTURE0 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_ambient->getName());
		}
        
		
		if( objmaterial->texture_diffuse )
		{
			hlglActiveTexture( GL_TEXTURE1 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_diffuse->getName());
		}
        
		
		if( objmaterial->texture_specular )
		{
			hlglActiveTexture( GL_TEXTURE2 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_specular->getName());
		}
        
        
		if( objmaterial->texture_disp )
		{
			hlglActiveTexture( GL_TEXTURE3 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_disp->getName());
		}	
		
		
		if( objmaterial->texture_bump )
		{
			hlglActiveTexture( GL_TEXTURE4 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_bump->getName());
		}
        
        
		if( objmaterial->texture_translucency )
		{
			hlglActiveTexture( GL_TEXTURE5 );
            
			hlglBindTexture(GL_TEXTURE_2D, objmaterial->texture_translucency->getName());
		}
		
        
//		if( objmaterial->materialdrawcallback ) objmaterial->materialdrawcallback( objmaterial );
	}
}

void HLWavefrontObjComponent::drawMesh(int index)
{
    OBJMESH *objmesh = &obj->objmesh[ index ];
    
    unsigned int n = 0;
    
    if( objmesh->visible && objmesh->distance )
    {
        unsigned int i = 0;
        
        if( objmesh->vao ) glBindVertexArray( objmesh->vao );
        
        else set_attributes_mesh(index);
        
        while( i != objmesh->n_objtrianglelist )
        {
            objmesh->current_material = objmesh->objtrianglelist[ i ].objmaterial;
            
            if( objmesh->current_material ) OBJ_draw_material( objmesh->current_material );
            
            if( objmesh->vao )
            {
                if( objmesh->n_objtrianglelist != 1 )
                { glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[ i ].vbo ); }
            }
            else
            { glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, objmesh->objtrianglelist[ i ].vbo ); }
            
            glDrawElements( objmesh->objtrianglelist[ i ].mode,
                           objmesh->objtrianglelist[ i ].n_indice_array,
                           GL_UNSIGNED_SHORT,
                           ( void * )NULL );
            
            n += objmesh->objtrianglelist[ i ].n_indice_array;
            
            ++i;
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        hlglActiveTexture(GL_TEXTURE0);
    }
}

void HLWavefrontObjComponent::drawMeshWithMaterial(int type)
{
    for (int j = 0; j < obj->n_objmesh; j++) 
    {
        OBJMATERIAL *objmaterial = obj->objmesh[ j ].objtrianglelist[ 0 ].objmaterial;
        if (type == 0 && objmaterial->dissolve != 1.0f) 
        {
            continue;
        }
        
//        if (type == 1 && objmaterial->dissolve != 0.0f) 
//        {
//            continue;
//        }
//        
//        if (type == 2 && (objmaterial->dissolve<=0 || objmaterial->dissolve>=1)) {
//            continue;
//        }
        
        kmGLPushMatrix();
        
        kmGLTranslatef(obj->objmesh[j].location.x, obj->objmesh[j].location.y, obj->objmesh[j].location.z);
        
        if (objmaterial->dissolve >= 0 && objmaterial->dissolve < 1) 
        {
            glCullFace(GL_FRONT);
            drawMesh(j);
            glCullFace(GL_BACK);
            drawMesh(j);
        }
        else 
        {
            drawMesh(j);
        }
        
        
        kmGLPopMatrix();
    }
}

void HLWavefrontObjComponent::draw(HLEntity* entity)
{
    if (!obj) 
    {
        return;
    }
//    program->use();
//    program->setUniformForModelViewProjectionMatrix();
//    kmMat4 matrixP;
//    kmMat4 matrixMV;
//    kmMat4 tmp;
//    kmMat3 matrixN;
//    
//    kmMat4Inverse(&tmp, &matrixMV);
//    kmMat4Transpose(&tmp, &tmp);
//    kmMat4ExtractRotation(&matrixN, &tmp);
//    
//    glUniformMatrix3fv(glGetUniformLocation(program->getProgram(), "u_NormalMatrix"), 1, GL_FALSE, matrixN.mat);
//    glUniform1i(glGetUniformLocation(program->getProgram(), "u_diffuse"), 1);
    
//    kmVec3 l;
//    kmVec3Fill(&l, 0.25, 0.25, 1);
////    glUniform3f(glGetUniformLocation(program->getProgram(), "u_LightPosition"), 0.f, 0.f, 0.f);
//    glUniform3fv(glGetUniformLocation(program->getProgram(), "u_LightPosition"), 1, (const GLfloat*)&l);
    
    drawMeshWithMaterial(0);
//    drawMeshWithMaterial(1);
    
    hlglEnable( GL_BLEND );
	hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    drawMeshWithMaterial(1);
    hlglDisable( GL_BLEND );
////    glBindBuffer( GL_ARRAY_BUFFER, objmesh->vbo );
////    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,
////                 objmesh->objtrianglelist[ 0 ].vbo );
//    glBindVertexArray( objmesh->vao );
//    
//    hlglBindTexture(GL_TEXTURE_2D, texture->getName());
//    
//    glDrawElements( GL_TRIANGLES,
//                   objmesh->objtrianglelist[ 0 ].n_indice_array,
//                   GL_UNSIGNED_SHORT,
//                  (void *)NULL );
//    
//    glBindVertexArray(0);
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
////    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();
}

NS_HL_END

#endif

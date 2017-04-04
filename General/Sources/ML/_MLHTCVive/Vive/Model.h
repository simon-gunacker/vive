//------------------------------------------------------------------------------
// Model.h
//
// Authors: Simon Gunacker
//
//------------------------------------------------------------------------------

#pragma once
#include "_MLHTCViveSystem.h"
#include <WEMBase/WEMModuleBase/WEMProcessor.h>
#include <GL/glew.h>

//------------------------------------------------------------------------------
// Represents a model to be rendered to virtual reality.
//
class Model
{
public:

	//------------------------------------------------------------------------------
	// Default constructor. Nothing happens in here. We just need to state it to be 
	// able to pass a model object to a vives constructor.
	//
	Model();

	//------------------------------------------------------------------------------
	// Iterates over a given wem structure putting all vertices and normals into a 
	// simple vector. In addition, the scale of the model (i.e. the maximum extent) 
	// is evaluated.
	//
	// @param wem the wem structure to be rendered to vr
	//
	Model(const ml::WEMPtr wem);

	//------------------------------------------------------------------------------
	// transfers the internal model data (retrieved when iteration over the wem structure) 
	// to a vertex buffer object.
	//
	void toGPU();

	//------------------------------------------------------------------------------
	// @return the scale (i.e. the maximum extent) of the model. This information is 
	// placed into the model matrix.
	//
	float getScale();

	//------------------------------------------------------------------------------
	// @return the amount of values used to internally represent the object. This 
	// is later used by glDrawTriangles to render the model
	//	
	GLsizei getSize();

	//------------------------------------------------------------------------------
	// @return the index of the vbo holding the models data. Used when the model 
	// gets rendered
	//	
	GLuint getVBO();

	//------------------------------------------------------------------------------
	// Destruct the model. TODO: free VBO here?
	//	
	~Model();

private:

	// vector to store model data internally
	std::vector<float> _data;

	// used to compute the maximum extent of the model. Set when iterating the 
	// wem structure in load()
	float _xmin;
	float _xmax;
	float _ymin;
	float _ymax;
	float _zmin;
	float _zmax;
	float _scale;

	// remember the vbo index (will be needed when the object gets actaully rendered)
	GLuint _vbo;

	void load(const ml::WEMPtr wem);
	float computeScale();
	float max(float a, float b);
	float abs(float);
};


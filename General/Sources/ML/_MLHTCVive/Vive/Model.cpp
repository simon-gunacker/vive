#include "Model.h"

Model::Model()
{
}

Model::Model(const ml::WEMPtr wem)
{
	this->_data = std::vector<float>();
	this->_xmin = +1e+30;
	this->_xmax = -1e+30;
	this->_ymin = +1e+30;
	this->_ymax = -1e+30;
	this->_zmin = +1e+30;
	this->_zmax = -1e+30;

	load(wem);
}

void Model::toGPU()
{
	if (_data.size() == 0)
	{
		throw std::exception("Cannot init VR. Missing wem file.");
	}

	// generate vbo, bind it (for usage) and store wemdata into it
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), &_data[0], GL_STATIC_DRAW);

	// unbind vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float Model::getScale()
{
	return _scale;
}

GLsizei Model::getSize()
{
	return (GLsizei)_data.size();
}

GLuint Model::getVBO()
{
	return _vbo;
}

Model::~Model()
{
}

// read the wem struture and put all its vertices and normals into a simple vector
void Model::load(const ml::WEMPtr wem)
{
	if (wem)
	{
		// for all patches
		for (unsigned int i = 0; i < wem->getNumWEMPatches(); i++)
		{
			ml::WEMPatch* wemPatch = wem->getWEMPatchAt(i);
			if (!wemPatch) break;

			ml::Vector3 center = wemPatch->getBoundingBox()->getCenter();

			// for all faces of patch
			for (unsigned int j = 0; j < wemPatch->getNumFaces(); j++)
			{
				ml::WEMFace* wemFace = wemPatch->getFaceAt(j);
				if (!wemFace) break;

				// for all vertices of face
				for (int k = 0; k < 3; k++)
				{
					ml::WEMNode* wemNode = wemFace->getNodeAt(k);
					if (!wemNode) break;

					wemNode->translate(-center);

					ml::Vector3 pos = wemNode->getPosition();
					ml::Vector3 norm = wemNode->getNormal();

					wemNode->translate(center);

					this->_data.push_back(pos.x);
					this->_data.push_back(pos.y);
					this->_data.push_back(pos.z);
					this->_data.push_back(norm.x);
					this->_data.push_back(norm.y);
					this->_data.push_back(norm.z);

					if (pos.x < _xmin) _xmin = pos.x; if (pos.x > _xmax) _xmax = pos.x;
					if (pos.y < _ymin) _ymin = pos.y; if (pos.y > _ymax) _ymax = pos.y;
					if (pos.z < _zmin) _zmin = pos.z; if (pos.z > _zmax) _zmax = pos.z;
				}
			}
		}
		_scale = computeScale();
	}
}

// gets the maximum extent of the model
float Model::computeScale()
{
	float scale = abs(this->_xmin);
	scale = max(scale, abs(this->_ymin));
	scale = max(scale, abs(this->_zmin));
	scale = max(scale, abs(this->_xmax));
	scale = max(scale, abs(this->_ymax));
	scale = max(scale, abs(this->_zmax));
	return scale;
}

// used by compute scale. Nothing special in here ...
float Model::max(float a, float b)
{
	return (a > b) ? a : b;
}

// used by compute scale. Nothing special in here ...
float Model::abs(float val)
{
	return (val < 0) ? -val : val;
}

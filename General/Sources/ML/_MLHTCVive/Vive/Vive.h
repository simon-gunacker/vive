//------------------------------------------------------------------------------
// Vive.h
//
// Authors: Simon Gunacker
//
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <openvr.h>
#include <Vive/Model.h>

// macro for inline shader definition
#define GLSL(version, shader)  "#version " #version "\n" #shader

//------------------------------------------------------------------------------
// Models a head mounted device.
//
class Vive
{
public:
	//------------------------------------------------------------------------------
	// Inits a Vive Object. Initialization basically constists of
	// * init hmd
	// * init compositior
	// * init open gl (we need a gl context for rendering)
	// * init framebuffers (one for each eye, this is where the model is rendered to)
	// * init shaders (for rendering)
	//
	// @param model the model to be rendered by the vive object
	//
	Vive(Model& model);

	//------------------------------------------------------------------------------
	// Renders the model associated with the Vive object.
	//
	// @param run rendering stops as soon as run (controlled outside of thread where 
	//            the vive lives in) gets false.
	//
	void render(bool* run);

	//------------------------------------------------------------------------------
	// Opens the compositor mirror window (showing on screen what is rendered on the hmd)
	//
	void showMirrorWindow();

	//------------------------------------------------------------------------------
	// Shutdown VR and cleanup OpenGL.
	//
	~Vive();

private:

	// the model that has to be rendered
	Model _model;

	// the hmd device itself
	vr::IVRSystem* _hmd;

	// pose matrices of tracked devices (we are only interested in the hmd, 
	// but this can also be used for controllers
	vr::TrackedDevicePose_t _trackedPoses[vr::k_unMaxTrackedDeviceCount];
	
	// the vr compositor
	vr::IVRCompositor* _compositor;

	// the OpenGL window (hidden, just need this for having a context)
	GLFWwindow* _window;

	// the framebuffers for left and right eye
	GLuint _framebuffer[2];

	// width of framebuffer
	uint32_t _framebufferWidth;

	// height of framebiffer
	uint32_t _framebufferHeight;

	// used by framebuffer
	GLuint _colorRenderTarget[2];

	// used by framebuffer
	GLuint _depthRenderTarget[2];

	// reference to compiled shader program in gpu
	GLuint _shader;

	// reference to mvp matrix used in shader program
	GLuint _mvp_shader_reference;

	void initHmd();
	void initCompositor();
	void initOpenGl();
	void initFramebuffers();
	void initShaders();
	GLuint compileShader(GLenum shaderType, const GLchar* shaderCode);
	void getShaderCompilationInfo(GLuint shaderId);
	void getProgramCompilationInfo(GLuint programId);
	void renderFrame();
	glm::mat4 getProjectionMatrix(int eye);
	glm::mat4 getEyeMatrix(int eye);
	glm::mat4 getPoseMatrix();
	glm::mat4 getModelMatrix();
};


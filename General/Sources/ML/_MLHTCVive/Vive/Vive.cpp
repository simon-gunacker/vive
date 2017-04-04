//------------------------------------------------------------------------------
// Vive.cpp
//
// Authors: Simon Gunacker
//
//------------------------------------------------------------------------------

#include "Vive.h"
#include <exception>

Vive::Vive(Model& model)
{
	// always call in this order!
	initHmd();
	initCompositor();
	initOpenGl();
	initFramebuffers();
	initShaders();

	_model = model;
	_model.toGPU();
}

void Vive::showMirrorWindow()
{
	vr::VRCompositor()->ShowMirrorWindow();
}

void Vive::render(bool* run)
{
	do {
		renderFrame();
		glfwSwapBuffers(_window);
		glfwPollEvents();
	} while (*run);
}

Vive::~Vive()
{
	vr::VR_Shutdown();
	glfwTerminate();
}

void Vive::initHmd()
{
	vr::EVRInitError err = vr::VRInitError_None;
	_hmd = vr::VR_Init(&err, vr::VRApplication_Scene);
	if (err != vr::VRInitError_None)
	{
		throw std::exception(vr::VR_GetVRInitErrorAsEnglishDescription(err));
	}
}

void Vive::initCompositor()
{
	_compositor = vr::VRCompositor();
	if (!_compositor)
	{
		vr::VR_Shutdown();
		throw std::exception("OpenVR Compositor initialization failed");
	}
}

// we need a window for getting our OpenGL context. 
// We use glfw to simplify window generation.
void Vive::initOpenGl()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		throw std::exception("Failed to initialize GLFW");
	}

	// window drawing hints
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // since we use compositior->ShowMirrorWindow

	// create window
	_window = glfwCreateWindow(1024, 768, "context", NULL, NULL);
	if (_window == NULL) 
	{
		glfwTerminate();
		throw std::exception("Failed to open GLFW window");
	}

	// set opengl context
	glfwMakeContextCurrent(_window);

	// init glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		throw std::exception("Failed to initialize GLEW");
	}

	// init once
	// stores attrib pointers describing the internal structure of all vbos
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	// capture escape key
	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);
}

// we generate custom framebuffers. 
// There is one for each eye. The are used to render 
// our model to hmd.
void Vive::initFramebuffers()
{
	// get framebuffersizes from hmd
	_hmd->GetRecommendedRenderTargetSize(&_framebufferWidth, &_framebufferHeight);

	// generate a framebuffer for each eye
	glGenFramebuffers(2, _framebuffer);
	glGenTextures(2, _colorRenderTarget);
	glGenTextures(2, _depthRenderTarget);
	for (int eye = 0; eye < 2; eye++)
	{
		glBindTexture(GL_TEXTURE_2D, _colorRenderTarget[eye]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _framebufferWidth, _framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, _depthRenderTarget[eye]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _framebufferWidth, _framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[eye]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorRenderTarget[eye], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthRenderTarget[eye], 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// our shaders. The are used for positioning and coloring 
// of our model.
void Vive::initShaders()
{
	// code for vertex shader
	const GLchar* vert = GLSL
	(
		410 core,
		uniform mat4 MVP;
		layout(location = 0) in vec3 position_modelspace;
		layout(location = 1) in vec3 normal_modelspace;
		out vec3 v3Normal;
		out vec3 v3Position;
		void main() 
		{
			vec4 position = MVP * vec4(position_modelspace, 1);

			v3Normal.xyz = normal_modelspace.xyz;
			v3Position = position.xyz;

			gl_Position = position;
		}
	);

	// code for fragment shader
	const GLchar* frag = GLSL
	(
		410 core,
		in vec3 v3Normal;
		in vec3 v3Position;
		out vec4 outputColor;
		void main() 
		{
			// light settings
			vec3 pos = vec3(0.0, 0.0, -5.0);
			vec4 amb = vec4(vec3(0.2), 1.0);
			vec4 dif = vec4(vec3(0.3), 1.0);
			vec4 spc = vec4(vec3(0.1), 1.0);

			vec3 light = normalize(pos - v3Position);
			vec3 eye = normalize(-v3Position);
			vec3 reflection = normalize(-reflect(light, v3Normal));

			//calculate light per vertex
			vec4 ambient = amb;
			vec4 diffuse = dif * abs(dot(light, v3Normal));
			vec4 specular = spc * pow(max(dot(reflection, eye), 0.0), 10);

			outputColor.xyz = ambient.xyz + diffuse.xyz + specular.xyz;
			// outputColor.a = 0.6;
		}
	);

	// compile shaders
	GLuint VertexShaderID = compileShader(GL_VERTEX_SHADER, vert);
	GLuint FragmentShaderID = compileShader(GL_FRAGMENT_SHADER, frag);

	// link shader pipeline
	_shader = glCreateProgram();
	glAttachShader(_shader, VertexShaderID);
	glAttachShader(_shader, FragmentShaderID);
	glLinkProgram(_shader);
	getProgramCompilationInfo(_shader);

	// free resources
	glDetachShader(_shader, VertexShaderID);
	glDetachShader(_shader, FragmentShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	_mvp_shader_reference = glGetUniformLocation(_shader, "MVP");
}

GLuint Vive::compileShader(GLenum shaderType, const GLchar* shaderCode)
{
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &shaderCode, NULL);
	glCompileShader(shaderId);
	getShaderCompilationInfo(shaderId);
	return shaderId;
}

void Vive::getShaderCompilationInfo(GLuint shaderId)
{
	GLint result = GL_FALSE;
	int loglen;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &loglen);

	if (result != GL_TRUE && loglen > 0)
	{
		std::vector<char> err(loglen + 1);
		glGetShaderInfoLog(shaderId, loglen, NULL, &err[0]);
		throw std::exception(std::string(err.begin(), err.end()).c_str());
	}
}

void Vive::getProgramCompilationInfo(GLuint programId)
{
	GLint result = GL_FALSE;
	int loglen;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &loglen);
	if (result != GL_TRUE && loglen > 0)
	{
		std::vector<char> err(loglen + 1);
		glGetProgramInfoLog(_shader, loglen, NULL, &err[0]);
		throw std::exception(std::string(err.begin(), err.end()).c_str());
	}
}

// this is the actual rendering process. We require successful initialization here.
// this method renders a single frame. It is called inside a loop in render().
void Vive::renderFrame()
{
	// compositor not in found -> call WaitGetPoses
	// https://github.com/ValveSoftware/openvr/issues/296 (search for notfound)
	vr::VRCompositor()->WaitGetPoses(&_trackedPoses[0], vr::k_unMaxTrackedDeviceCount, NULL, 0);

	for (int eye = 0; eye < 2; eye++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[eye]);
		glViewport(0, 0, _framebufferWidth, _framebufferHeight);
		glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glm::mat4 projection = getProjectionMatrix(eye);
		glm::mat4 eyepos = getEyeMatrix(eye);
		glm::mat4 pose = getPoseMatrix();
		glm::mat4 model = getModelMatrix();
		glm::mat4 mvp = projection * eyepos * pose * model;

		glUseProgram(_shader);
		glUniformMatrix4fv(_mvp_shader_reference, 1, GL_FALSE, &mvp[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, _model.getVBO());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, _model.getSize());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(_colorRenderTarget[eye])), vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::EVREye(eye), &tex);
	}
}

// used for transforming camera space into projection space. This is what is 
// eventually rendered to the framebuffers.
glm::mat4 Vive::getProjectionMatrix(int eye)
{
	vr::HmdMatrix44_t projection = _hmd->GetProjectionMatrix(vr::EVREye(eye), 1.0f, 100.0f, vr::EGraphicsAPIConvention::API_OpenGL);
	return glm::mat4(
		projection.m[0][0], projection.m[1][0], projection.m[2][0], projection.m[3][0],
		projection.m[0][1], projection.m[1][1], projection.m[2][1], projection.m[3][1],
		projection.m[0][2], projection.m[1][2], projection.m[2][2], projection.m[3][2],
		projection.m[0][3], projection.m[1][3], projection.m[2][3], projection.m[3][3]
	);
}

// for each eye, there is a given offset. Together with the pose matrix, 
// this is the virtual realities counterpart to the view matrix.
// it transforms world space into camera space.
glm::mat4 Vive::getEyeMatrix(int eye)
{
	vr::HmdMatrix34_t eyepos = _hmd->GetEyeToHeadTransform(vr::EVREye(eye));
	return glm::inverse(glm::mat4(
		eyepos.m[0][0], eyepos.m[1][0], eyepos.m[2][0], 0.0,
		eyepos.m[0][1], eyepos.m[1][1], eyepos.m[2][1], 0.0,
		eyepos.m[0][2], eyepos.m[1][2], eyepos.m[2][2], 0.0,
		eyepos.m[0][3], eyepos.m[1][3], eyepos.m[2][3], 1.0f
	));
}

// the pose of the hmd device. Together with the eye matrix, 
// this is the virtual realities counterpart to the view matrix.
// it transforms world space into camera space.
glm::mat4 Vive::getPoseMatrix()
{
	// see https://github.com/ValveSoftware/openvr/wiki/IVRSystem::GetDeviceToAbsoluteTrackingPose
	vr::HmdMatrix34_t pose = _trackedPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
	return glm::inverse(glm::mat4(
		pose.m[0][0], pose.m[1][0], pose.m[2][0], 0.0,
		pose.m[0][1], pose.m[1][1], pose.m[2][1], 0.0,
		pose.m[0][2], pose.m[1][2], pose.m[2][2], 0.0,
		pose.m[0][3], pose.m[1][3], pose.m[2][3], 1.0f
	));
}

// used for transforming model space (relative model coordinates)
// into world space (absolute world coordinates)
// this is also responsible for scaling the object.
// this is the only place where we need to change any values when 
// we want to change the position of an object
glm::mat4 Vive::getModelMatrix()
{
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Translate = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);
	glm::mat4 Model = glm::mat4(1.0f);

	Scale = glm::scale(Scale, glm::vec3(1 / _model.getScale()));
	Translate = glm::translate(Translate, glm::vec3(0.0f, 2.0f, -2.0f));
	Rotate = glm::rotate(Rotate, (-3.14f / 2), glm::vec3(1, 0, 0));

	return Translate * Rotate * Scale;
}

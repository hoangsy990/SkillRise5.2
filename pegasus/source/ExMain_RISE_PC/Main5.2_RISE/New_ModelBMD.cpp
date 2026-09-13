#include "stdafx.h"
#include "New_ModelBMD.h"
#include "RISE/PegasusRuntimeCapacity.h"
#include "RISE/PegasusRuntimeTrace.h"

#ifdef jdk_shader_local330

#include "GlobalBitmap.h"
#include "MU_OpenGL.h"
#include "New_RenderBMD.h"
#include "TextureScript.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

using namespace OGL330;

extern double WorldTime;
extern int WaterTextureNumber;

namespace
{
	struct ShaderLocations
	{
		GLint Projection;
		GLint View;
		GLint BodyLight;
		GLint LightPosition;
		GLint MeshUv;
		GLint Setting1;
		GLint Setting2;
		GLint EnableLight;
		GLint Texture;
		GLint ShadowMode;
		GLint UseTexture;
		GLint BodyOrigin;
		GLint Bones;

		ShaderLocations()
			: Projection(-1), View(-1), BodyLight(-1), LightPosition(-1), MeshUv(-1),
			  Setting1(-1), Setting2(-1), EnableLight(-1), Texture(-1), ShadowMode(-1),
			  UseTexture(-1), BodyOrigin(-1), Bones(-1)
		{
		}
	};

	std::unordered_map<int, GLuint> g_shaderPrograms;
	std::unordered_map<GLuint, ShaderLocations> g_shaderLocations;
	GLuint g_currentShader = 0;
	float g_batchProjection[16];
	float g_batchView[16];
	bool g_batchMatricesValid = false;
	std::vector<BMD*> g_gpuSkinningOnlyModels;

	void LogShaderError(const std::string& message)
	{
		std::string output = "OGL330: " + message + "\n";
		OutputDebugStringA(output.c_str());
	}

	ShaderLocations& GetShaderLocations(GLuint program)
	{
		std::unordered_map<GLuint, ShaderLocations>::iterator found = g_shaderLocations.find(program);
		if (found != g_shaderLocations.end())
		{
			return found->second;
		}

		ShaderLocations& locations = g_shaderLocations[program];
		locations.Projection = glGetUniformLocation(program, "uProj");
		locations.View = glGetUniformLocation(program, "uView");
		locations.BodyLight = glGetUniformLocation(program, "u_bodyLight");
		locations.LightPosition = glGetUniformLocation(program, "u_lightPosition");
		locations.MeshUv = glGetUniformLocation(program, "u_meshUV");
		locations.Setting1 = glGetUniformLocation(program, "u_setting1");
		locations.Setting2 = glGetUniformLocation(program, "u_setting2");
		locations.EnableLight = glGetUniformLocation(program, "u_enableLight");
		locations.Texture = glGetUniformLocation(program, "uTexture");
		locations.ShadowMode = glGetUniformLocation(program, "u_shadowMode");
		locations.UseTexture = glGetUniformLocation(program, "u_useTexture");
		locations.BodyOrigin = glGetUniformLocation(program, "u_bodyOrigin");
		locations.Bones = glGetUniformLocation(program, "u_Bones");
		return locations;
	}

	std::string LoadTextFile(const char* path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			LogShaderError(std::string("cannot open shader file ") + path);
			return std::string();
		}

		std::ostringstream contents;
		contents << file.rdbuf();
		return contents.str();
	}

	GLuint CompileShader(GLenum type, const std::string& source, const char* path)
	{
		if (source.empty())
		{
			return 0;
		}

		GLuint shader = glCreateShader(type);
		const char* text = source.c_str();
		glShaderSource(shader, 1, &text, NULL);
		glCompileShader(shader);

		GLint compiled = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> log(max(1, length));
			glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), NULL, &log[0]);
			LogShaderError(std::string("compile failed for ") + path + ": " + &log[0]);
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	GLuint LoadShaderProgram(const char* vertexPath, const char* fragmentPath)
	{
		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, LoadTextFile(vertexPath), vertexPath);
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, LoadTextFile(fragmentPath), fragmentPath);
		if (vertexShader == 0 || fragmentShader == 0)
		{
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			return 0;
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linked = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE)
		{
			GLint length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> log(max(1, length));
			glGetProgramInfoLog(program, static_cast<GLsizei>(log.size()), NULL, &log[0]);
			LogShaderError(std::string("link failed for ") + vertexPath + " and " + fragmentPath + ": " + &log[0]);
			glDeleteProgram(program);
			program = 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return program;
	}
}

OGL330MODEL::RenderMeshVAO::RenderMeshVAO()
	: OldBmd(NULL), MeshIndex(-1), TextureId(-1), RenderFlags(0), EnableLight(false),
	  Shader(0), Alpha(1.0f), BoneMatrices(NULL), Transform(false),
	  RequestScale(1.0f), BodyScale(1.0f)
{
	Color.x = 1.0f;
	Color.y = 1.0f;
	Color.z = 1.0f;
}

bool OGL330MODEL::Init()
{
	Release();

	const char* shaderNames[SHADER_330_ALL] =
	{
		"Model", "BlendMesh",
		"Chrome1", "Chrome2", "Chrome3", "Chrome4", "Chrome5", "Chrome6", "Chrome7",
		"Oil", "Metal"
	};

	bool success = true;
	for (int i = 0; i < SHADER_330_ALL; ++i)
	{
		char vertexPath[MAX_PATH];
		char fragmentPath[MAX_PATH];
		sprintf(vertexPath, "Data\\Effect\\VBO\\%s.vs", shaderNames[i]);
		sprintf(fragmentPath, "Data\\Effect\\VBO\\%s.fs", shaderNames[i]);

		GLuint program = LoadShaderProgram(vertexPath, fragmentPath);
		g_shaderPrograms[i] = program;
		if (program == 0)
		{
			success = false;
		}
	}

	if (!success)
	{
		Release();
	}
	return success;
}

bool OGL330MODEL::CanUseGpuSkinningOnly(BMD* model)
{
	if (!OGL330::IsShader() || !GMMeshShader->Enabled() || g_NewRenderBMD == NULL ||
		model == NULL || model->Meshs == NULL || model->NewMeshes == NULL ||
		model->NumMeshs <= 0 ||
		model->NewMeshes->size() < static_cast<size_t>(model->NumMeshs))
	{
		return false;
	}

	bool hasDrawableMesh = false;
	for (int i = 0; i < model->NumMeshs; ++i)
	{
		const Mesh_t& source = model->Meshs[i];
		if (source.NumVertices <= 0 || source.NumTriangles <= 0)
		{
			continue;
		}

		const VAOMesh& mesh = (*model->NewMeshes)[i];
		if (mesh.VAO == 0 || mesh.VBO == 0 || mesh.IBO == 0 ||
			mesh.IndexCount == 0 || mesh.Bones.empty() || mesh.Bones.size() > 85)
		{
			return false;
		}
		hasDrawableMesh = true;
	}

	return hasDrawableMesh;
}

bool OGL330MODEL::ShouldSkipCpuSkinning(BMD* model)
{
	return !g_gpuSkinningOnlyModels.empty() &&
		g_gpuSkinningOnlyModels.back() == model &&
		CanUseGpuSkinningOnly(model);
}

OGL330MODEL::GpuSkinningScope::GpuSkinningScope(BMD* model, bool enabled)
	: m_active(enabled && CanUseGpuSkinningOnly(model))
{
	if (m_active)
	{
		g_gpuSkinningOnlyModels.push_back(model);
	}
}

OGL330MODEL::GpuSkinningScope::~GpuSkinningScope()
{
	if (m_active && !g_gpuSkinningOnlyModels.empty())
	{
		g_gpuSkinningOnlyModels.pop_back();
	}
}

void OGL330MODEL::Release()
{
	for (std::unordered_map<int, GLuint>::iterator it = g_shaderPrograms.begin(); it != g_shaderPrograms.end(); ++it)
	{
		if (it->second != 0)
		{
			glDeleteProgram(it->second);
		}
	}

	g_shaderPrograms.clear();
	g_shaderLocations.clear();
	g_currentShader = 0;
	g_gpuSkinningOnlyModels.clear();
	GMMeshShader->Release();
}

void OGL330MODEL::ConvertOldMeshToVaoMesh(GLuint modelId)
{
	// The isolated Pegasus effects live in the explicitly allocated tail after
	// MAX_MODELS.  Rejecting that tail here left the BMD loaded on the CPU but
	// without VAO/GPU data, so casts were received while their models vanished
	// only on the shader renderer.
	if (!OGL330::IsShader() || Models == NULL ||
		!rise::pegasus::IsRuntimeModelId(modelId, MAX_MODELS))
	{
		return;
	}

	BMD* model = &Models[modelId];
	if (model->NumMeshs <= 0)
	{
		if (modelId >= MAX_MODELS)
			rise::pegasus::TraceRuntime("GPU_UPLOAD", static_cast<int>(modelId),
				model->NumMeshs, "NO_MESH");
		return;
	}

	model->LoadMeshToVAO();
	model->UploadAllToGPU();
	if (modelId >= MAX_MODELS)
		rise::pegasus::TraceRuntime("GPU_UPLOAD", static_cast<int>(modelId),
			model->NumMeshs, "PASS");
}

void OGL330MODEL::UseShader(GLuint shaderId)
{
	if (shaderId != 0 && g_currentShader != shaderId)
	{
		glUseProgram(shaderId);
		g_currentShader = shaderId;
	}
}

void OGL330MODEL::UnUseShader()
{
	if (g_currentShader != 0)
	{
		glUseProgram(0);
		g_currentShader = 0;
	}
}

void OGL330MODEL::SetTargetRender(OBJECT* object)
{
	if (!OGL330::IsShader())
	{
		return;
	}

	if (object != NULL)
	{
		GMMeshShader->BeginScope();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		GMMeshShader->EndScope();
	}
}

void OGL330MODEL::SendUniform(
	GLuint shaderId,
	const mvec4& bodyLight,
	const mvec4& lightPosition,
	const mvec4& meshUv,
	const mvec4& setting1,
	const mvec4& setting2,
	bool enableLight,
	bool shadow,
	bool useTexture,
	vec3_t bodyOrigin)
{
	float projection[16];
	float view[16];
	const float* projectionMatrix = g_batchProjection;
	const float* viewMatrix = g_batchView;
	if (!g_batchMatricesValid)
	{
		glGetFloatv(GL_PROJECTION_MATRIX, projection);
		glGetFloatv(GL_MODELVIEW_MATRIX, view);
		projectionMatrix = projection;
		viewMatrix = view;
	}

	ShaderLocations& locations = GetShaderLocations(shaderId);
	if (locations.Projection != -1) glUniformMatrix4fv(locations.Projection, 1, GL_FALSE, projectionMatrix);
	if (locations.View != -1) glUniformMatrix4fv(locations.View, 1, GL_FALSE, viewMatrix);
	if (locations.BodyLight != -1) glUniform4f(locations.BodyLight, bodyLight.x, bodyLight.y, bodyLight.z, bodyLight.w);
	if (locations.LightPosition != -1) glUniform4f(locations.LightPosition, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
	if (locations.MeshUv != -1) glUniform4f(locations.MeshUv, meshUv.x, meshUv.y, meshUv.z, meshUv.w);
	if (locations.Setting1 != -1) glUniform4f(locations.Setting1, setting1.x, setting1.y, setting1.z, setting1.w);
	if (locations.Setting2 != -1) glUniform4f(locations.Setting2, setting2.x, setting2.y, setting2.z, setting2.w);
	if (locations.EnableLight != -1) glUniform1i(locations.EnableLight, enableLight ? 1 : 0);
	if (locations.Texture != -1) glUniform1i(locations.Texture, 0);
	if (locations.ShadowMode != -1) glUniform1i(locations.ShadowMode, shadow ? 1 : 0);
	if (locations.UseTexture != -1) glUniform1i(locations.UseTexture, useTexture ? 1 : 0);
	if (locations.BodyOrigin != -1) glUniform3f(locations.BodyOrigin, bodyOrigin[0], bodyOrigin[1], bodyOrigin[2]);
}

GLint OGL330MODEL::GetBoneUniformLocation(GLuint shaderId)
{
	return GetShaderLocations(shaderId).Bones;
}

OGL330MODEL::CGMMeshShader::CGMMeshShader()
	: m_scopeDepth(0), m_enabled(true), m_transform(false), m_finalBone(NULL),
	  m_transformModel(NULL)
{
	m_data.reserve(16);
	memset(m_lightPositionOrigin, 0, sizeof(vec3_t));
	memset(m_lightDirectionOrigin, 0, sizeof(vec3_t));
	memset(m_lightDirection, 0, sizeof(vec3_t));
	SetHighLight(false, false);
}

OGL330MODEL::CGMMeshShader::~CGMMeshShader()
{
	Release();
}

OGL330MODEL::CGMMeshShader* OGL330MODEL::CGMMeshShader::Instance()
{
	static CGMMeshShader instance;
	return &instance;
}

void OGL330MODEL::CGMMeshShader::BeginScope()
{
	++m_scopeDepth;
}

void OGL330MODEL::CGMMeshShader::EndScope()
{
	if (m_scopeDepth > 0)
	{
		--m_scopeDepth;
	}

	if (m_scopeDepth == 0)
	{
		FlushAllMesh();
	}
}

void OGL330MODEL::CGMMeshShader::Release()
{
	m_data.clear();
	m_scopeDepth = 0;
	m_finalBone = NULL;
	m_transformModel = NULL;
	m_transform = false;
}

bool OGL330MODEL::CGMMeshShader::IsAlpha(int renderFlags) const
{
	return (renderFlags & RENDER_BRIGHT) != 0 ||
		(renderFlags & RENDER_CHROME3) != 0 ||
		(renderFlags & RENDER_CHROME4) != 0 ||
		(renderFlags & RENDER_CHROME5) != 0 ||
		(renderFlags & RENDER_CHROME7) != 0;
}

void OGL330MODEL::CGMMeshShader::SetHighLight(bool highLight, bool battleCastle)
{
	if (highLight)
	{
		Vector(1.3f, 0.0f, 2.0f, m_lightPositionOrigin);
	}
	else if (battleCastle)
	{
		Vector(0.5f, -1.0f, 1.0f, m_lightPositionOrigin);
	}
	else
	{
		Vector(0.0f, -1.5f, 0.0f, m_lightPositionOrigin);
	}

	VectorCopy(m_lightPositionOrigin, m_lightDirectionOrigin);
}

void OGL330MODEL::CGMMeshShader::AddBoneTransform(BMD* model, float(*boneMatrix)[3][4], bool transform)
{
	m_transform = transform;
	m_finalBone = boneMatrix == NULL ? NULL : boneMatrix[0][0];
	m_transformModel = model;
}

void OGL330MODEL::CGMMeshShader::MakeShaderType(
	int shaderType,
	bool enableLight,
	bool alphaInColor,
	float blendU,
	float blendV,
	RenderMeshVAO& command)
{
	mvec4 bodyLight;
	if (alphaInColor)
	{
		bodyLight.x = command.Color.x * command.Alpha;
		bodyLight.y = command.Color.y * command.Alpha;
		bodyLight.z = command.Color.z * command.Alpha;
		bodyLight.w = 1.0f;
	}
	else
	{
		bodyLight.x = command.Color.x;
		bodyLight.y = command.Color.y;
		bodyLight.z = command.Color.z;
		bodyLight.w = command.Alpha;
	}

	mvec4 lightPosition;
	lightPosition.w = command.Alpha;
	if (enableLight)
	{
		lightPosition.x = m_lightDirection[0];
		lightPosition.y = m_lightDirection[1];
		lightPosition.z = m_lightDirection[2];
	}

	mvec4 meshUv;
	mvec4 setting1;
	mvec4 setting2;

	switch (shaderType)
	{
	case SHADER_330_BLENDMESH:
	case SHADER_330_OIL:
		meshUv.x = blendU;
		meshUv.y = blendV;
		meshUv.z = 1.0f;
		break;
	case SHADER_330_CHROME1:
		setting1.x = 1.0f;
		setting1.z = static_cast<int>(WorldTime) % 10000 * 0.0001f;
		setting2.x = 0.5f;
		setting2.y = 0.5f;
		setting2.z = 2.0f;
		setting2.w = 1.0f;
		break;
	case SHADER_330_CHROME2:
		setting1.x = 1.0f;
		setting1.w = static_cast<int>(WorldTime) % 5000 * 0.00024f - 0.4f;
		setting2.x = 0.8f;
		setting2.y = 2.0f;
		setting2.z = 1.0f;
		setting2.w = 3.0f;
		break;
	case SHADER_330_CHROME3:
		setting2.x = 0.0f;
		setting2.y = -0.1f;
		setting2.z = -0.8f;
		setting2.w = 1.0f;
		break;
	case SHADER_330_CHROME4:
		setting1.x = cosf(static_cast<float>(WorldTime) * 0.001f);
		setting1.y = sinf(static_cast<float>(WorldTime) * 0.002f);
		setting1.z = 1.0f;
		setting1.w = static_cast<int>(WorldTime) % 10000 * 0.0001f;
		meshUv.x = blendU;
		meshUv.y = blendV;
		meshUv.z = 1.0f;
		setting2.x = 0.5f;
		setting2.y = 3.0f;
		setting2.z = 0.5f;
		setting2.w = 3.0f;
		break;
	case SHADER_330_CHROME5:
		setting1.x = cosf(static_cast<float>(WorldTime) * 0.001f);
		setting1.y = sinf(static_cast<float>(WorldTime) * 0.002f);
		setting1.z = 1.0f;
		setting1.w = static_cast<int>(WorldTime) % 10000 * 0.0001f;
		setting2.x = 2.5f;
		setting2.y = 1.0f;
		setting2.z = 3.0f;
		setting2.w = 5.0f;
		break;
	case SHADER_330_CHROME6:
		setting1.x = 0.8f;
		setting1.y = 2.0f;
		setting1.z = static_cast<int>(WorldTime) % 5000 * 0.00024f - 0.4f;
		break;
	case SHADER_330_CHROME7:
		setting1.x = 0.8f;
		setting1.y = 0.8f;
		setting1.z = static_cast<float>(WorldTime);
		setting1.w = 0.00006f;
		break;
	case SHADER_330_METAL:
		setting2.x = 0.5f;
		setting2.y = 0.2f;
		setting2.z = 0.5f;
		setting2.w = 0.5f;
		break;
	default:
		break;
	}

	command.BodyLight = bodyLight;
	command.LightPosition = lightPosition;
	command.MeshUv = meshUv;
	command.Setting1 = setting1;
	command.Setting2 = setting2;
	command.Shader = g_shaderPrograms[shaderType];
}

bool OGL330MODEL::CGMMeshShader::AddMeshCommand(
	BMD* source,
	int meshIndex,
	int renderFlags,
	float alpha,
	int blendMesh,
	float blendMeshLight,
	float blendU,
	float blendV,
	int texture)
{
	if (!m_enabled || source == NULL || source != m_transformModel || m_finalBone == NULL ||
		source->NewMeshes == NULL ||
		meshIndex < 0 || meshIndex >= source->NumMeshs ||
		meshIndex >= static_cast<int>(source->NewMeshes->size()))
	{
		return false;
	}

	VAOMesh& newMesh = (*source->NewMeshes)[meshIndex];
	if (newMesh.VAO == 0)
	{
		return false;
	}

	int sourceTexture = source->IndexTexture[newMesh.Texture];
	if (sourceTexture == BITMAP_HIDE)
	{
		return true;
	}

	switch (sourceTexture)
	{
	case BITMAP_SKIN:
		if (source->HideSkin) return true;
		sourceTexture = BITMAP_SKIN + source->Skin;
		break;
	case BITMAP_WATER:
		sourceTexture = BITMAP_WATER + WaterTextureNumber;
		break;
	case BITMAP_HAIR:
		if (source->HideSkin) return true;
		sourceTexture = BITMAP_HAIR + (source->Skin - 8);
		break;
	default:
		break;
	}

	if (texture != -1)
	{
		sourceTexture = texture;
	}

	int shaderType = SHADER_330_NONE;
	int shaderTexture = -1;
	OGL330::GetTextureShader(renderFlags, shaderTexture, shaderType);

	if (shaderType != SHADER_330_NONE)
	{
		if (newMesh.NoneBlendMesh ||
			(newMesh.TextureScriptData != NULL && newMesh.TextureScriptData->getNoneBlendMesh()))
		{
			return true;
		}
	}

	RenderMeshVAO command;
	command.OldBmd = source;
	command.MeshIndex = meshIndex;
	command.TextureId = shaderTexture == -1 ? sourceTexture : shaderTexture;
	command.RenderFlags = renderFlags;
	command.Alpha = alpha > 0.99f ? 1.0f : alpha;
	command.EnableLight = source->LightEnable && !source->bOffLight;
	command.BoneMatrices = m_finalBone;
	command.Transform = m_transform;
	command.RequestScale = source->RequestScale;
	command.BodyScale = source->BodyScale;
	command.BodyOrigin.x = source->BodyOrigin[0];
	command.BodyOrigin.y = source->BodyOrigin[1];
	command.BodyOrigin.z = source->BodyOrigin[2];
	command.Color.x = source->BodyLight[0];
	command.Color.y = source->BodyLight[1];
	command.Color.z = source->BodyLight[2];

	if (shaderType == SHADER_330_NONE)
	{
		if (newMesh.TextureScriptData != NULL && newMesh.TextureScriptData->getStreamMesh())
		{
			shaderType = SHADER_330_BLENDMESH;
		}
		else if (meshIndex == blendMesh || meshIndex == source->StreamMesh)
		{
			shaderType = SHADER_330_BLENDMESH;
			command.EnableLight = false;
		}

		if (blendMesh <= -2 || newMesh.Texture == blendMesh)
		{
			// The legacy renderer keeps blend meshes subtractive when the
			// caller explicitly requests RENDER_DARK.
			if ((command.RenderFlags & RENDER_DARK) == 0)
			{
				command.RenderFlags |= RENDER_BRIGHT;
			}
			if (blendU != 0.0f || blendV != 0.0f)
			{
				shaderType = SHADER_330_BLENDMESH;
			}

			command.Color.x *= blendMeshLight;
			command.Color.y *= blendMeshLight;
			command.Color.z *= blendMeshLight;
			command.Alpha = 1.0f;
			command.EnableLight = false;
		}
	}
	else
	{
		command.EnableLight = false;
	}

	if (command.EnableLight)
	{
		vec34_t matrix;
		Vector(0.0f, 0.0f, -45.0f, source->ShadowAngle);
		AngleMatrix(source->ShadowAngle, matrix);
		VectorIRotate(m_lightDirectionOrigin, matrix, m_lightDirection);
	}

	MakeShaderType(shaderType, command.EnableLight, IsAlpha(renderFlags), blendU, blendV, command);
	if (command.Shader == 0)
	{
		return false;
	}

	m_data.push_back(command);
	if (m_scopeDepth == 0)
	{
		FlushAllMesh();
	}
	return true;
}

void OGL330MODEL::CGMMeshShader::FlushAllMesh()
{
	if (m_data.empty() || g_NewRenderBMD == NULL)
	{
		return;
	}

	glGetFloatv(GL_PROJECTION_MATRIX, g_batchProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_batchView);
	g_batchMatricesValid = true;

	for (MeshVAO::iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		g_NewRenderBMD->Render(*it);
	}

	g_batchMatricesValid = false;
	glBindVertexArray(0);
	OGL330MODEL::UnUseShader();
	DisableAlphaBlend();
	EnableDepthTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_data.clear();
}

#endif

#include "stdafx.h"
#include "New_RenderBMD.h"

#ifdef jdk_shader_local330

#include "GlobalBitmap.h"
#include "ZzzBMD.h"
#if defined(PEGASUS_ISOLATED_TEST)
#include "RISE/PegasusRuntimeTrace.h"
#include "RISE/PegasusDragonViolentRuntime.h"
#include "RISE/PegasusSwordWrathRuntime.h"
#include "RISE/PegasusHavocSpearRuntime.h"
#endif

CGMNewRenderBMD* g_NewRenderBMD = NULL;

void CGMShaderBMD::Render(OGL330MODEL::RenderMeshVAO& command)
{
	BMD& model = *command.OldBmd;
	if (model.NewMeshes == NULL ||
		command.MeshIndex < 0 ||
		command.MeshIndex >= static_cast<int>(model.NewMeshes->size()))
	{
		return;
	}

	VAOMesh& mesh = (*model.NewMeshes)[command.MeshIndex];
	if (mesh.VAO == 0 || mesh.IndexCount == 0)
	{
		return;
	}

	OGL330MODEL::UseShader(command.Shader);

	bool useTexture = false;
	if ((command.RenderFlags & RENDER_COLOR) == RENDER_COLOR)
	{
		if ((command.RenderFlags & RENDER_BRIGHT) == RENDER_BRIGHT)
		{
			EnableAlphaBlend();
		}
		else if ((command.RenderFlags & RENDER_DARK) == RENDER_DARK)
		{
			EnableAlphaBlendMinus();
		}
		else
		{
			DisableAlphaBlend();
		}

		if ((command.RenderFlags & RENDER_NODEPTH) == RENDER_NODEPTH)
		{
			DisableDepthTest();
		}

		DisableTexture();
		if (command.Alpha < 0.99f)
		{
			EnableAlphaTest();
		}
	}
	else if ((command.RenderFlags & RENDER_CHROME) == RENDER_CHROME ||
		(command.RenderFlags & RENDER_CHROME2) == RENDER_CHROME2 ||
		(command.RenderFlags & RENDER_CHROME3) == RENDER_CHROME3 ||
		(command.RenderFlags & RENDER_CHROME4) == RENDER_CHROME4 ||
		(command.RenderFlags & RENDER_CHROME5) == RENDER_CHROME5 ||
		(command.RenderFlags & RENDER_CHROME6) == RENDER_CHROME6 ||
		(command.RenderFlags & RENDER_CHROME7) == RENDER_CHROME7 ||
		(command.RenderFlags & RENDER_METAL) == RENDER_METAL ||
		(command.RenderFlags & RENDER_OIL) == RENDER_OIL)
	{
		useTexture = true;
		glActiveTexture(GL_TEXTURE0);
		BindTexture(command.TextureId);

		if ((command.RenderFlags & RENDER_CHROME3) == RENDER_CHROME3 ||
			(command.RenderFlags & RENDER_CHROME4) == RENDER_CHROME4 ||
			(command.RenderFlags & RENDER_CHROME5) == RENDER_CHROME5 ||
			(command.RenderFlags & RENDER_CHROME7) == RENDER_CHROME7 ||
			(command.RenderFlags & RENDER_BRIGHT) == RENDER_BRIGHT)
		{
			EnableAlphaBlend();
		}
		else if ((command.RenderFlags & RENDER_DARK) == RENDER_DARK)
		{
			EnableAlphaBlendMinus();
		}
		else if ((command.RenderFlags & RENDER_LIGHTMAP) == RENDER_LIGHTMAP)
		{
			EnableLightMap();
		}
		else if (command.Alpha >= 0.99f)
		{
			DisableAlphaBlend();
		}
		else
		{
			EnableAlphaTest();
		}

		if ((command.RenderFlags & RENDER_NODEPTH) == RENDER_NODEPTH)
		{
			DisableDepthTest();
		}
	}
	else if ((command.RenderFlags & RENDER_TEXTURE) == RENDER_TEXTURE)
	{
		useTexture = true;
		glActiveTexture(GL_TEXTURE0);
		BindTexture(command.TextureId);

		if ((command.RenderFlags & RENDER_BRIGHT) == RENDER_BRIGHT)
		{
			EnableAlphaBlend();
		}
		else if ((command.RenderFlags & RENDER_DARK) == RENDER_DARK)
		{
			EnableAlphaBlendMinus();
		}
		else
		{
			BITMAP_t* bitmap = Bitmaps.GetTexture(command.TextureId);
			if (command.Alpha < 0.99f || (bitmap != NULL && bitmap->Components == 4))
			{
				EnableAlphaTest();
			}
			else
			{
				DisableAlphaBlend();
			}
		}

		if ((command.RenderFlags & RENDER_NODEPTH) == RENDER_NODEPTH)
		{
			DisableDepthTest();
		}
	}
	else if ((command.RenderFlags & RENDER_BRIGHT) == RENDER_BRIGHT)
	{
		EnableAlphaBlend();
		DisableTexture();
		DisableDepthMask();

		if ((command.RenderFlags & RENDER_NODEPTH) == RENDER_NODEPTH)
		{
			DisableDepthTest();
		}
	}

	const float requestScale = command.RequestScale;
	const bool applyRequestScale = requestScale != 0.0f && requestScale != 1.0f;
	vec3_t bodyOrigin =
	{
		command.BodyOrigin.x,
		command.BodyOrigin.y,
		command.BodyOrigin.z
	};

	OGL330MODEL::SendUniform(
		command.Shader,
		command.BodyLight,
		command.LightPosition,
		command.MeshUv,
		command.Setting1,
		command.Setting2,
		command.EnableLight,
		(command.RenderFlags & RENDER_SHADOWMAP) != 0,
		useTexture,
		bodyOrigin);

	if (!mesh.SendIndexBone(
		command.Shader,
		command.BoneMatrices,
		command.Transform,
		bodyOrigin,
		command.BodyScale,
		applyRequestScale,
		requestScale))
	{
		return;
	}

	glBindVertexArray(mesh.VAO);
#if defined(PEGASUS_ISOLATED_TEST)
	// Sample the executed command, not mutable OBJECT state or GL restored
	// after queueing. Three pulse bands bound this diagnostic to three rows.
	static unsigned tracedWrathPulseBands = 0;
	if (command.OldBmd == &Models[rise::pegasus::kSwordWrathRootModel] &&
		command.MeshIndex == 0 && command.BodyLight.x > 0.0f)
	{
		// The native blend-slot path bakes pulse into RGB and sets queued
		// Alpha=1. Wrath's authored red channel is1, so executed red is pulse.
		const unsigned band = command.BodyLight.x < 0.35f ? 0u :
			command.BodyLight.x < 0.65f ? 1u : 2u;
		if ((tracedWrathPulseBands & (1u << band)) == 0)
		{
			tracedWrathPulseBands |= 1u << band;
			GLint program = 0, sourceBlend = 0, destinationBlend = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &program);
			glGetIntegerv(GL_BLEND_SRC, &sourceBlend);
			glGetIntegerv(GL_BLEND_DST, &destinationBlend);
			char detail[384];
			_snprintf_s(detail, sizeof(detail), _TRUNCATE,
				"program=%d texture=%d flags=%d blend=%d,%d enabled=%d alpha=%.5f rgba=%.5f,%.5f,%.5f,%.5f lit=%d textured=%d",
				program, command.TextureId, command.RenderFlags, sourceBlend, destinationBlend,
				static_cast<int>(glIsEnabled(GL_BLEND)), command.Alpha, command.BodyLight.x,
				command.BodyLight.y, command.BodyLight.z, command.BodyLight.w,
				static_cast<int>(command.EnableLight), static_cast<int>(useTexture));
			rise::pegasus::TraceRuntime("SWORD_WRATH_SHADER_DRAW", 1500, band, detail);
		}
	}
	static bool tracedDragonHead = false;
	// Capture both authored scale families: impact starts at1.6, caster at8.
	// A single global latch previously proved only the first caster ring.
	static bool tracedHavocPair[2] = {};
	const int havocScaleBand = command.BodyScale < 4.0f ? 0 : 1;
	static bool hasHavocBase = false;
	static float havocBaseKey[4] = {};
	static char havocBaseDetail[640] = {};
	if (!tracedHavocPair[havocScaleBand] && command.OldBmd == &Models[rise::pegasus::kHavocShockwaveModel] &&
		command.MeshIndex == 0 && model.IndexTexture && model.Meshs)
	{
		const bool basePass = command.TextureId == model.IndexTexture[model.Meshs[0].Texture];
		GLint program = 0, src = 0, dst = 0, depthFunc = 0;
		GLint minFilter = 0, magFilter = 0, wrapS = 0, wrapT = 0;
		GLboolean depthWrite = GL_FALSE;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		glGetIntegerv(GL_BLEND_SRC, &src);
		glGetIntegerv(GL_BLEND_DST, &dst);
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
		char detail[640];
		_snprintf_s(detail, sizeof(detail), _TRUNCATE,
			"program=%d texture=%d flags=%d blend=%d,%d depth=%d func=%d write=%d filter=%d,%d wrap=%d,%d scale=%.5f origin=%.3f,%.3f,%.3f rgba=%.5f,%.5f,%.5f,%.5f",
			program, command.TextureId, command.RenderFlags, src, dst,
			static_cast<int>(glIsEnabled(GL_DEPTH_TEST)), depthFunc, static_cast<int>(depthWrite),
			minFilter, magFilter, wrapS, wrapT, command.BodyScale,
			command.BodyOrigin.x, command.BodyOrigin.y, command.BodyOrigin.z,
			command.BodyLight.x, command.BodyLight.y, command.BodyLight.z, command.BodyLight.w);
		if (basePass)
		{
			hasHavocBase = true;
			havocBaseKey[0] = command.BodyOrigin.x;
			havocBaseKey[1] = command.BodyOrigin.y;
			havocBaseKey[2] = command.BodyOrigin.z;
			havocBaseKey[3] = command.BodyScale;
			strcpy_s(havocBaseDetail, detail);
		}
		else if (hasHavocBase && havocBaseKey[0] == command.BodyOrigin.x &&
			havocBaseKey[1] == command.BodyOrigin.y && havocBaseKey[2] == command.BodyOrigin.z &&
			havocBaseKey[3] == command.BodyScale)
		{
			tracedHavocPair[havocScaleBand] = true;
			rise::pegasus::TraceRuntime("HAVOC_SHOCKWAVE_SHADER_PAIR", 2013, 0, havocBaseDetail);
			rise::pegasus::TraceRuntime("HAVOC_SHOCKWAVE_SHADER_PAIR", 2013, 1, detail);
		}
	}
	if (!tracedDragonHead && command.OldBmd == &Models[rise::pegasus::kDragonViolentHeadModel] &&
		command.MeshIndex == 0 && command.BodyLight.z > 0.0f)
	{
		tracedDragonHead = true;
		GLint program = 0, sourceBlend = 0, destinationBlend = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		glGetIntegerv(GL_BLEND_SRC, &sourceBlend);
		glGetIntegerv(GL_BLEND_DST, &destinationBlend);
		char detail[320];
		_snprintf_s(detail, sizeof(detail), _TRUNCATE,
			"program=%d texture=%d flags=%d blend=%d,%d enabled=%d rgba=%.4f,%.4f,%.4f,%.4f lit=%d",
			program, command.TextureId, command.RenderFlags, sourceBlend, destinationBlend,
			static_cast<int>(glIsEnabled(GL_BLEND)), command.BodyLight.x,
			command.BodyLight.y, command.BodyLight.z, command.BodyLight.w,
			static_cast<int>(command.EnableLight));
		rise::pegasus::TraceRuntime("DRAGON_HEAD_SHADER_DRAW", 242, 0, detail);
	}
	// Inspect the queued draw, not the restored GL state after RenderMesh returns.
	static bool tracedSwordTail = false;
	std::vector<unsigned char> tailBefore;
	GLint tailViewport[4] = {};
	if (!tracedSwordTail && command.OldBmd == &Models[MODEL_PEGASUS_SWORD_BLOW_SMALL] &&
		command.MeshIndex == 1 && command.BodyLight.z > 0.05f)
	{
		tracedSwordTail = true;
		GLint program = 0, texture = 0, sourceBlend = 0, destinationBlend = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
		glGetIntegerv(GL_BLEND_SRC, &sourceBlend);
		glGetIntegerv(GL_BLEND_DST, &destinationBlend);
		char detail[384];
		_snprintf_s(detail, sizeof(detail), _TRUNCATE,
			"program=%d textureId=%d glTexture=%d blend=%d,%d enabled=%d rgba=%.5f,%.5f,%.5f,%.5f lit=%d textured=%d flags=%d",
			program, command.TextureId, texture, sourceBlend, destinationBlend,
			static_cast<int>(glIsEnabled(GL_BLEND)), command.BodyLight.x,
			command.BodyLight.y, command.BodyLight.z, command.BodyLight.w,
			static_cast<int>(command.EnableLight), static_cast<int>(useTexture), command.RenderFlags);
		rise::pegasus::TraceRuntime("SWORD_BLOW_TAIL_SHADER_DRAW", 1501, 1, detail);
		// One before/after read around this exact draw separates its emitted
		// colour from terrain, target layers and other overlapping swords.
		glGetIntegerv(GL_VIEWPORT, tailViewport);
		if (tailViewport[2] > 0 && tailViewport[3] > 0 &&
			tailViewport[2] <= 8192 && tailViewport[3] <= 8192)
		{
			tailBefore.resize(static_cast<size_t>(tailViewport[2]) * tailViewport[3] * 4);
			glReadPixels(tailViewport[0], tailViewport[1], tailViewport[2], tailViewport[3],
				GL_RGBA, GL_UNSIGNED_BYTE, tailBefore.data());
		}
	}
#endif
	glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, NULL);
#if defined(PEGASUS_ISOLATED_TEST)
	if (!tailBefore.empty())
	{
		std::vector<unsigned char> tailAfter(tailBefore.size());
		glReadPixels(tailViewport[0], tailViewport[1], tailViewport[2], tailViewport[3],
			GL_RGBA, GL_UNSIGNED_BYTE, tailAfter.data());
		unsigned int pixels = 0;
		double delta[3] = {};
		for (size_t i = 0; i < tailBefore.size(); i += 4)
		{
			if (tailAfter[i] >= 250 || tailAfter[i + 1] >= 250 || tailAfter[i + 2] >= 250 ||
				static_cast<int>(tailAfter[i + 2]) - tailBefore[i + 2] < 3)
				continue;
			++pixels;
			for (int channel = 0; channel < 3; ++channel)
				delta[channel] += static_cast<int>(tailAfter[i + channel]) - tailBefore[i + channel];
		}
		char detail[256];
		_snprintf_s(detail, sizeof(detail), _TRUNCATE,
			"pixels=%u delta=%.0f,%.0f,%.0f ratio=%.5f,%.5f,1 viewport=%d,%d",
			pixels, delta[0], delta[1], delta[2], delta[2] > 0 ? delta[0] / delta[2] : 0,
			delta[2] > 0 ? delta[1] / delta[2] : 0, tailViewport[2], tailViewport[3]);
		rise::pegasus::TraceRuntime("SWORD_BLOW_TAIL_FRAMEBUFFER_DELTA", 1501, 1, detail);
	}
#endif
}

#endif

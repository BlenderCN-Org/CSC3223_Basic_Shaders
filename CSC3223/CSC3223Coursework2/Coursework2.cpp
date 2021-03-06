#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"
#include "../../Common/Maths.h"

#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

#include "Renderer.h"

using namespace NCL;
using namespace CSC3223;

OGLShader* getLightingShader() {
	return new OGLShader("LightingVert.glsl", "LightingFrag.glsl");
}

OGLShader* getShrinkShader() {
	return new OGLShader("ShrinkVert.glsl", "ShrinkFrag.glsl");
}

OGLShader* getFadeShader() {
	return new OGLShader("FadeVert.glsl", "FadeFrag.glsl");
}

OGLShader* getTexBlendShader() {
	return new OGLShader("TextureBlendVert.glsl", "TextureBlendFrag.glsl");
}

RenderObject* Coursework2(Renderer &renderer) {
	OGLMesh* m = new OGLMesh("cube.msh");
	vector<Vector4> col;
	col.reserve(m->GetIndexCount());
	for (int i = 0; i < m->GetIndexCount(); i++) {
		col.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	}
	m->SetVertexColours(col);
	m->SetPrimitiveType(GeometryPrimitive::Triangles);
	m->UploadToGPU();

	RenderObject* object = new RenderObject(m);
	object->SetShader(getFadeShader());

	TextureBase* doge = OGLTexture::RGBATextureFromFilename("doge.PNG");
	TextureBase* brick = OGLTexture::RGBATextureFromFilename("brick.PNG");

	object->SetBaseTexture(doge);
	object->SetSecondaryTexture(brick);

	renderer.AddRenderObject(object);

	renderer.SetLightProperties(Vector3(0, 25, 0), Vector3(1.0f, 1.0f, 1.0f), 200.0f);

	return object;
}

int main() {
	Window*w = Window::CreateGameWindow("CSC3223 Coursework 2!");

	if (!w->HasInitialised()) {
		return -1;
	}

	Renderer* renderer = new Renderer(*w);
	renderer->EnableDepthBuffer(true);
	
	RenderObject* cube = Coursework2(*renderer);

	renderer->SetProjectionMatrix(Matrix4::Perspective(1, 1000, w->GetScreenAspect(), 45.0f));

	float rotation = 0.0f;

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer->SetAlphaBlendingState(true);
		renderer->SetBlendToLinear();
		float time = w->GetTimer()->GetTimeDelta();
		rotation += time * 0.1f;
		renderer->Update(time);

		Matrix4 modelMat = Matrix4::Translation(Vector3(0, 0, -10));

		modelMat = modelMat * Matrix4::Rotation(rotation, Vector3(1, 1, 1));

		cube->SetTransform(modelMat);

		renderer->DrawString("OpenGL Rendering!", Vector2(10, 10));

		renderer->Render();

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_HOME)) {
			w->SetFullScreen(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_END)) {
			w->SetFullScreen(false);
		}

		w->SetTitle(std::to_string(time));
	}

	delete renderer;

	Window::DestroyGameWindow();
}
#ifndef _SKY_H_
#define _SKY_H_

#include "../Camera/BaseCamera.h"
#include "../Maths/CommonMaths.h"

#include "../Texture/Texture.h"

#include <d3d11_1.h>

#include "../Shaders/ShaderHandler.h"
#include <vector>

class SkyDome final
{
public:
	SkyDome() = delete;
	SkyDome(ShaderHandler& shaderHandler, Vector3D centre, float radius, unsigned int divisions);
	~SkyDome();

	void Render(BaseCamera* camera);
	void Update(const float deltaTime);

private:
	void GenerateDome();

	void CalculateVerticies();
	void CalculateIndicies();

	void SetupShaders();

	void FillLUT();

	Vector3D     mDomeCentre;
	float        mRadius;
	unsigned int mDivisions;
	
	// Shaders for the final render
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader*  mPixelShader;

	// Shaders for the LUT texture
	ID3D11ComputeShader* mLUTGenerationComputeShader;

	// Buffer holding the vertex data for the dome
	ID3D11Buffer*       mVertexBuffer;
	ID3D11Buffer*       mIndexBuffer;

	ID3D11Buffer*       mSkyConstantBuffer;
	ID3D11Buffer*       mTimeOfDayConstantBuffer;
	float               mTimeOfDay;

	unsigned int        mVertexBufferStride;
	unsigned int        mIndexCount;
	unsigned int        mVertexCount;

	ShaderHandler&      mShaderHandler;

	ID3D11InputLayout*  mInputLayout;

	DirectX::XMFLOAT4X4 mModelMat;

	ID3D11RasterizerState* renderState;

	// 3D Lookup table for the pre-computed data
	// U - height
	// V - View angle 
	// S - Sun angle
	Texture3D*             mLUTTexture;
	SamplerState*          mSamplerState;
};

#endif
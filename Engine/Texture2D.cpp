#include "Texture2D.hpp"

Window* Texture2D::window = nullptr;

Texture2D::~Texture2D()
{
	SafeDelete(this->sampleState);
	SafeDelete(this->textrue);
}



Texture2D::Texture2D(std::string src, std::string vs, std::string ps, UINT MaxInstance)
{
	Init(src,vs,ps, MaxInstance);
}



void Texture2D::Init(std::string src, std::string vs, std::string ps, UINT MaxInstance)
{
	const auto& devcon = window->GetContext();
	const auto& dev = window->GetDevice();


	CreateVertexTexBuffer(MaxInstance);


	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;


	polygonLayout[1].SemanticName = "tex";
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "POS";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;

	polygonLayout[3].SemanticName = "SIZE";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;


	InitializeShaders(vs.c_str(), "main", ps.c_str(), "main", polygonLayout, 4);

	this->CreateSample();



	// Load the texture in.
	CheckFAILED(D3DX11CreateShaderResourceViewFromFileA(dev, src.c_str(), NULL, NULL, &textrue, NULL));

	//D3DX11_IMAGE_INFO m_info;
	//ID3D11Resource* res = nullptr;
	//ID3D11Texture2D* tex;
	//D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//D3D11_TEXTURE2D_DESC texDesc;
	//void* dataPtr;
	//D3DX11GetImageInfoFromFileA(src.c_str(), NULL, &m_info, NULL);
	//textrue->GetResource(&res);
	//textrue->GetDesc(&dc);

	//texDesc.ArraySize = m_info.ArraySize;
	//texDesc.BindFlags = 0;
	//texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ| D3D11_CPU_ACCESS_WRITE;
	//texDesc.Format = m_info.Format;
	//texDesc.Width = m_info.Width;  // must be same as backbuffer
	//texDesc.Height = m_info.Height; // must be same as backbuffer
	//texDesc.MipLevels = dc.Texture2D.MipLevels;
	//texDesc.MiscFlags = m_info.MiscFlags;
	//texDesc.SampleDesc.Count = 1;
	//texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage = D3D11_USAGE_STAGING;

	//
	//CheckFAILED(dev->CreateTexture2D(&texDesc, 0, &tex));
	//devcon->CopyResource(tex, res);
	//CheckFAILED(devcon->Map(tex, 0, D3D11_MAP_READ_WRITE, 0, &mappedResource));
	//dataPtr = mappedResource.pData;
	//BYTE* arr = new BYTE[(UINT64)300 * (UINT64)mappedResource.RowPitch];
	//memcpy(arr, dataPtr, (UINT64)300 * (UINT64)mappedResource.RowPitch);

	//for (UINT32 i = 0; i < (UINT64)300 * (UINT64)mappedResource.RowPitch; i+=4)
	//{
	//	UINT32 sum = 0;
	//	for (int j = 0; j < 4; j++)
	//		sum += arr[i+j];
	//	if (sum == 255*4)
	//	{
	//		arr[i + 0] = 0;
	//		arr[i + 1] = 255;
	//		arr[i + 2] = 255;
	//		arr[i + 3] = 0;
	//	}
	//}
	//memcpy(dataPtr, arr, (UINT64)300  * (UINT64)mappedResource.RowPitch);
	//devcon->Unmap(tex, 0);

	//std::string str = "without_alpha_" + src.substr(0, src.find(".")) + ".png";
	//D3DX11SaveTextureToFileA(devcon,tex, D3DX11_IFF_PNG, str.c_str());
	//delete[] arr;
	//tex->Release();
	//res->Release();

}

void Texture2D::AddInstance(const fVec2& pos,const fVec2& size,const Camera& camera)
{
	TextrueInstanceType in;
	//fVec3 screenWorld = (p - cPos);
	
	//fVec2 screenWorld = pos - camera.GetPos().ToVec2();
	//if(camera.InScreen(screenWorld))
	
	in.pos = pos;
	in.size = size/2;
	Model11::AddInstance(in);

}


void Texture2D::Draw(bool clearAfter)
{
	if (textrue)
	{
		const auto& devcon = window->GetContext();
		devcon->PSSetShaderResources(0, 1, &textrue);
		devcon->PSSetSamplers(0, 1, &sampleState);
		Model11::Draw();
		devcon->PSSetShaderResources(0, 0, 0);
		devcon->PSSetSamplers(0, 0, 0);
	}
	else
	{
		Model11::Draw();
	}
	if(clearAfter)
		ClearInstance();
}

void Texture2D::CreateSample()
{
	const auto& dev = window->GetDevice();
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	// Create the texture sampler state.
	CheckFAILED(dev->CreateSamplerState(&samplerDesc, &sampleState));
}

void Texture2D::CreateVertexTexBuffer(UINT MaxInstance)
{
	const auto& devcon = window->GetContext();
	const auto& dev = window->GetDevice();


	struct TextrueVertex
	{
		fVec3 pos;
		fVec2 tex;
	};

	TextrueVertex vertex[4];

	vertex[0].pos = fVec3(-1.0f, -1.0f, 0.0f);
	vertex[1].pos = fVec3(-1.0f, 1.0f, 0.0f);
	vertex[2].pos = fVec3(1.0f, 1.0f, 0.0f);
	vertex[3].pos = fVec3(1.0f, -1.0f, 0.0f);

	vertex[0].tex = fVec2(0, 1);
	vertex[1].tex = fVec2(0, 0);
	vertex[2].tex = fVec2(1, 0);
	vertex[3].tex = fVec2(1, 1);

	unsigned int ind[] =
	{
		0,1,2,
		0,2,3
	};

	InitBuffer(dev, devcon, vertex, ind, 4, 6, MaxInstance, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sizeof(TextrueVertex), sizeof(TextrueInstanceType));
}

void Texture2D::CreateVertexColorBuffer(UINT MaxInstance)
{
	const auto& devcon = window->GetContext();
	const auto& dev = window->GetDevice();


	struct TextrueVertex
	{
		fVec3 pos;
	};

	TextrueVertex vertex[4];

	vertex[0].pos = fVec3(-1.0f, -1.0f, 0.0f);
	vertex[1].pos = fVec3(-1.0f, 1.0f, 0.0f);
	vertex[2].pos = fVec3(1.0f, 1.0f, 0.0f);
	vertex[3].pos = fVec3(1.0f, -1.0f, 0.0f);


	unsigned int ind[] =
	{
		0,1,2,
		0,2,3
	};

	InitBuffer(dev, devcon, vertex, ind, 4, 6, MaxInstance, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sizeof(TextrueVertex), sizeof(TextrueInstanceType));

}




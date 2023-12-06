#pragma once
#include "BaseMesh.h"

#ifndef _TESSMESH_H_
#define _TESSMESH_H_

class TessellationPlane : public BaseMesh
{
public:
	TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution);
	~TessellationPlane();

protected:
	void initBuffers(ID3D11Device* device);

private:
	int resolution;
};
#endif

#pragma once
#include "PCH.h"

class Bitmap
{
private:
	ID2D1Bitmap* pBitmap = nullptr;
	ID2D1BitmapRenderTarget* pBitmapRenderTarget = nullptr;
	D2D1_POINT_2F m_Pivot = {};
	UINT m_PixelWidth = 0;
	UINT m_PixelHeight = 0;
	UINT m_Pitch = 0;
	D2D1_SIZE_F m_Size = {};
	float halfwidth = 0.0f;
	float halfheight = 0.0f;
public:
	~Bitmap();
	HRESULT CreateGraphicsResources(ID2D1HwndRenderTarget* rt, int resource, BYTE r, BYTE g, BYTE b, float pivotx, float pivoty, float scale);
	void Draw(ID2D1HwndRenderTarget* rt, float angle, float x, float y);
	void DiscardGraphicsResources();
private:
	std::vector<BYTE> FileLoader(int resource);
};
#include "PCH.h"
#include "DigitalClock.h"
#include "Direct2D.h"
#include "Math.h"

DigitalClock::~DigitalClock()
{
	DiscardGraphicsResources();
}

void DigitalClock::SevenSegment::Segment::Init(ID2D1Factory* pD2DFactory, BOOL vertical, float longdist, float shortdist, float halfwidth, float sk, D2D1_POINT_2F offset)
{
	HRESULT hr = pD2DFactory->CreatePathGeometry(&Geometry);
	if (FAILED(hr)) return;

	hr = Geometry->Open(&Sink);
	if (FAILED(hr)) return;

	Sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	D2D1_POINT_2F p1 = { -longdist, 0 };
	D2D1_POINT_2F p2 = { -shortdist,-halfwidth };
	D2D1_POINT_2F p3 = { shortdist  ,-halfwidth };
	D2D1_POINT_2F p4 = { longdist, 0 };
	D2D1_POINT_2F p5 = { shortdist, halfwidth };
	D2D1_POINT_2F p6 = { -shortdist, halfwidth };
	if (vertical)
	{
		p1 = { 0, -longdist };
		p2 = { -halfwidth, -shortdist };
		p3 = { -halfwidth, shortdist };
		p4 = { 0, longdist };
		p5 = { halfwidth, shortdist };
		p6 = { halfwidth , -shortdist };
	}
	//float sk = -0.1f;
	Sink->BeginFigure(skew(p1 + offset, sk), D2D1_FIGURE_BEGIN_FILLED);
	Sink->AddLine(skew(p2 + offset, sk));
	Sink->AddLine(skew(p3 + offset, sk));
	Sink->AddLine(skew(p4 + offset, sk));
	Sink->AddLine(skew(p5 + offset, sk));
	Sink->AddLine(skew(p6 + offset, sk));
	Sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	hr = Sink->Close();
}

DigitalClock::SevenSegment::Segment::~Segment()
{
	SafeRelease(&Geometry);
	SafeRelease(&Sink);
}

void DigitalClock::SevenSegment::Segment::DrawSegment(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush)
{
	pRenderTarget->FillGeometry(Geometry, pBrush);
}

void DigitalClock::SevenSegment::InitGeometry(ID2D1Factory* pD2DFactory)
{
	const float longdist = 31.0f;
	const float shortdist = 19.0f;
	const float halfwidth = 12.0f;
	const float sk = -0.2f;
	const float space = 1.5f;
	D2D1_POINT_2F segoffset = { 0.0f, -longdist * 2.0f };
	D2D1_POINT_2F segmentSpacing = { 0.0f, -space * 2.0f };
	D2D1_POINT_2F offset = segoffset + segmentSpacing;
	Seg[0].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { -longdist, -longdist };
	segmentSpacing = { -space, -space };
	offset = segoffset + segmentSpacing;
	Seg[1].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { longdist, -longdist };
	segmentSpacing = { space, -space };
	offset = segoffset + segmentSpacing;
	Seg[2].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { 0.0f, 0.0f };
	segmentSpacing = { 0.0f, 0.0f };
	offset = segoffset + segmentSpacing;
	Seg[3].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { -longdist, longdist };
	segmentSpacing = { -space, space };
	offset = segoffset + segmentSpacing;
	Seg[4].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { longdist, longdist };
	segmentSpacing = { space, space };
	offset = segoffset + segmentSpacing;
	Seg[5].Init(pD2DFactory, TRUE, longdist, shortdist, halfwidth, sk, offset);
	segoffset = { 0.0f, longdist * 2.0f };
	segmentSpacing = { 0.0f, space * 2.0f };
	offset = segoffset + segmentSpacing;
	Seg[6].Init(pD2DFactory, FALSE, longdist, shortdist, halfwidth, sk, offset);
}

HRESULT DigitalClock::CreateGraphicsResources(ID2D1HwndRenderTarget* pRenderTarget)
{
	HRESULT hr = S_OK;
	D2D1::ColorF color = D2D1::ColorF(0.02f, 0.02f, 0.02f, 1.0f);
	if (pNormalBrush == nullptr)
		hr = pRenderTarget->CreateSolidColorBrush(color, &pNormalBrush);
	if (FAILED(hr))
		return hr;

	color = D2D1::ColorF(0.8f, 0.04f, 0.04f, 1.0f);
	if (pHighlightBrush == nullptr)
		hr = pRenderTarget->CreateSolidColorBrush(color, &pHighlightBrush);
	if (FAILED(hr))
		return hr;

	return hr;
}

void DigitalClock::DiscardGraphicsResources()
{
	SafeRelease(&pNormalBrush);
	SafeRelease(&pHighlightBrush);
}

void DigitalClock::SevenSegment::Draw(ID2D1HwndRenderTarget* pRenderTarget, int value, ID2D1SolidColorBrush* pFullBrush)
{
	switch (value)
	{
	case 0:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 1:
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 2:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 3:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 4:
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 5:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 6:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 7:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 8:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 9:
		Seg[0].DrawSegment(pRenderTarget, pFullBrush);
		Seg[1].DrawSegment(pRenderTarget, pFullBrush);
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case -1:
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		break;
	case 'd':
		Seg[2].DrawSegment(pRenderTarget, pFullBrush);
		Seg[3].DrawSegment(pRenderTarget, pFullBrush);
		Seg[4].DrawSegment(pRenderTarget, pFullBrush);
		Seg[5].DrawSegment(pRenderTarget, pFullBrush);
		Seg[6].DrawSegment(pRenderTarget, pFullBrush);
	}
}

void DigitalClock::Init(ID2D1Factory* pD2DFactory)
{
	m_SevenSegment.InitGeometry(pD2DFactory);
}

void DigitalClock::Draw(ID2D1HwndRenderTarget* pRenderTarget, D2D1::Matrix3x2F transform, BOOL negative, BOOL Highlighted, BOOL bTenths, INT64 days, INT64 hours, INT64 mins, INT64 tenths)
{
	const float DigitSpacing = 100.0f;
	const float DotSpacing = 40.0f;
	float spacing = 0.0f;
	D2D1::Matrix3x2F DigitTransform = D2D1::Matrix3x2F::Identity();
	ID2D1SolidColorBrush* pBrush = pNormalBrush;
	if (Highlighted)
		pBrush = pHighlightBrush;

	if (negative)
	{
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, -1, pBrush);
		spacing += DigitSpacing;
	}
	// 5 digits of days
	if (days > 9999)
	{
		int digit = (days / 10000) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 999)
	{
		int digit = (days / 1000) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 99)
	{
		int digit = (days / 100) % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 9)
	{
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		int digit = (days / 10) % 10;
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (days > 0)
	{
		int digit = days % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		//DrawColon(pRenderTarget);
		spacing += DigitSpacing;
		// d for Days
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, 'd', pBrush);
		spacing += DigitSpacing + DigitSpacing;
	}
	// Draw Hours
	int clippedhours = hours % 24;
	if (hours > 9)
	{
		int digit = clippedhours / 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	if (hours > 0)
	{
		int digit = clippedhours % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		DrawColon(pRenderTarget, pBrush);
		spacing += DigitSpacing + DotSpacing;
	}
	// Draw Minutes
	int clippedminutes = mins % 60;
	if (mins > 9)
	{
		int digit = clippedminutes / 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
		spacing += DigitSpacing;
	}
	{
		int digit = clippedminutes % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
	}
	// Draw Tenths
	if (bTenths)
	{
		DrawDot(pRenderTarget, pBrush);
		spacing += DigitSpacing + DotSpacing;
		int digit = tenths % 10;
		DigitTransform = D2D1::Matrix3x2F::Translation(spacing, 0.0f);
		pRenderTarget->SetTransform(DigitTransform * transform);
		m_SevenSegment.Draw(pRenderTarget, digit, pBrush);
	}
}

void DigitalClock::DrawDot(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* Brush)
{
	D2D1_ELLIPSE ellipse = { { 70 + -0.2f * 70,  70 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
}

void DigitalClock::DrawColon(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* Brush)
{
	D2D1_ELLIPSE ellipse = { { 70 + -0.2f * 40,  40 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
	ellipse = { { 70 + -0.2f * -40, -40 }, 10, 10 };
	pRenderTarget->FillEllipse(ellipse, Brush);
}
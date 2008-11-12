/////////////////////////////////////////////////////////////////////////////
// Name:	areabackground.cpp
// Purpose:
// Author:	Moskvichev Andrey V.
// Created:	2008/11/07
// RCS-ID:	$Id: wxAdvTable.h,v 1.3 2008/11/07 16:42:58 moskvichev Exp $
// Copyright:	(c) 2008 Moskvichev Andrey V.
// Licence:	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/areabackground.h>

AreaBackground::AreaBackground()
{
}

AreaBackground::~AreaBackground()
{
}

NoAreaBackground::NoAreaBackground()
{
}

NoAreaBackground::~NoAreaBackground()
{
}

void NoAreaBackground::Draw(wxDC &dc, wxRect rc)
{
	// do nothing
}


FillAreaBackground::FillAreaBackground(wxPen borderPen, wxBrush fillBrush)
{
	m_fillBrush = fillBrush;
	m_borderPen = borderPen;
}

FillAreaBackground::~FillAreaBackground()
{
}

void FillAreaBackground::Draw(wxDC &dc, wxRect rc)
{
	dc.SetPen(m_borderPen);
	dc.SetBrush(m_fillBrush);
	dc.DrawRectangle(rc);
}

GradientAreaBackground::GradientAreaBackground(wxPen borderPen, wxColour colour1, wxColour colour2, wxDirection dir)
{
	m_borderPen = borderPen;
	m_colour1 = colour1;
	m_colour2 = colour2;
	m_dir = dir;
}

GradientAreaBackground::~GradientAreaBackground()
{
}

void GradientAreaBackground::Draw(wxDC &dc, wxRect rc)
{
	dc.GradientFillLinear(rc, m_colour1, m_colour2, m_dir);

	dc.SetPen(m_borderPen);
	dc.SetBrush(noBrush);
	dc.DrawRectangle(rc);
}

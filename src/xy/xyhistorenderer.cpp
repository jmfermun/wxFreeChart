
/////////////////////////////////////////////////////////////////////////////
// Name:	xyhistorenderer.cpp
// Purpose:
// Author:	Moskvichev Andrey V.
// Created:	2008/11/07
// RCS-ID:	$Id: wxAdvTable.h,v 1.3 2008/11/07 16:42:58 moskvichev Exp $
// Copyright:	(c) 2008 Moskvichev Andrey V.
// Licence:	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/xy/xyhistorenderer.h>

XYHistoRenderer::XYHistoRenderer(int barWidth, bool vertical)
{
	m_barWidth = barWidth;
	m_vertical = vertical;
	m_serieShift = barWidth + 2; // XXX temporary!

	m_barArea = new FillAreaBackground(*wxBLACK_PEN, *wxRED_BRUSH);
}

XYHistoRenderer::~XYHistoRenderer()
{
	SAFE_DELETE(m_barArea);
}

void XYHistoRenderer::DrawBar(wxDC &dc, wxRect rcData, wxCoord x, wxCoord y)
{
	wxRect rcBar;

	if (m_vertical) {
		rcBar.x = x - m_barWidth / 2;
		rcBar.y = y;
		rcBar.width = m_barWidth;
		rcBar.height = rcData.height - y + rcData.y;
	}
	else {
		rcBar.x = rcData.x;
		rcBar.y = y - m_barWidth / 2;
		rcBar.width = x - rcData.x;
		rcBar.height = m_barWidth;
	}

	m_barArea->Draw(dc, rcBar);
}

void XYHistoRenderer::Draw(wxDC &dc, wxRect rc, Axis *horizAxis, Axis *vertAxis, XYDataset *dataset)
{
	const int serieCount = dataset->GetSerieCount();

	wxCoord xShift = 0;
	wxCoord yShift = 0;

	wxCoord c0 = (m_serieShift - 1) * serieCount / 2 - m_barWidth / 2;
	if (m_vertical) {
		xShift -= c0;
	}
	else {
		yShift -= c0;
	}

	PrepareDC(dc);
	for (int serie = 0; serie < serieCount; serie++) {
		for (int n = 0; n < dataset->GetCount(serie); n++) {
			double xVal;
			double yVal;

			if (m_vertical) {
				xVal = dataset->GetX(n, serie);
				yVal = dataset->GetY(n, serie);
			}
			else {
				xVal = dataset->GetY(n, serie);
				yVal = dataset->GetX(n, serie);
			}

			wxCoord x = horizAxis->ToGraphics(dc, rc.x, rc.width, xVal);
			wxCoord y = vertAxis->ToGraphics(dc, rc.y, rc.height, yVal);

			x += xShift;
			y += yShift;

			DrawBar(dc, rc, x, y);
		}

		if (m_vertical) {
			xShift += m_serieShift;
		}
		else {
			yShift += m_serieShift;
		}
	}
}

void XYHistoRenderer::Draw(wxDC &dc, wxRect rcData, wxCoord x0, wxCoord y0, wxCoord x1, wxCoord y1)
{
}

void XYHistoRenderer::DrawLegendSymbol(wxDC &dc, wxCoord x0, wxCoord y0, wxCoord x1, wxCoord y1)
{
// TODO
	//dc.SetPen(symbolPen);
	dc.DrawLine(x0, (y0 + y1) / 2, x1, (y0 + y1) / 2);
}
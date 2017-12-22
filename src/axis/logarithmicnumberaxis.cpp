/////////////////////////////////////////////////////////////////////////////
// Name:	logarithmicnumberaxis.cpp
// Purpose: label axis implementation
// Author:	Andreas Kuechler
// Created:	2008/11/07
// Copyright:	(c) 2010 Andreas Kuechler
// Licence:	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/axis/logarithmicnumberaxis.h>
#include <wx/xy/xydataset.h>
#include <math.h>

IMPLEMENT_CLASS(LogarithmicNumberAxis, NumberAxis);

LogarithmicNumberAxis::LogarithmicNumberAxis(AXIS_LOCATION location)
: NumberAxis(location)
, m_longExponent(false)
, m_logBase(10.0)
{
	m_logBase == 10.0 ? SetTickFormat(wxT("%2.2e")) : SetTickFormat(wxT("%2.2f"));
	m_subticks = m_logBase - 1;
}

LogarithmicNumberAxis::~LogarithmicNumberAxis()
{
}

bool LogarithmicNumberAxis::AcceptDataset(Dataset *dataset)
{
    // This axis only accepts datasets that contain values > 0
    if (dataset->GetMinValue(IsVertical()) > 0)
        return true;
    else
        return false;
}

void LogarithmicNumberAxis::AutomaticTicksCalc()
{
    // This algorithm ensures that there will be at least 2 ticks
	m_hasLabels = true;

    m_maxValue = pow(m_logBase, ceil(log(m_maxValue)/log(m_logBase)));
    m_minValue = pow(m_logBase, floor(log(m_minValue)/log(m_logBase)));

    // We need some degree of separation here to calculate the scales if all the values are the same.
    // Giving m_maxValue an order of magnitud greather than m_minValue is sufficient.
    if (m_maxValue == m_minValue)
        m_maxValue = m_minValue * m_logBase;

    float maxOrderOfMagnitud = log(m_maxValue)/log(m_logBase);
    float minOrderOfMagnitud = log(m_minValue)/log(m_logBase);

    m_labelCount = maxOrderOfMagnitud - minOrderOfMagnitud + 1;

    if (m_enableSubticks)
        m_labelCount = (m_labelCount - 1) * (m_logBase - 1) + 1;
}

void LogarithmicNumberAxis::SetLogBase(double logBase)
{
	m_logBase = logBase;
	m_logBase == 10.0 ? SetTickFormat(wxT("%2.2e")) : SetTickFormat(wxT("%2.2f"));
	m_subticks = m_logBase - 1;
}

double LogarithmicNumberAxis::GetLogBase()
{
    return m_logBase;
}

void LogarithmicNumberAxis::EnableLongLabelExponent(bool enable)
{
	m_longExponent = enable;
}

double LogarithmicNumberAxis::GetValue(size_t step)
{
	double minValue, maxValue, returnValue;

	GetDataBounds(minValue, maxValue);

	if (m_fixedBounds)
    {
        double logMin = log(minValue) / log(m_logBase);
        double logMax = log(maxValue) / log(m_logBase);
        double logInterval = (logMax - logMin) / (GetLabelCount() - 1);

        returnValue = minValue * pow(m_logBase, step * logInterval);
    }
    else
    {
        if (m_enableSubticks)
        {
            returnValue = minValue * pow(m_logBase, (int)((step/(m_logBase-1))));
            returnValue += returnValue * (step % (int)(m_logBase-1));
        }
        else
            returnValue = minValue * pow(m_logBase, step);
    }


	return returnValue;
}

void LogarithmicNumberAxis::GetLabel(size_t step, wxString& label)
{
	NumberAxis::GetLabel(step, label);

	if (m_logBase == 10.0 && !m_longExponent) {
		label.erase(label.length() - 3, 1);
	}
}

wxCoord LogarithmicNumberAxis::ToGraphics(wxDC &WXUNUSED(dc), int minCoord, int gRange, double value)
{
	double minValue, maxValue;
	GetDataBounds(minValue, maxValue);

	minCoord += m_marginMin;
	gRange -= (m_marginMin + m_marginMax);

	if (gRange < 0) {
		gRange = 0;
	}

	if (m_useWin) {
		minValue = m_winPos;
		maxValue = m_winPos + m_winWidth;
	}

	double logValue = log(value) / log(m_logBase);
	double logMax = log(maxValue) / log(m_logBase);
	double logMin = log(minValue) / log(m_logBase);

	return ::ToGraphics(minCoord, gRange, logMin, logMax, 0/*textMargin*/, IsVertical(), logValue);
}

double LogarithmicNumberAxis::ToData(wxDC &WXUNUSED(dc), int minCoord, int gRange, wxCoord g)
{
	double minValue, maxValue;
	GetDataBounds(minValue, maxValue);

	minCoord += m_marginMin;
	gRange -= (m_marginMin + m_marginMax);
	if (gRange < 0) {
		gRange = 0;
	}

	if (m_useWin) {
		minValue = m_winPos;
		maxValue = m_winPos + m_winWidth;
	}

	double logMin = log(minValue) / log(m_logBase);
	double logMax = log(minValue) / log(m_logBase);
	return ::ToData(minCoord, gRange, logMin, logMax, 0/*textMargin*/, IsVertical(), g);
}

double LogarithmicNumberAxis::BoundValue(double value)
{
//  double v = abs((double)log10(value));
	if (m_useWin) {
		if (value <= m_winPos) {
			return m_winPos;
		}
		else if (value >= (m_winPos + m_winWidth)) {
			return m_winPos + m_winWidth;
		}
		else {
			return value;
		}
	}
	else {
		return value;
	}
}

bool LogarithmicNumberAxis::IsVisible(double value)
{
  if(value == 0.0) {
    return false;
  }
  else {
    if (m_useWin) {
		return (value >= m_winPos && value <= (m_winPos + m_winWidth));
    }
    else {
      double minValue, maxValue;
      GetDataBounds(minValue, maxValue);

      return (value >= minValue && value <= maxValue);
    }
  }
}

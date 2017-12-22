/////////////////////////////////////////////////////////////////////////////
// Name:	numberaxis.cpp
// Purpose: number axis implementation
// Author:	Moskvichev Andrey V.
// Created:	2008/11/07
// Copyright:	(c) 2008-2010 Moskvichev Andrey V.
// Licence:	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/axis/numberaxis.h>
#include <wx/xy/xydataset.h>
#include <wx/category/categorydataset.h>

#ifdef WIN32
#include <float.h>

bool IsNormalValue(double v)
{
	switch (_fpclass(v)) {
		case _FPCLASS_SNAN:
		case _FPCLASS_QNAN:
		case _FPCLASS_NINF:
		case _FPCLASS_PINF:
			return false;
		default:
			return true;
	}
}
#else
#include <math.h>

bool IsNormalValue(double v)
{
	switch (fpclassify(v)) {
		case FP_NAN:
		case FP_INFINITE:
		case FP_SUBNORMAL:
			return false;
		default:
			return true;
	}
}

#endif

IMPLEMENT_CLASS(NumberAxis, Axis)

NumberAxis::NumberAxis(AXIS_LOCATION location)
: LabelAxis(location)
{
	// default values
	m_tickFormat = wxT("%.2f");

	m_minValue = 0;
	m_maxValue = 100;

	m_labelInterval = 10;
	m_labelCount = 5;

	m_intValues = false;
	m_hasLabels = false;
	m_fixedBounds = false;

	m_multiplier = 1;

	m_subticks = 10;
}

NumberAxis::~NumberAxis()
{
}

void NumberAxis::SetTickFormat(const wxString &tickFormat)
{
    m_tickFormat = tickFormat;
    FireAxisChanged();
}

void NumberAxis::SetLabelCount(size_t labelCount)
{
    if (m_fixedBounds && m_labelCount != labelCount)
    {
        m_labelCount = labelCount;

        FixedTicksCalc();

        FireAxisChanged();
    }
}

void NumberAxis::IntegerValues(bool intValues)
{
    if (m_intValues != intValues) {
        m_intValues = intValues;
        // TODO recalc tick steps
        FireAxisChanged();
    }
}

bool NumberAxis::AcceptDataset(Dataset *WXUNUSED(dataset))
{
	return true;
}

void NumberAxis::SetFixedBounds(double minValue, double maxValue)
{
	m_minValue = minValue;
	m_maxValue = maxValue;
	m_fixedBounds = true;

	FixedTicksCalc();

	FireBoundsChanged();
}

void NumberAxis::UpdateBounds()
{
    if (m_fixedBounds)
    {
        FixedTicksCalc();
    }
    else
    {
        // Obtain the minimum and maximum values from the linked datasets to this axis
        for (size_t n = 0; n < m_datasets.Count(); n++)
        {
            bool verticalAxis = IsVertical();

            double minValue = m_datasets[n]->GetMinValue(verticalAxis);
            double maxValue = m_datasets[n]->GetMaxValue(verticalAxis);

            if (n == 0)
            {
                m_minValue = minValue;
                m_maxValue = maxValue;
            }
            else
            {
                m_minValue = wxMin(m_minValue, minValue);
                m_maxValue = wxMax(m_maxValue, maxValue);
            }
        }

        AutomaticTicksCalc();
    }

    FireBoundsChanged();
}

void NumberAxis::FixedTicksCalc()
{
	m_hasLabels = false;
	m_labelInterval = (m_maxValue - m_minValue) / (double)(m_labelCount - 1);

	if (!IsNormalValue(m_labelInterval)) {
		// overflow condition bugfix
		m_minValue = 0;
		m_maxValue = 0;
		m_labelInterval = 0;
	}
	else {
		if (ABS(m_maxValue - m_minValue) > 0.000000001) {
			m_hasLabels = true;
		}
	}
}

void NumberAxis::AutomaticTicksCalc()
{
    // This algorithm ensures that there will be at least 2 ticks
	m_hasLabels = true;

    // We need some degree of separation here to calculate the scales if all the values are the same.
    // Adding/subtracting 0.5 will give us a range of 1.
	if (m_maxValue == m_minValue)
	{
		m_maxValue += 0.5;
		m_minValue -= 0.5;
	}

    double valueRange = m_maxValue - m_minValue;

    if (valueRange < 0)
        valueRange = -valueRange;

    double rangeOrderOfMagnitude = floor(log10(valueRange));

    m_maxValue = ceil(m_maxValue / (1 * pow(10, rangeOrderOfMagnitude))) * pow(10, rangeOrderOfMagnitude);
    m_minValue = floor(m_minValue / (1 * pow(10, rangeOrderOfMagnitude))) * pow(10, rangeOrderOfMagnitude);

    valueRange = m_maxValue - m_minValue;

    m_labelInterval = pow(10, rangeOrderOfMagnitude);
    m_labelCount = round(valueRange / m_labelInterval) + 1;

    if (m_enableSubticks)
    {
        m_labelInterval /= 10;
        m_labelCount = (m_labelCount - 1) * 10 + 1;
    }
}

wxSize NumberAxis::GetLongestLabelExtent(wxDC &dc)
{
	dc.SetFont(GetLabelTextFont());

	wxSize sizeMinValue = dc.GetTextExtent(wxString::Format(m_tickFormat, m_minValue));
	wxSize sizeMaxValue = dc.GetTextExtent(wxString::Format(m_tickFormat, m_maxValue));

	if (sizeMinValue.x > sizeMaxValue.x) {
		return sizeMinValue;
	}
	else {
		return sizeMaxValue;
	}
}

void NumberAxis::GetDataBounds(double &minValue, double &maxValue) const
{
	minValue = m_minValue;
	maxValue = m_maxValue;
}

double NumberAxis::GetValue(size_t step)
{
	return m_minValue + step * m_labelInterval;
}

void NumberAxis::GetLabel(size_t step, wxString &label)
{
	double value = GetValue(step);

	if (value == -0) {
		value = 0;
	}

	if (m_intValues) {
		// orig : label = wxString::Format(wxT("%i"), (int) value);
		label = wxString::Format(wxT("%i"), (int)(value * m_multiplier));
	}
	else {
		// orig : label = wxString::Format(m_tickFormat, value);
		label = wxString::Format(m_tickFormat, value * m_multiplier);
	}
}

bool NumberAxis::IsEnd(size_t step)
{
	return step >= m_labelCount;
}

bool NumberAxis::HasLabels()
{
	return m_hasLabels;
}

size_t NumberAxis::GetLabelCount() const
{
  return m_labelCount;
}

double NumberAxis::GetMultiplier() const
{
  return m_multiplier;
}

void NumberAxis::SetMultiplier(double multiplier)
{
  m_multiplier = multiplier;
}

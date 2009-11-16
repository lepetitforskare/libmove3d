/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "SpinBoxSliderConnector_p.hpp"

using namespace QtShiva;

SpinBoxSliderConnector::SpinBoxSliderConnector( QObject* _parent,
                                                QDoubleSpinBox* _spinBox,
                                                QSlider* _slider,
                                                Env::doubleParameter p) :
        QObject( _parent ), m_spinBox( _spinBox ), m_slider( _slider )
{
  connect( m_spinBox, SIGNAL(valueChanged( double )), SLOT(spinBoxValueChanged( double ) ) );
  connect( m_slider, SIGNAL(valueChanged( int )), SLOT(sliderValueChanged( int ) ) );

  connect(this, SIGNAL(valueChanged( double )), ENV.getObject(p),SLOT(set(double)), Qt::DirectConnection);
  connect(ENV.getObject(p), SIGNAL(valueChanged(double)), this, SLOT(spinBoxValueChanged( double )), Qt::DirectConnection);

  m_spinBox->setValue(ENV.getDouble(p));

}

SpinBoxSliderConnector::~SpinBoxSliderConnector()
{
}

double SpinBoxSliderConnector::value() const
{
  return m_spinBox->value();
}

void SpinBoxSliderConnector::setValue( double _value )
{
  m_spinBox->setValue( _value );
}

void SpinBoxSliderConnector::spinBoxValueChanged( double _value )
{
  bool v = m_slider->blockSignals(true);
  m_slider->setValue( int(_value * ( m_slider->maximum() - m_slider->minimum() ) / ( m_spinBox->maximum() - m_spinBox->minimum() ) ) );
  m_slider->blockSignals(v);
  emit( valueChanged( m_spinBox->value() ) );
}

void SpinBoxSliderConnector::sliderValueChanged( int _value )
{
  bool v = m_spinBox->blockSignals(true);
  m_spinBox->setValue( _value * ( m_spinBox->maximum() - m_spinBox->minimum() ) / ( m_slider->maximum() - m_slider->minimum() ) );
  m_spinBox->blockSignals(v);
  emit( valueChanged( m_spinBox->value() ) );
}

#include "moc_SpinBoxSliderConnector_p.cpp"

//##################################################################################################
//
//   Custom Visualization Core library
//   Copyright (C) 2011-2012 Ceetron AS
//    
//   This library is free software: you can redistribute it and/or modify 
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation, either version 3 of the License, or 
//   (at your option) any later version. 
//    
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY 
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//   FITNESS FOR A PARTICULAR PURPOSE.   
//    
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>> 
//   for more details. 
//
//##################################################################################################

#include "cafPdmUiSliderEditor.h"

#include "cafPdmUiDefaultObjectEditor.h"
#include "cafPdmObject.h"
#include "cafPdmUiFieldEditorHandle.h"
#include "cafPdmField.h"

#include "cafFactory.h"

#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QIntValidator>

#include <assert.h>


namespace caf
{

CAF_PDM_UI_FIELD_EDITOR_SOURCE_INIT(PdmUiSliderEditor);

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PdmUiSliderEditor::configureAndUpdateUi(const QString& uiConfigName)
{
    assert(!m_lineEdit.isNull());

    QIcon ic = field()->uiIcon(uiConfigName);
    if (!ic.isNull())
    {
        m_label->setPixmap(ic.pixmap(ic.actualSize(QSize(64, 64))));
    }
    else
    {
        m_label->setText(field()->uiName(uiConfigName));
    }

    m_label->setVisible(!field()->isUiHidden(uiConfigName));
    m_label->setEnabled(!field()->isUiReadOnly(uiConfigName));

    m_lineEdit->setEnabled(!field()->isUiReadOnly(uiConfigName));
    m_slider->setEnabled(!field()->isUiReadOnly(uiConfigName));

    field()->ownerObject()->editorAttribute(field(), uiConfigName, &m_attributes);
    m_lineEdit->setValidator(new QIntValidator(m_attributes.m_minimum, m_attributes.m_maximum, this));

    {
        m_slider->blockSignals(true);
        m_slider->setRange(m_attributes.m_minimum, m_attributes.m_maximum);
        m_slider->blockSignals(false);
    }

    QString textValue = field()->uiValue().toString();
    m_lineEdit->setText(textValue);
    updateSliderPosition();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QWidget* PdmUiSliderEditor::createEditorWidget(QWidget * parent)
{
    QWidget* containerWidget = new QWidget(parent);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    containerWidget->setLayout(layout);

    m_lineEdit = new QLineEdit(containerWidget);
    m_lineEdit->setMaximumWidth(30);
    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));

    m_slider = new QSlider(Qt::Horizontal, containerWidget);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_slider);

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderValueChanged(int)));
    
    return containerWidget;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QWidget* PdmUiSliderEditor::createLabelWidget(QWidget * parent)
{
    m_label = new QLabel(parent);
    return m_label;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PdmUiSliderEditor::slotEditingFinished()
{
    updateSliderPosition();

    writeValueToField();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PdmUiSliderEditor::slotSliderValueChanged(int position)
{
    m_lineEdit->setText(QString::number(position));

    writeValueToField();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PdmUiSliderEditor::updateSliderPosition()
{
    QString textValue = m_lineEdit->text();

    bool convertOk = false;
    int newSliderValue = textValue.toInt(&convertOk);
    if (convertOk)
    {
        newSliderValue = qBound(m_attributes.m_minimum, newSliderValue, m_attributes.m_maximum);
        m_slider->setValue(newSliderValue);
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PdmUiSliderEditor::writeValueToField()
{
    QString textValue = m_lineEdit->text();
    QVariant v;
    v = textValue;
    this->setValueToField(v);
}


} // end namespace caf

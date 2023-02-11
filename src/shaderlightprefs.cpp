#include "shaderlightprefs.h"
#include "canvas.h"
#include <QColorDialog>

const QString ShaderLightPrefs::PREFS_GEOM = "shaderPrefsGeometry";

ShaderLightPrefs::ShaderLightPrefs(QWidget *parent, Canvas *_canvas) : QDialog(parent)
{
    canvas = _canvas;

    QVBoxLayout* prefsLayout = new QVBoxLayout;
    this->setLayout(prefsLayout);

    QLabel* title = new QLabel("Shader preferences");
    QFont boldFont = QApplication::font();
    boldFont.setWeight(QFont::Bold);
    title->setFont(boldFont);
    title->setAlignment(Qt::AlignCenter);
    prefsLayout->addWidget(title);

    QWidget* middleWidget = new QWidget;
    QGridLayout* middleLayout = new QGridLayout;
    middleWidget->setLayout(middleLayout);
    this->layout()->addWidget(middleWidget);

    // labels
    middleLayout->addWidget(new QLabel("Ambient Color"),0,0);
    middleLayout->addWidget(new QLabel("Directive Color"),1,0);
    middleLayout->addWidget(new QLabel("Direction"),2,0);

    QPixmap dummy(20, 20);

    dummy.fill(canvas->getAmbientColor());
    buttonAmbientColor = new QPushButton;
    buttonAmbientColor->setIcon(QIcon(dummy));
    middleLayout->addWidget(buttonAmbientColor,0,1);
    buttonAmbientColor->setFocusPolicy(Qt::NoFocus);
    connect(buttonAmbientColor,SIGNAL(clicked(bool)),this,SLOT(buttonAmbientColorClicked()));

    editAmbientFactor = new QLineEdit;
    editAmbientFactor->setValidator(new QDoubleValidator);
    editAmbientFactor->setText(QString("%1").arg(canvas->getAmbientFactor()));
    middleLayout->addWidget(editAmbientFactor,0,2);
    connect(editAmbientFactor,SIGNAL(editingFinished()),this,SLOT(editAmbientFactorFinished()));

    QPushButton* buttonResetAmbientColor = new QPushButton("Reset");
    middleLayout->addWidget(buttonResetAmbientColor,0,3);
    buttonResetAmbientColor->setFocusPolicy(Qt::NoFocus);
    connect(buttonResetAmbientColor,SIGNAL(clicked(bool)),this,SLOT(resetAmbientColorClicked()));


    dummy.fill(canvas->getDirectiveColor());
    buttonDirectiveColor = new QPushButton;
    buttonDirectiveColor->setIcon(QIcon(dummy));
    middleLayout->addWidget(buttonDirectiveColor,1,1);
    buttonDirectiveColor->setFocusPolicy(Qt::NoFocus);
    connect(buttonDirectiveColor,SIGNAL(clicked(bool)),this,SLOT(buttonDirectiveColorClicked()));

    editDirectiveFactor = new QLineEdit;
    editDirectiveFactor->setValidator(new QDoubleValidator);
    editDirectiveFactor->setText(QString("%1").arg(canvas->getDirectiveFactor()));
    middleLayout->addWidget(editDirectiveFactor,1,2);
    connect(editDirectiveFactor,SIGNAL(editingFinished()),this,SLOT(editDirectiveFactorFinished()));

    QPushButton* buttonResetDirectiveColor = new QPushButton("Reset");
    middleLayout->addWidget(buttonResetDirectiveColor,1,3);
    buttonResetDirectiveColor->setFocusPolicy(Qt::NoFocus);
    connect(buttonResetDirectiveColor,SIGNAL(clicked(bool)),this,SLOT(resetDirectiveColorClicked()));

    // Fill in directions

    comboDirections = new QComboBox;
    comboDirections->setFocusPolicy(Qt::NoFocus);
    middleLayout->addWidget(comboDirections,2,1,1,2);
    comboDirections->addItems(canvas->getNameDir());
    comboDirections->setCurrentIndex(canvas->getCurrentLightDirection());
    connect(comboDirections,SIGNAL(currentIndexChanged(int)),this,SLOT(comboDirectionsChanged(int)));

    QPushButton* buttonResetDirection = new QPushButton("Reset");
    middleLayout->addWidget(buttonResetDirection,2,3);
    buttonResetDirection->setFocusPolicy(Qt::NoFocus);
    connect(buttonResetDirection,SIGNAL(clicked(bool)),this,SLOT(resetDirection()));

    checkboxUseWireFrame = new QCheckBox("Add wireframe");
    checkboxUseWireFrame->setChecked(canvas->getUseWire());
    middleLayout->addWidget(checkboxUseWireFrame,3,0);
    checkboxUseWireFrame->setFocusPolicy(Qt::NoFocus);
    connect(checkboxUseWireFrame,SIGNAL(stateChanged(int)),this,SLOT(checkboxUseWireFrameChanged()));

    QLabel* labelWireColor = new QLabel("Wire Color");
    middleLayout->addWidget(labelWireColor,4,0);
    dummy.fill(canvas->getWireColor());
    buttonWireColor = new QPushButton;
    buttonWireColor->setIcon(QIcon(dummy));
    middleLayout->addWidget(buttonWireColor,4,1);
    buttonWireColor->setFocusPolicy(Qt::NoFocus);
    QPushButton* buttonResetWireColor = new QPushButton("Reset");
    buttonResetWireColor->setFocusPolicy(Qt::NoFocus);
    middleLayout->addWidget(buttonResetWireColor,4,3);
    connect(buttonWireColor,SIGNAL(clicked(bool)),this,SLOT(buttonWireColorClicked()));
    connect(buttonResetWireColor,SIGNAL(clicked(bool)),this,SLOT(resetWireColorClicked()));

    labelWireWidth = new QLabel(QString("Wire Width : %1").arg((int)canvas->getWireWidth()));
    middleLayout->addWidget(labelWireWidth,5,0);
    sliderWireWidth = new QSlider(Qt::Horizontal);
    sliderWireWidth->setFocusPolicy(Qt::NoFocus);
    sliderWireWidth->setRange(1,10);
    sliderWireWidth->setTickPosition(QSlider::TicksBelow);
    sliderWireWidth->setSingleStep(1);
    sliderWireWidth->setPageStep(1);
    sliderWireWidth->setValue((int)canvas->getWireWidth());
    middleLayout->addWidget(sliderWireWidth,5,1,1,2);
    connect(sliderWireWidth,SIGNAL(valueChanged(int)),this,SLOT(sliderWireWidthChanged()));
    QPushButton* buttonResetLineWidth = new QPushButton("Reset");
    buttonResetLineWidth->setFocusPolicy(Qt::NoFocus);
    middleLayout->addWidget(buttonResetLineWidth,5,3);
    connect(buttonResetLineWidth,SIGNAL(clicked(bool)),this,SLOT(resetWireWidthClicked()));


    // Ok button
    QWidget* boxButton = new QWidget;
    QHBoxLayout* boxButtonLayout = new QHBoxLayout;
    boxButton->setLayout(boxButtonLayout);
    QFrame *spacerL = new QFrame;
    spacerL->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));
    QPushButton* okButton = new QPushButton("Ok");
    boxButtonLayout->addWidget(spacerL);
    boxButtonLayout->addWidget(okButton);
    this->layout()->addWidget(boxButton);
    okButton->setFocusPolicy(Qt::NoFocus);
    connect(okButton,SIGNAL(clicked(bool)),this,SLOT(okButtonClicked()));

    QSettings settings;
    if (!settings.value(PREFS_GEOM).isNull()) {
        restoreGeometry(settings.value(PREFS_GEOM).toByteArray());
    }
}

void ShaderLightPrefs::buttonAmbientColorClicked() {
    QColor newColor = QColorDialog::getColor(canvas->getAmbientColor(), this, QString("Choose color"),QColorDialog::DontUseNativeDialog);
    if (newColor.isValid() == true)
    {
        canvas->setAmbientColor(newColor);
        QPixmap dummy(20, 20);
        dummy.fill(canvas->getAmbientColor());
        buttonAmbientColor->setIcon(QIcon(dummy));
        canvas->update();
    }
}

void ShaderLightPrefs::editAmbientFactorFinished() {
    canvas->setAmbientFactor(editAmbientFactor->text().toDouble());
    canvas->update();
}

void ShaderLightPrefs::resetAmbientColorClicked() {
    canvas->resetAmbientColor();
    QPixmap dummy(20, 20);
    dummy.fill(canvas->getAmbientColor());
    buttonAmbientColor->setIcon(QIcon(dummy));
    editAmbientFactor->setText(QString("%1").arg(canvas->getAmbientFactor()));
    canvas->update();
}

void ShaderLightPrefs::buttonDirectiveColorClicked() {
    QColor newColor = QColorDialog::getColor(canvas->getDirectiveColor(), this, QString("Choose color"),QColorDialog::DontUseNativeDialog);
    if (newColor.isValid() == true)
    {
        canvas->setDirectiveColor(newColor);
        QPixmap dummy(20, 20);
        dummy.fill(canvas->getDirectiveColor());
        buttonDirectiveColor->setIcon(QIcon(dummy));
        canvas->update();
    }
}

void ShaderLightPrefs::editDirectiveFactorFinished() {
    canvas->setDirectiveFactor(editDirectiveFactor->text().toDouble());
    canvas->update();
}

void ShaderLightPrefs::resetDirectiveColorClicked() {
    canvas->resetDirectiveColor();
    QPixmap dummy(20, 20);
    dummy.fill(canvas->getDirectiveColor());
    buttonDirectiveColor->setIcon(QIcon(dummy));
    editDirectiveFactor->setText(QString("%1").arg(canvas->getDirectiveFactor()));
    canvas->update();
}

void ShaderLightPrefs::okButtonClicked() {
    this->close();
}

void ShaderLightPrefs::comboDirectionsChanged(int ind) {
    canvas->setCurrentLightDirection(ind);
    canvas->update();
}

void ShaderLightPrefs::resetDirection() {
    canvas->resetCurrentLightDirection();
    comboDirections->setCurrentIndex(canvas->getCurrentLightDirection());
    canvas->update();
}

void ShaderLightPrefs::resizeEvent(QResizeEvent *event)
{
    QSettings().setValue(PREFS_GEOM, saveGeometry());
}

void ShaderLightPrefs::moveEvent(QMoveEvent *event)
{
    QSettings().setValue(PREFS_GEOM, saveGeometry());
}

void ShaderLightPrefs::checkboxUseWireFrameChanged() {
    bool state = checkboxUseWireFrame->isChecked();
    canvas->setUseWire(state);
    canvas->update();
}

void ShaderLightPrefs::buttonWireColorClicked() {
    QColor newColor = QColorDialog::getColor(canvas->getWireColor(), this, QString("Choose color"),QColorDialog::DontUseNativeDialog);
    if (newColor.isValid() == true)
    {
        canvas->setWireColor(newColor);
        QPixmap dummy(20, 20);
        dummy.fill(canvas->getWireColor());
        buttonWireColor->setIcon(QIcon(dummy));
        canvas->update();
    }
}

void ShaderLightPrefs::resetWireColorClicked() {
    canvas->resetWireColor();
    QPixmap dummy(20, 20);
    dummy.fill(canvas->getWireColor());
    buttonWireColor->setIcon(QIcon(dummy));
    canvas->update();
}

void ShaderLightPrefs::sliderWireWidthChanged() {
    int lw = sliderWireWidth->value();
    canvas->setWireWidth((double) lw);
    labelWireWidth->setText(QString("Wire Width : %1").arg(lw));
    canvas->update();
}

void ShaderLightPrefs::resetWireWidthClicked() {
    canvas->resetWireWidth();
    sliderWireWidth->setValue((int)canvas->getWireWidth());
}

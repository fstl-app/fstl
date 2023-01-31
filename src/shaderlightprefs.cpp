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
    middleLayout->addWidget(comboDirections,2,1,1,2);
    comboDirections->addItems(canvas->getNameDir());
    comboDirections->setCurrentIndex(canvas->getCurrentLightDirection());
    connect(comboDirections,SIGNAL(currentIndexChanged(int)),this,SLOT(comboDirectionsChanged(int)));

    QPushButton* buttonResetDirection = new QPushButton("Reset");
    middleLayout->addWidget(buttonResetDirection,2,3);
    buttonResetDirection->setFocusPolicy(Qt::NoFocus);
    connect(buttonResetDirection,SIGNAL(clicked(bool)),this,SLOT(resetDirection()));


    // Hide button
    QPushButton* hideButton = new QPushButton("Hide");
    this->layout()->addWidget(hideButton);
    hideButton->setFocusPolicy(Qt::NoFocus);
    connect(hideButton,SIGNAL(clicked(bool)),this,SLOT(hideButtonClicked()));

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

void ShaderLightPrefs::hideButtonClicked() {
    this->hide();
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



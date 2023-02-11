#ifndef SHADERLIGHTPREFS_H
#define SHADERLIGHTPREFS_H

#include <QDialog>

class Canvas;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSlider;

class ShaderLightPrefs : public QDialog
{
    Q_OBJECT
public:
    ShaderLightPrefs(QWidget* parent, Canvas* _canvas);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

private slots:
    void buttonAmbientColorClicked();
    void editAmbientFactorFinished();
    void resetAmbientColorClicked();

    void buttonDirectiveColorClicked();
    void editDirectiveFactorFinished();
    void resetDirectiveColorClicked();

    void comboDirectionsChanged(int ind);
    void resetDirection();

    void checkboxUseWireFrameChanged();
    void buttonWireColorClicked();
    void resetWireColorClicked();
    void sliderWireWidthChanged();
    void resetWireWidthClicked();

    void okButtonClicked();

private:
    Canvas* canvas;
    QPushButton* buttonAmbientColor;
    QLineEdit* editAmbientFactor;
    QPushButton* buttonDirectiveColor;
    QLineEdit* editDirectiveFactor;
    QComboBox* comboDirections;

    QCheckBox* checkboxUseWireFrame;
    QPushButton* buttonWireColor;
    QLabel* labelWireWidth;
    QSlider* sliderWireWidth;

    const static QString PREFS_GEOM;
};

#endif // SHADERLIGHTPREFS_H

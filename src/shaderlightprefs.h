#ifndef SHADERLIGHTPREFS_H
#define SHADERLIGHTPREFS_H

#include <QDialog>

class Canvas;
class QLabel;
class QLineEdit;
class QComboBox;

class ShaderLightPrefs : public QDialog
{
    Q_OBJECT
public:
    ShaderLightPrefs(QWidget* parent, Canvas* _canvas);

private slots:
    void buttonAmbientColorClicked();
    void editAmbientFactorFinished();
    void resetAmbientColorClicked();

    void buttonDirectiveColorClicked();
    void editDirectiveFactorFinished();
    void resetDirectiveColorClicked();

    void comboDirectionsChanged(int ind);
    void resetDirection();

    void hideButtonClicked();

private:
    Canvas* canvas;
    QPushButton* buttonAmbientColor;
    QLineEdit* editAmbientFactor;
    QPushButton* buttonDirectiveColor;
    QLineEdit* editDirectiveFactor;
    QComboBox* comboDirections;
};

#endif // SHADERLIGHTPREFS_H

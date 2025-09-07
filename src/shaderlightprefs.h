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

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;

private slots:
    void buttonAmbientColorClicked();
    void editAmbientFactorFinished();
    void resetAmbientColorClicked();

    void buttonDirectiveColorClicked();
    void editDirectiveFactorFinished();
    void resetDirectiveColorClicked();

    void comboDirectionsChanged(int ind);
    void resetDirection();

    void okButtonClicked();

private:
    Canvas* canvas;
    QPushButton* buttonAmbientColor;
    QLineEdit* editAmbientFactor;
    QPushButton* buttonDirectiveColor;
    QLineEdit* editDirectiveFactor;
    QComboBox* comboDirections;

    const static QString PREFS_GEOM;
};

#endif // SHADERLIGHTPREFS_H

#ifndef ENVIRONMENTWIDGET_H
#define ENVIRONMENTWIDGET_H

#include <QWidget>
#include "simulator/environment.h"

class EnvironmentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EnvironmentWidget(QWidget *parent = 0);
    void initEnvironment(std::map<std::string, std::string> s);
    void setEnvironment(Environment* env);
    Environment* getEnvironment();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

    std::string readAndSetGenomeFrom(int value, QString path);
    std::map<std::string, std::string> readSettingsFrom(QString path);
    void setEnvironmentSettings(std::map<std::string, std::string> s);
    void resetEnvironment();
    bool evolution = false;
    bool replay = false;
    void setLogging();
    void saveTrace();
signals:
    void initEnvWithSettingsFromUi();

public slots:
    void doStep();
private:
    Environment* env;
    QTransform _transform;

};

#endif // ENVIRONMENTWIDGET_H

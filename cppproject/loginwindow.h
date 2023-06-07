#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QObject>
#include <QWidget>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);

    // Declare any additional member functions or signals/slots here

private:
         // Declare any private member variables or helper functions here
};

#endif // LOGINWINDOW_H

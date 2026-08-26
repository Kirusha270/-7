#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onLoadPhoto();
    void onRegister();

private:

    QLineEdit *m_nameEdit;
    QLineEdit *m_surnameEdit;
    QLineEdit *m_nickEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_idEdit;
    QComboBox *m_serverCombo;
    QPushButton *m_loadPhotoBtn;
    QPushButton *m_registerBtn;
    QLabel *m_photoLabel;

    QString m_photoPath;

    bool validateFields(QString &error);
    bool isNicknameTaken(const QString &nick);
    QString encryptPassword(const QString &password);
    bool saveUserData(const QString &nick,
                      const QString &name,
                      const QString &surname,
                      const QString &email,
                      const QString &idKey,
                      const QString &encryptedPass,
                      const QString &photoSource);
    void copyDefaultPhoto(const QString &destDir);
};

#endif // MAINWINDOW_H

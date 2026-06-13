#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include "config.hpp"

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(ConfigManager *configManager, QWidget *parent = nullptr);

private slots:
    void saveSettings();

private:
    void initUi();

    ConfigManager *config;
    QLineEdit *geminiKeyEdit;
    QLineEdit *openaiKeyEdit;
    QLineEdit *ollamaEndpointEdit;
    QTextEdit *systemPromptEdit;
    
    QPushButton *btnSave;
    QPushButton *btnCancel;
};

#endif 

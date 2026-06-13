#include "settingsdialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

SettingsDialog::SettingsDialog(ConfigManager *configManager, QWidget *parent)
    : QDialog(parent), config(configManager)
{
    initUi();
}

void SettingsDialog::initUi() {
    setWindowTitle("AI Credentials & Styling Config");
    resize(650, 500);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    
    QGroupBox *geminiGroup = new QGroupBox("Google Gemini Settings", this);
    QVBoxLayout *geminiLayout = new QVBoxLayout(geminiGroup);
    geminiLayout->addWidget(new QLabel("Gemini API Key:", this));
    geminiKeyEdit = new QLineEdit(this);
    geminiKeyEdit->setEchoMode(QLineEdit::Password);
    geminiKeyEdit->setText(config->getGeminiKey());
    geminiLayout->addWidget(geminiKeyEdit);
    mainLayout->addWidget(geminiGroup);

    
    QGroupBox *openaiGroup = new QGroupBox("OpenAI Settings", this);
    QVBoxLayout *openaiLayout = new QVBoxLayout(openaiGroup);
    openaiLayout->addWidget(new QLabel("OpenAI API Key:", this));
    openaiKeyEdit = new QLineEdit(this);
    openaiKeyEdit->setEchoMode(QLineEdit::Password);
    openaiKeyEdit->setText(config->getOpenAIKey());
    openaiLayout->addWidget(openaiKeyEdit);
    mainLayout->addWidget(openaiGroup);

    
    QGroupBox *ollamaGroup = new QGroupBox("Ollama (Local) Settings", this);
    QVBoxLayout *ollamaLayout = new QVBoxLayout(ollamaGroup);
    ollamaLayout->addWidget(new QLabel("Ollama Endpoint URL:", this));
    ollamaEndpointEdit = new QLineEdit(this);
    ollamaEndpointEdit->setText(config->getOllamaEndpoint());
    ollamaLayout->addWidget(ollamaEndpointEdit);
    mainLayout->addWidget(ollamaGroup);

    
    QGroupBox *promptGroup = new QGroupBox("System Instruction Prompt", this);
    QVBoxLayout *promptLayout = new QVBoxLayout(promptGroup);
    promptLayout->addWidget(new QLabel("Modify how the AI structures the CSS/HTML code:", this));
    systemPromptEdit = new QTextEdit(this);
    systemPromptEdit->setPlainText(config->getSystemPrompt());
    promptLayout->addWidget(systemPromptEdit);
    mainLayout->addWidget(promptGroup);

    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnCancel = new QPushButton("Cancel", this);
    btnSave = new QPushButton("Save Settings", this);
    btnSave->setStyleSheet("background-color: #7c4dff; font-weight: bold; color: white;");

    connect(btnCancel, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &SettingsDialog::saveSettings);

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    mainLayout->addLayout(btnLayout);
}

void SettingsDialog::saveSettings() {
    config->setGeminiKey(geminiKeyEdit->text().trimmed());
    config->setOpenAIKey(openaiKeyEdit->text().trimmed());
    config->setOllamaEndpoint(ollamaEndpointEdit->text().trimmed());
    config->setSystemPrompt(systemPromptEdit->toPlainText());
    accept();
}

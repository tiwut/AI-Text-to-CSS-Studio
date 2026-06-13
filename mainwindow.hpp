#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include "config.hpp"
#include "ai_client.hpp"
#include "preview_view.hpp"
#include "code_editor.hpp"
#include <QProgressBar>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSettings();
    void onProviderChanged(const QString &provider);
    void onOllamaModelsLoaded(const QStringList &models);
    void onOllamaModelsFailed(const QString &err);
    void onResolutionChanged(const QString &preset);
    void generateArt();
    void onArtGenerated(const QString &htmlCode);
    void onArtGenerationFailed(const QString &err);
    void onCodeChanged();
    void reloadPreviewFromCode();
    void exportPng();
    void saveHtml();

private:
    void initUi();
    void setupConnections();

    ConfigManager config;
    AIClient *aiClient;

    
    QComboBox *comboProvider;
    QComboBox *comboModel;
    QTextEdit *txtPrompt;
    QPushButton *btnGenerate;
    QPushButton *btnSettings;

    
    QComboBox *comboResolution;
    QSpinBox *spinWidth;
    QSpinBox *spinHeight;
    QPushButton *btnExportPng;
    QPushButton *btnSaveHtml;

    
    QTabWidget *tabWidget;
    PreviewView *previewView;
    CodeEditor *codeEditor;

    QTimer *debounceTimer;
    QProgressBar *progressBar;
};

#endif 

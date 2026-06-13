#include "mainwindow.hpp"
#include "settingsdialog.hpp"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

const QString APP_STYLE = R"(
QMainWindow {
    background-color: #0c0c14;
}

QWidget {
    color: #e2e2e9;
    font-family: 'Inter', -apple-system, sans-serif;
    font-size: 13px;
}

#side_panel {
    background-color: #131322;
    border-right: 1px solid #232338;
}

#logo_title {
    color: #ffffff;
    font-weight: bold;
    font-size: 18px;
}

#logo_subtitle {
    color: #7c7c9c;
    font-size: 11px;
}

QLabel {
    color: #a0a0b2;
    font-weight: 500;
}

QLineEdit, QTextEdit, QComboBox, QSpinBox {
    background-color: #1c1c30;
    border: 1px solid #2c2c46;
    border-radius: 6px;
    padding: 6px 10px;
    color: #ffffff;
}

QLineEdit:focus, QTextEdit:focus, QComboBox:focus, QSpinBox:focus {
    border: 1px solid #7c4dff;
    background-color: #202038;
}

QComboBox::drop-down {
    border: 0px;
}

QComboBox QAbstractItemView {
    background-color: #1c1c30;
    border: 1px solid #2c2c46;
    selection-background-color: #7c4dff;
    selection-color: #ffffff;
}

QPushButton {
    background-color: #24243e;
    border: 1px solid #3c3c62;
    border-radius: 6px;
    color: #ffffff;
    font-weight: bold;
    padding: 8px 15px;
}

QPushButton:hover {
    background-color: #2c2c4e;
    border: 1px solid #4e4e7c;
}

QPushButton:pressed {
    background-color: #1a1a2e;
}

#btn_generate {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7c4dff, stop:1 #651fff);
    border: none;
    color: #ffffff;
    font-weight: bold;
    font-size: 14px;
    border-radius: 6px;
    padding: 10px;
}

#btn_generate:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #926dff, stop:1 #7c3eff);
}

#btn_generate:pressed {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6038d6, stop:1 #4f14d6);
}

#btn_export {
    background-color: #00e676;
    color: #0b0b0f;
    font-weight: bold;
    border: none;
}

#btn_export:hover {
    background-color: #33eb91;
}

#btn_export:pressed {
    background-color: #00b35c;
}

QTabWidget::pane {
    border: 1px solid #232338;
    background-color: #0c0c14;
    border-radius: 8px;
}

QTabBar::tab {
    background-color: #131322;
    color: #7c7c9c;
    border: 1px solid #232338;
    border-bottom: none;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    padding: 8px 16px;
    margin-right: 4px;
}

QTabBar::tab:hover {
    background-color: #1c1c30;
    color: #ffffff;
}

QTabBar::tab:selected {
    background-color: #0c0c14;
    color: #ffffff;
    border-bottom: 2px solid #7c4dff;
    font-weight: bold;
}

QStatusBar {
    background-color: #0c0c14;
    color: #7c7c9c;
    border-top: 1px solid #1c1c30;
}

QSplitter::handle {
    background-color: #232338;
}

QGroupBox {
    border: 1px solid #232338;
    border-radius: 8px;
    margin-top: 15px;
    padding-top: 15px;
    font-weight: bold;
    color: #ffffff;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 10px;
    padding: 0 5px;
}
)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), aiClient(new AIClient(this)), debounceTimer(new QTimer(this))
{
    initUi();
    setupConnections();
}

MainWindow::~MainWindow() {
}

void MainWindow::initUi() {
    setWindowTitle("AI Text-to-CSS Layer Studio (C++)");
    resize(1200, 750);
    setStyleSheet(APP_STYLE);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter);

    
    
    
    QWidget *leftWidget = new QWidget(this);
    leftWidget->setObjectName("side_panel");
    leftWidget->setMinimumWidth(320);
    leftWidget->setMaximumWidth(400);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(15, 15, 15, 15);
    leftLayout->setSpacing(12);

    
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(2);
    QLabel *lblLogo = new QLabel("CSS Art Engine", this);
    lblLogo->setObjectName("logo_title");
    QLabel *lblSub = new QLabel("Generate multi-resolution CSS illustrations using Text LLMs", this);
    lblSub->setObjectName("logo_subtitle");
    lblSub->setWordWrap(true);
    headerLayout->addWidget(lblLogo);
    headerLayout->addWidget(lblSub);
    leftLayout->addLayout(headerLayout);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #232338;");
    leftLayout->addWidget(line);

    
    btnSettings = new QPushButton("⚙ API Credentials && Settings", this);
    leftLayout->addWidget(btnSettings);

    
    leftLayout->addWidget(new QLabel("AI Provider:", this));
    comboProvider = new QComboBox(this);
    comboProvider->addItems({"Ollama", "Gemini", "OpenAI"});
    comboProvider->setCurrentText(config.getSelectedProvider());
    leftLayout->addWidget(comboProvider);

    
    leftLayout->addWidget(new QLabel("Model:", this));
    comboModel = new QComboBox(this);
    comboModel->setEditable(true);
    leftLayout->addWidget(comboModel);

    
    leftLayout->addWidget(new QLabel("Prompt Description:", this));
    txtPrompt = new QTextEdit(this);
    txtPrompt->setPlaceholderText(
        "Describe the illustration you want to generate...\n"
        "e.g., 'A sleek retro sunset with neon grid mountains and glowing palm trees'"
    );
    leftLayout->addWidget(txtPrompt);

    
    btnGenerate = new QPushButton("🎨 Generate CSS Art", this);
    btnGenerate->setObjectName("btn_generate");
    leftLayout->addWidget(btnGenerate);

    QFrame *line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("background-color: #232338;");
    leftLayout->addWidget(line2);

    
    QGroupBox *exportGroup = new QGroupBox("Export Configuration", this);
    QVBoxLayout *exportLayout = new QVBoxLayout(exportGroup);

    exportLayout->addWidget(new QLabel("Export Resolution Preset:", this));
    comboResolution = new QComboBox(this);
    comboResolution->addItems({
        "Square HD (1024x1024)",
        "Square 2K (2048x2048)",
        "Full HD Landscape (1920x1080)",
        "4K Ultra HD (3840x2160)",
        "Mobile Portrait (1080x1920)",
        "Custom..."
    });
    exportLayout->addWidget(comboResolution);

    QHBoxLayout *customSizeLayout = new QHBoxLayout();
    customSizeLayout->addWidget(new QLabel("W:", this));
    spinWidth = new QSpinBox(this);
    spinWidth->setRange(128, 8192);
    spinWidth->setValue(1024);
    spinWidth->setEnabled(false);
    customSizeLayout->addWidget(spinWidth);

    customSizeLayout->addWidget(new QLabel("H:", this));
    spinHeight = new QSpinBox(this);
    spinHeight->setRange(128, 8192);
    spinHeight->setValue(1024);
    spinHeight->setEnabled(false);
    customSizeLayout->addWidget(spinHeight);
    exportLayout->addLayout(customSizeLayout);

    btnExportPng = new QPushButton("💾 Export Image (PNG)", this);
    btnExportPng->setObjectName("btn_export");
    exportLayout->addWidget(btnExportPng);

    btnSaveHtml = new QPushButton("🌐 Save HTML/CSS Source", this);
    exportLayout->addWidget(btnSaveHtml);

    leftLayout->addWidget(exportGroup);
    leftLayout->addStretch();

    mainSplitter->addWidget(leftWidget);

    
    
    
    tabWidget = new QTabWidget(this);
    
    previewView = new PreviewView(this);
    tabWidget->addTab(previewView, "👁  Live Canvas");

    codeEditor = new CodeEditor(this);
    tabWidget->addTab(codeEditor, "📝 HTML/CSS Source");

    mainSplitter->addWidget(tabWidget);

    mainSplitter->setSizes({350, 850});

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 0);
    progressBar->setTextVisible(false);
    progressBar->setMaximumHeight(12);
    progressBar->setMaximumWidth(150);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            background-color: #1c1c30;
            border: 1px solid #2c2c46;
            border-radius: 6px;
        }
        QProgressBar::chunk {
            background-color: #7c4dff;
            border-radius: 6px;
        }
    )");
    progressBar->setVisible(false);
    statusBar()->addPermanentWidget(progressBar);

    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections() {
    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(comboProvider, &QComboBox::currentTextChanged, this, &MainWindow::onProviderChanged);
    connect(comboResolution, &QComboBox::currentTextChanged, this, &MainWindow::onResolutionChanged);
    connect(btnGenerate, &QPushButton::clicked, this, &MainWindow::generateArt);
    connect(btnExportPng, &QPushButton::clicked, this, &MainWindow::exportPng);
    connect(btnSaveHtml, &QPushButton::clicked, this, &MainWindow::saveHtml);

    
    debounceTimer->setSingleShot(true);
    connect(codeEditor, &CodeEditor::textChanged, this, &MainWindow::onCodeChanged);
    connect(debounceTimer, &QTimer::timeout, this, &MainWindow::reloadPreviewFromCode);

    
    connect(aiClient, &AIClient::progress, this, [this](const QString &msg) {
        statusBar()->showMessage(msg);
    });
    connect(aiClient, &AIClient::artGenerated, this, &MainWindow::onArtGenerated);
    connect(aiClient, &AIClient::errorOccurred, this, &MainWindow::onArtGenerationFailed);
    connect(aiClient, &AIClient::ollamaModelsLoaded, this, &MainWindow::onOllamaModelsLoaded);

    
    onProviderChanged(comboProvider->currentText());
}

void MainWindow::openSettings() {
    SettingsDialog dialog(&config, this);
    if (dialog.exec() == QDialog::Accepted) {
        statusBar()->showMessage("Settings saved successfully.", 3000);
        if (comboProvider->currentText() == "Ollama") {
            onProviderChanged("Ollama");
        }
    }
}

void MainWindow::onProviderChanged(const QString &provider) {
    config.setSelectedProvider(provider);
    comboModel->clear();

    QString savedModel = config.getSelectedModel(provider);

    if (provider == "Gemini") {
        QStringList models = {"gemini-2.5-flash", "gemini-2.0-flash", "gemini-1.5-flash", "gemini-1.5-pro", "gemini-2.0-pro-exp"};
        comboModel->addItems(models);
        if (models.contains(savedModel)) {
            comboModel->setCurrentText(savedModel);
        } else {
            comboModel->setCurrentText("gemini-2.5-flash");
        }
    } else if (provider == "OpenAI") {
        QStringList models = {"gpt-4o-mini", "gpt-4o", "gpt-3.5-turbo"};
        comboModel->addItems(models);
        if (models.contains(savedModel)) {
            comboModel->setCurrentText(savedModel);
        } else {
            comboModel->setCurrentText("gpt-4o-mini");
        }
    } else if (provider == "Ollama") {
        statusBar()->showMessage("Checking local Ollama models...");
        QString endpoint = config.getOllamaEndpoint();
        aiClient->fetchOllamaModels(endpoint);
    }
}

void MainWindow::onOllamaModelsLoaded(const QStringList &models) {
    if (comboProvider->currentText() != "Ollama") return;

    comboModel->clear();
    comboModel->addItems(models);

    QString savedModel = config.getSelectedModel("Ollama");
    if (!savedModel.isEmpty() && models.contains(savedModel)) {
        comboModel->setCurrentText(savedModel);
    } else if (!models.isEmpty()) {
        
        bool found = false;
        for (const QString &pref : {"llama3", "gemma2", "mistral", "phi3"}) {
            for (const QString &m : models) {
                if (m.contains(pref)) {
                    comboModel->setCurrentText(m);
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        if (!found) {
            comboModel->setCurrentText(models[0]);
        }
    }
    statusBar()->showMessage("Loaded Ollama models list.", 3000);
}

void MainWindow::onOllamaModelsFailed(const QString &err) {
    if (comboProvider->currentText() != "Ollama") return;

    comboModel->clear();
    comboModel->addItems({"llama3", "llama2", "gemma", "mistral", "phi3"});
    comboModel->setCurrentText("llama3");
    statusBar()->showMessage(QString("Ollama offline. Loaded fallback models. (Error: %1)").arg(err), 5000);
}

void MainWindow::onResolutionChanged(const QString &preset) {
    if (preset == "Square HD (1024x1024)") {
        spinWidth->setValue(1024);
        spinHeight->setValue(1024);
        spinWidth->setEnabled(false);
        spinHeight->setEnabled(false);
    } else if (preset == "Square 2K (2048x2048)") {
        spinWidth->setValue(2048);
        spinHeight->setValue(2048);
        spinWidth->setEnabled(false);
        spinHeight->setEnabled(false);
    } else if (preset == "Full HD Landscape (1920x1080)") {
        spinWidth->setValue(1920);
        spinHeight->setValue(1080);
        spinWidth->setEnabled(false);
        spinHeight->setEnabled(false);
    } else if (preset == "4K Ultra HD (3840x2160)") {
        spinWidth->setValue(3840);
        spinHeight->setValue(2160);
        spinWidth->setEnabled(false);
        spinHeight->setEnabled(false);
    } else if (preset == "Mobile Portrait (1080x1920)") {
        spinWidth->setValue(1080);
        spinHeight->setValue(1920);
        spinWidth->setEnabled(false);
        spinHeight->setEnabled(false);
    } else { 
        spinWidth->setEnabled(true);
        spinHeight->setEnabled(true);
    }
}

void MainWindow::generateArt() {
    QString prompt = txtPrompt->toPlainText().trimmed();
    if (prompt.isEmpty()) {
        QMessageBox::warning(this, "Empty Prompt", "Please enter a detailed prompt to describe the design you want.");
        return;
    }

    QString provider = comboProvider->currentText();
    QString model = comboModel->currentText();

    config.setSelectedModel(provider, model);

    QString apiKey = "";
    if (provider == "Gemini") {
        apiKey = config.getGeminiKey();
        if (apiKey.isEmpty()) {
            QMessageBox::critical(this, "API Key Missing", "Please set your Google Gemini API key in Settings.");
            openSettings();
            return;
        }
    } else if (provider == "OpenAI") {
        apiKey = config.getOpenAIKey();
        if (apiKey.isEmpty()) {
            QMessageBox::critical(this, "API Key Missing", "Please set your OpenAI API key in Settings.");
            openSettings();
            return;
        }
    }

    QString endpoint = config.getOllamaEndpoint();
    QString systemPrompt = config.getSystemPrompt();

    
    btnGenerate->setEnabled(false);
    btnGenerate->setText("⏳ Generating Art...");
    progressBar->setVisible(true);
    statusBar()->showMessage("Querying LLM, styling content, please wait...");

    aiClient->generateArt(provider, model, prompt, systemPrompt, apiKey, endpoint);
}

void MainWindow::onArtGenerated(const QString &htmlCode) {
    btnGenerate->setEnabled(true);
    btnGenerate->setText("🎨 Generate CSS Art");
    progressBar->setVisible(false);
    statusBar()->showMessage("Art generation complete! Loaded to canvas.", 4000);

    
    codeEditor->blockSignals(true);
    codeEditor->setPlainText(htmlCode);
    codeEditor->blockSignals(false);

    
    previewView->updateHtml(htmlCode);
    
    
    tabWidget->setCurrentIndex(0);
}

void MainWindow::onArtGenerationFailed(const QString &err) {
    btnGenerate->setEnabled(true);
    btnGenerate->setText("🎨 Generate CSS Art");
    progressBar->setVisible(false);
    statusBar()->showMessage("Generation failed.", 4000);
    QMessageBox::critical(this, "Generation Error", QString("An error occurred:\n%1").arg(err));
}

void MainWindow::onCodeChanged() {
    debounceTimer->start(500);
}

void MainWindow::reloadPreviewFromCode() {
    QString code = codeEditor->toPlainText();
    previewView->updateHtml(code);
    statusBar()->showMessage("Updated Live Canvas from Editor.", 2000);
}

void MainWindow::exportPng() {
    QString currentHtml = previewView->currentHtml();
    if (currentHtml.isEmpty()) {
        QMessageBox::warning(this, "Export Failed", "Please generate your artwork before exporting.");
        return;
    }

    int w = spinWidth->value();
    int h = spinHeight->value();

    QString defaultPath = QDir::homePath() + "/css_art_export.png";
    QString filePath = QFileDialog::getSaveFileName(
        this, "Save Artwork Image",
        defaultPath,
        "PNG Images (*.png);;JPEG Images (*.jpg *.jpeg)"
    );

    if (filePath.isEmpty()) return;

    statusBar()->showMessage(QString("Rendering artwork at %1x%2 for export...").arg(w).arg(h));
    btnExportPng->setEnabled(false);
    btnExportPng->setText("⏳ Rendering...");

    previewView->exportImage(w, h, filePath, 
        [this](QString path) {
            btnExportPng->setEnabled(true);
            btnExportPng->setText("💾 Export Image (PNG)");
            statusBar()->showMessage(QString("Artwork exported successfully to %1").arg(path), 5000);
            QMessageBox::information(this, "Export Complete", QString("Successfully exported high-resolution image to:\n%1").arg(path));
        },
        [this](QString errMsg) {
            btnExportPng->setEnabled(true);
            btnExportPng->setText("💾 Export Image (PNG)");
            statusBar()->showMessage("Export failed.", 4000);
            QMessageBox::critical(this, "Export Error", QString("Failed to export high-resolution canvas:\n%1").arg(errMsg));
        }
    );
}

void MainWindow::saveHtml() {
    QString htmlCode = codeEditor->toPlainText();
    if (htmlCode.isEmpty()) {
        QMessageBox::warning(this, "Save Failed", "Please generate your artwork before saving.");
        return;
    }

    QString defaultPath = QDir::homePath() + "/css_art.html";
    QString filePath = QFileDialog::getSaveFileName(
        this, "Save HTML Source",
        defaultPath,
        "HTML Files (*.html *.htm)"
    );

    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Save Error", QString("Failed to open file for writing:\n%1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << htmlCode;
    file.close();

    statusBar()->showMessage(QString("HTML source saved to %1").arg(filePath), 4000);
    QMessageBox::information(this, "Save Complete", QString("HTML/CSS code saved to:\n%1").arg(filePath));
}

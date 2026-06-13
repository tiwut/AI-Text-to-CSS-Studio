#include "preview_view.hpp"
#include <QTimer>
#include <QDebug>

ImageExporter::ImageExporter(const QString &htmlContent, int width, int height, int previewWidth, int previewHeight, const QString &outputPath, QObject *parent)
    : QObject(parent), htmlContent(htmlContent), width(width), height(height), previewWidth(previewWidth), previewHeight(previewHeight), outputPath(outputPath), view(nullptr)
{
}

void ImageExporter::start() {
    view = new QWebEngineView();
    
    view->setAttribute(Qt::WidgetAttribute::WA_DontShowOnScreen, true);
    view->resize(width, height);
    
    connect(view, &QWebEngineView::loadFinished, this, &ImageExporter::onLoadFinished);
    
    double scaleX = (double)width / previewWidth;
    double scaleY = (double)height / previewHeight;
    double zoomFactor = qMin(scaleX, scaleY);

    QString styledHtml = htmlContent + QString(R"(
        <style>
        ::-webkit-scrollbar { display: none !important; }
        html {
            background: #0b0b0f !important;
            overflow: hidden !important;
            width: 100% !important;
            height: 100% !important;
            margin: 0 !important;
            padding: 0 !important;
        }
        body {
            width: %1px !important;
            height: %2px !important;
            position: absolute !important;
            left: 50% !important;
            top: 50% !important;
            margin: 0 !important;
            margin-left: -%3px !important;
            margin-top: -%4px !important;
            transform: scale(%5) !important;
            transform-origin: center center !important;
            overflow: hidden !important;
            box-sizing: border-box !important;
        }
        </style>
    )").arg(previewWidth)
       .arg(previewHeight)
       .arg((double)previewWidth / 2.0)
       .arg((double)previewHeight / 2.0)
       .arg(zoomFactor);

    view->show();
    view->setHtml(styledHtml);
}

void ImageExporter::onLoadFinished(bool success) {
    if (!success) {
        emit error("Failed to render HTML at target resolution.");
        view->close();
        view->deleteLater();
        this->deleteLater();
        return;
    }
    
    QTimer::singleShot(800, this, &ImageExporter::capture);
}

void ImageExporter::capture() {
    try {
        QPixmap pixmap = view->grab();
        if (pixmap.isNull()) {
            emit error("Captured an empty image canvas.");
        } else {
            bool success = pixmap.save(outputPath);
            if (success) {
                emit finished(outputPath);
            } else {
                emit error("Could not write the image file. Check file path or permissions.");
            }
        }
    } catch (const std::exception &e) {
        emit error(QString("Exception during export: %1").arg(e.what()));
    }
    view->close();
    view->deleteLater();
    this->deleteLater();
}




PreviewView::PreviewView(QWidget *parent)
    : QWebEngineView(parent), htmlCode("")
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    
    
    setHtml(R"(
        <html>
        <body style="margin:0; background:#0b0b0f; color:#8e8e9f; display:flex; flex-direction:column; align-items:center; justify-content:center; height:100vh; font-family:sans-serif;">
            <div style="border: 2px dashed #3a3a4a; padding: 40px; border-radius: 12px; text-align: center;">
                <h2 style="color: #ffffff; margin-bottom: 8px;">CSS Canvas Playground</h2>
                <p>Enter a prompt on the left to generate your first CSS vector image.</p>
            </div>
        </body>
        </html>
    )");
}

void PreviewView::updateHtml(const QString &code) {
    htmlCode = code;
    setHtml(code);
}

QString PreviewView::currentHtml() const {
    return htmlCode;
}

void PreviewView::exportImage(int width, int height, const QString &outputPath, 
                             const std::function<void(QString)> &successCallback, 
                             const std::function<void(QString)> &errorCallback) 
{
    if (htmlCode.isEmpty()) {
        errorCallback("No content to export. Generate an image first.");
        return;
    }

    int pWidth = this->width();
    int pHeight = this->height();
    if (pWidth <= 0) pWidth = 800;
    if (pHeight <= 0) pHeight = 800;

    ImageExporter *exporter = new ImageExporter(htmlCode, width, height, pWidth, pHeight, outputPath);
    connect(exporter, &ImageExporter::finished, this, [successCallback](const QString &path) {
        successCallback(path);
    });
    connect(exporter, &ImageExporter::error, this, [errorCallback](const QString &errMsg) {
        errorCallback(errMsg);
    });
    exporter->start();
}

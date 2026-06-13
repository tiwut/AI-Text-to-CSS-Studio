#include <QApplication>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QDir>
#include "preview_view.hpp"

const QString HTML_SAMPLE = R"(
<!DOCTYPE html>
<html>
<head>
<style>
body {
    margin: 0;
    width: 100vw;
    height: 100vh;
    background: linear-gradient(135deg, #120c1f 0%, #1a0f30 100%);
    display: flex;
    justify-content: center;
    align-items: center;
    color: #ff007f;
    font-size: 4rem;
    font-family: sans-serif;
    text-shadow: 0 0 20px #ff007f;
}
.circle {
    width: 200px;
    height: 200px;
    background: #00f0ff;
    border-radius: 50%;
    box-shadow: 0 0 30px #00f0ff;
}
</style>
</head>
<body>
    <div class="circle"></div>
</body>
</html>
)";

int main(int argc, char *argv[]) {
    
    char* offscreenArgv[] = { argv[0], (char*)"-platform", (char*)"offscreen" };
    int offscreenArgc = 3;
    
    QApplication app(offscreenArgc, offscreenArgv);
    
    QString outputPath = QDir::currentPath() + "/test_export_output_cpp.png";
    if (QFile::exists(outputPath)) {
        QFile::remove(outputPath);
    }
    
    qDebug() << "Starting C++ offscreen rendering at 1920x1080...";
    
    ImageExporter *exporter = new ImageExporter(HTML_SAMPLE, 1920, 1080, 800, 800, outputPath);
    
    QObject::connect(exporter, &ImageExporter::finished, [&app](const QString &path) {
        qDebug() << "SUCCESS: Export saved to" << path;
        QFile file(path);
        if (file.exists() && file.size() > 0) {
            qDebug() << "VERIFIED: C++ Output file size is" << file.size() << "bytes";
        } else {
            qDebug() << "ERROR: Saved file is empty or missing!";
        }
        app.quit();
    });
    
    QObject::connect(exporter, &ImageExporter::error, [&app](const QString &errMsg) {
        qDebug() << "ERROR: Export failed:" << errMsg;
        app.quit();
    });
    
    exporter->start();
    
    return app.exec();
}

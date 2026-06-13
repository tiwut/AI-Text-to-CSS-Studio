#ifndef PREVIEW_VIEW_HPP
#define PREVIEW_VIEW_HPP

#include <QObject>
#include <QSize>
#include <QTimer>
#include <QWebEngineView>
#include <QPixmap>

class ImageExporter : public QObject {
    Q_OBJECT
public:
    ImageExporter(const QString &htmlContent, int width, int height, int previewWidth, int previewHeight, const QString &outputPath, QObject *parent = nullptr);
    void start();

signals:
    void finished(const QString &path);
    void error(const QString &errMsg);

private slots:
    void onLoadFinished(bool success);
    void capture();

private:
    QString htmlContent;
    int width;
    int height;
    int previewWidth;
    int previewHeight;
    QString outputPath;
    QWebEngineView *view;
};


class PreviewView : public QWebEngineView {
    Q_OBJECT
public:
    explicit PreviewView(QWidget *parent = nullptr);

    void updateHtml(const QString &htmlCode);
    void exportImage(int width, int height, const QString &outputPath, 
                     const std::function<void(QString)> &successCallback, 
                     const std::function<void(QString)> &errorCallback);

    QString currentHtml() const;

private:
    QString htmlCode;
};

#endif 

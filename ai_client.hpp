#ifndef AI_CLIENT_HPP
#define AI_CLIENT_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AIClient : public QObject {
    Q_OBJECT
public:
    explicit AIClient(QObject *parent = nullptr);
    ~AIClient();

    void generateArt(const QString &provider, 
                     const QString &model, 
                     const QString &prompt, 
                     const QString &systemPrompt, 
                     const QString &apiKey, 
                     const QString &endpoint);

    void fetchOllamaModels(const QString &endpoint);

signals:
    void progress(const QString &message);
    void artGenerated(const QString &htmlCode);
    void ollamaModelsLoaded(const QStringList &models);
    void errorOccurred(const QString &errorMsg);

private slots:
    void onArtReplyFinished(QNetworkReply *reply);
    void onOllamaModelsReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QString currentProvider;
};

#endif 

#include "ai_client.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

static QString cleanHtmlCode(const QString &text) {
    QString cleaned = text.trimmed();
    
    if (cleaned.startsWith("```html")) {
        cleaned = cleaned.mid(7);
    } else if (cleaned.startsWith("```")) {
        cleaned = cleaned.mid(3);
    }
    if (cleaned.endsWith("```")) {
        cleaned = cleaned.left(cleaned.length() - 3);
    }
    return cleaned.trimmed();
}

AIClient::AIClient(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this))
{
}

AIClient::~AIClient() {
}

void AIClient::generateArt(const QString &provider, 
                           const QString &model, 
                           const QString &prompt, 
                           const QString &systemPrompt, 
                           const QString &apiKey, 
                           const QString &endpoint) 
{
    currentProvider = provider;
    emit progress(QString("Connecting to %1 (%2)...").arg(provider, model));

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray payload;

    if (provider == "OpenAI") {
        if (apiKey.isEmpty()) {
            emit errorOccurred("OpenAI API key is missing. Set it in Settings.");
            return;
        }
        request.setUrl(QUrl("https://api.openai.com/v1/chat/completions"));
        request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

        QJsonObject root;
        root["model"] = model;
        root["temperature"] = 0.7;

        QJsonArray messages;
        QJsonObject sysMsg;
        sysMsg["role"] = "system";
        sysMsg["content"] = systemPrompt;
        messages.append(sysMsg);

        QJsonObject userMsg;
        userMsg["role"] = "user";
        userMsg["content"] = prompt;
        messages.append(userMsg);

        root["messages"] = messages;
        payload = QJsonDocument(root).toJson();

    } else if (provider == "Gemini") {
        if (apiKey.isEmpty()) {
            emit errorOccurred("Gemini API key is missing. Set it in Settings.");
            return;
        }
        
        QString urlStr = QString("https://generativelanguage.googleapis.com/v1beta/models/%1:generateContent?key=%2")
                         .arg(model, apiKey);
        request.setUrl(QUrl(urlStr));

        QJsonObject root;
        
        QJsonArray contents;
        QJsonObject contentObj;
        QJsonArray parts;
        QJsonObject partObj;
        partObj["text"] = prompt;
        parts.append(partObj);
        contentObj["parts"] = parts;
        contents.append(contentObj);
        root["contents"] = contents;

        if (!systemPrompt.isEmpty()) {
            QJsonObject sysInstruction;
            QJsonArray sysParts;
            QJsonObject sysPartObj;
            sysPartObj["text"] = systemPrompt;
            sysParts.append(sysPartObj);
            sysInstruction["parts"] = sysParts;
            root["systemInstruction"] = sysInstruction;
        }

        payload = QJsonDocument(root).toJson();

    } else if (provider == "Ollama") {
        QString urlStr = QString("%1/api/chat").arg(endpoint);
        request.setUrl(QUrl(urlStr));

        QJsonObject root;
        root["model"] = model;
        root["stream"] = false;

        QJsonArray messages;
        QJsonObject sysMsg;
        sysMsg["role"] = "system";
        sysMsg["content"] = systemPrompt;
        messages.append(sysMsg);

        QJsonObject userMsg;
        userMsg["role"] = "user";
        userMsg["content"] = prompt;
        messages.append(userMsg);

        root["messages"] = messages;
        payload = QJsonDocument(root).toJson();
    } else {
        emit errorOccurred(QString("Unknown provider: %1").arg(provider));
        return;
    }

    QNetworkReply *reply = networkManager->post(request, payload);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onArtReplyFinished(reply);
    });
}

void AIClient::onArtReplyFinished(QNetworkReply *reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString err = reply->errorString();
        
        QByteArray body = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(body);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("error")) {
                QJsonObject errObj = obj["error"].toObject();
                if (errObj.contains("message")) {
                    err = errObj["message"].toString();
                } else if (obj["error"].isString()) {
                    err = obj["error"].toString();
                }
            }
        }
        emit errorOccurred(QString("Network Error: %1").arg(err));
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Failed to parse response JSON from AI API.");
        return;
    }

    QJsonObject root = doc.object();
    QString extractedCode = "";

    if (currentProvider == "OpenAI") {
        QJsonArray choices = root["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject first = choices[0].toObject();
            QJsonObject msg = first["message"].toObject();
            extractedCode = msg["content"].toString();
        }
    } else if (currentProvider == "Gemini") {
        QJsonArray candidates = root["candidates"].toArray();
        if (!candidates.isEmpty()) {
            QJsonObject first = candidates[0].toObject();
            QJsonObject content = first["content"].toObject();
            QJsonArray parts = content["parts"].toArray();
            if (!parts.isEmpty()) {
                extractedCode = parts[0].toObject()["text"].toString();
            }
        }
    } else if (currentProvider == "Ollama") {
        QJsonObject message = root["message"].toObject();
        extractedCode = message["content"].toString();
    }

    if (extractedCode.isEmpty()) {
        emit errorOccurred("AI returned an empty response or unexpected JSON structure.");
    } else {
        emit artGenerated(cleanHtmlCode(extractedCode));
    }
}

void AIClient::fetchOllamaModels(const QString &endpoint) {
    QNetworkRequest request;
    request.setUrl(QUrl(QString("%1/api/tags").arg(endpoint)));
    
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onOllamaModelsReplyFinished(reply);
    });
}

void AIClient::onOllamaModelsReplyFinished(QNetworkReply *reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Could not fetch Ollama models: %1").arg(reply->errorString()));
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Ollama returned invalid model JSON tags.");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray modelsArray = root["models"].toArray();
    QStringList modelsList;

    for (int i = 0; i < modelsArray.size(); ++i) {
        QJsonObject modelObj = modelsArray[i].toObject();
        QString name = modelObj["name"].toString();
        if (!name.isEmpty()) {
            modelsList.append(name);
        }
    }

    emit ollamaModelsLoaded(modelsList);
}

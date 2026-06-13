#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QString>
#include <QSettings>

extern const QString DEFAULT_SYSTEM_PROMPT;

class ConfigManager {
public:
    ConfigManager();

    QString getGeminiKey() const;
    void setGeminiKey(const QString &key);

    QString getOpenAIKey() const;
    void setOpenAIKey(const QString &key);

    QString getOllamaEndpoint() const;
    void setOllamaEndpoint(const QString &endpoint);

    QString getSelectedProvider() const;
    void setSelectedProvider(const QString &provider);

    QString getSelectedModel(const QString &provider) const;
    void setSelectedModel(const QString &provider, const QString &model);

    QString getSystemPrompt() const;
    void setSystemPrompt(const QString &prompt);

    void resetToDefaults();

private:
    QSettings settings;
};

#endif 

#include "config.hpp"

const QString DEFAULT_SYSTEM_PROMPT = QString::fromUtf8(R"(You are an elite CSS and HTML artist.
Your goal is to generate a beautiful, detailed, visually stunning, and highly accurate static illustration or graphic that perfectly represents the user's prompt using ONLY a single self-contained HTML file.

CRITICAL RULES:
1. Output ONLY a valid HTML document.
2. Do NOT write any Markdown code block backticks (like ```html ... ```). Start directly with <!DOCTYPE html> or <html> and end with </html>.
3. Absolutely NO conversational responses, explanations, comments, or suggestions outside the HTML.
4. Use inline <style> tags containing CSS rules:
   - Use CSS gradients, flexbox, grid, box-shadows, clip-paths, and inline SVG shapes to build rich graphics.
   - The graphic must be high-quality, complex, and closely resemble the user's prompt. It should look like a professional, polished vector graphic design.
   - Ensure the layout is responsive and centered. Set `body { margin: 0; width: 100vw; height: 100vh; display: flex; align-items: center; justify-content: center; background: #0b0b0f; overflow: hidden; font-family: sans-serif; }`.
   - All visual elements must scale proportionally so they don't break at higher resolutions. Use relative units (%, vmin, vmax, rem) instead of absolute pixels (px) where possible, or use a container with an aspect ratio.
5. Absolutely DO NOT create animations, transitions, or moving elements. The image must be completely STATIC.
6. Focus heavily on static detail: depth, lighting, perspective, gradients, layering, shadows, and crisp SVG paths to represent complex items.
7. If the user prompt requests text inside the image, style it with modern typography, shadows, or gradients.
)");

ConfigManager::ConfigManager()
    : settings("AITxtToPicture", "Studio")
{
    
    if (!settings.contains("ollama_endpoint")) {
        settings.setValue("ollama_endpoint", "http://localhost:11434");
    }
    if (!settings.contains("selected_provider")) {
        settings.setValue("selected_provider", "Ollama");
    }
    
    
    if (settings.contains("system_prompt")) {
        QString current = settings.value("system_prompt").toString();
        if (current.contains("Create subtle micro-animations")) {
            settings.setValue("system_prompt", DEFAULT_SYSTEM_PROMPT);
        }
    } else {
        settings.setValue("system_prompt", DEFAULT_SYSTEM_PROMPT);
    }
    if (!settings.contains("models/OpenAI")) {
        settings.setValue("models/OpenAI", "gpt-4o-mini");
    }
    if (!settings.contains("models/Gemini")) {
        settings.setValue("models/Gemini", "gemini-2.5-flash");
    }
    if (!settings.contains("models/Ollama")) {
        settings.setValue("models/Ollama", "llama3");
    }
}

QString ConfigManager::getGeminiKey() const {
    return settings.value("gemini_api_key", "").toString();
}

void ConfigManager::setGeminiKey(const QString &key) {
    settings.setValue("gemini_api_key", key);
}

QString ConfigManager::getOpenAIKey() const {
    return settings.value("openai_api_key", "").toString();
}

void ConfigManager::setOpenAIKey(const QString &key) {
    settings.setValue("openai_api_key", key);
}

QString ConfigManager::getOllamaEndpoint() const {
    return settings.value("ollama_endpoint", "http://localhost:11434").toString();
}

void ConfigManager::setOllamaEndpoint(const QString &endpoint) {
    settings.setValue("ollama_endpoint", endpoint);
}

QString ConfigManager::getSelectedProvider() const {
    return settings.value("selected_provider", "Ollama").toString();
}

void ConfigManager::setSelectedProvider(const QString &provider) {
    settings.setValue("selected_provider", provider);
}

QString ConfigManager::getSelectedModel(const QString &provider) const {
    return settings.value("models/" + provider, "").toString();
}

void ConfigManager::setSelectedModel(const QString &provider, const QString &model) {
    settings.setValue("models/" + provider, model);
}

QString ConfigManager::getSystemPrompt() const {
    return settings.value("system_prompt", DEFAULT_SYSTEM_PROMPT).toString();
}

void ConfigManager::setSystemPrompt(const QString &prompt) {
    settings.setValue("system_prompt", prompt);
}

void ConfigManager::resetToDefaults() {
    settings.setValue("ollama_endpoint", "http://localhost:11434");
    settings.setValue("selected_provider", "Ollama");
    settings.setValue("system_prompt", DEFAULT_SYSTEM_PROMPT);
    settings.setValue("models/OpenAI", "gpt-4o-mini");
    settings.setValue("models/Gemini", "gemini-2.5-flash");
    settings.setValue("models/Ollama", "llama3");
    settings.setValue("openai_api_key", "");
    settings.setValue("gemini_api_key", "");
}

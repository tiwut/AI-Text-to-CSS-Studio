#include "code_editor.hpp"
#include <QPainter>
#include <QTextBlock>
#include <QTextFormat>




HTMLHighlighter::HTMLHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    
    QColor tagColor("#ff79c6");       
    QColor attrColor("#50fa7b");      
    QColor stringColor("#f1fa8c");    
    QColor cssPropColor("#8be9fd");   
    QColor cssValColor("#ffb86c");    
    QColor commentColor("#6272a4");   

    
    QTextCharFormat tagFormat;
    tagFormat.setForeground(tagColor);
    tagFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(R"(</?[a-zA-Z0-9:-]+)");
    rule.format = tagFormat;
    rules.append(rule);

    
    QTextCharFormat attrFormat;
    attrFormat.setForeground(attrColor);
    rule.pattern = QRegularExpression(R"(\s[a-zA-Z0-9:-]+(?==))");
    rule.format = attrFormat;
    rules.append(rule);

    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(stringColor);
    rule.pattern = QRegularExpression(R"("[^"\\]*(?:\\.[^"\\]*)*")");
    rule.format = stringFormat;
    rules.append(rule);

    
    rule.pattern = QRegularExpression(R"('[^'\\]*(?:\\.[^'\\]*)*')");
    rules.append(rule);

    
    QTextCharFormat cssPropFormat;
    cssPropFormat.setForeground(cssPropColor);
    rule.pattern = QRegularExpression(R"([a-zA-Z-]+\s*(?=:))");
    rule.format = cssPropFormat;
    rules.append(rule);

    
    QTextCharFormat cssValFormat;
    cssValFormat.setForeground(cssValColor);
    rule.pattern = QRegularExpression(R"((?<=:)\s*[^;]+)");
    rule.format = cssValFormat;
    rules.append(rule);

    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(commentColor);
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(R"(<!--.*?-->)");
    rule.format = commentFormat;
    rules.append(rule);
    
    rule.pattern = QRegularExpression(R"(/\*.*?\*/)");
    rules.append(rule);
}

void HTMLHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}





CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    
    setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #181824;
            color: #f8f8f2;
            border: 1px solid #282a36;
            border-radius: 8px;
            font-family: 'Courier New', Consolas, monospace;
            font-size: 13px;
            selection-background-color: #44475a;
            selection-color: #ffffff;
        }
    )");

    highlighter = new HTMLHighlighter(document());
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int maxBlock = qMax(1, blockCount());
    while (maxBlock >= 10) {
        maxBlock /= 10;
        digits++;
    }
    int space = 15 + fontMetrics().horizontalAdvance('9') * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int ) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#1e1e2e"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("#6272a4"));
            painter.drawText(0, top, lineNumberArea->width() - 5, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        blockNumber++;
    }
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor("#282a36");
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

#include <iostream>
#include <string>
#include "generated/preprocessor.cpp"

const int INDENT = 2;
const char INDENT_CHAR = ' ';
const char BLOCK_SEPARATOR = '#';
const char STATEMENT_SEPARATOR = ';';
const char END_LINE = '\n';

void reportError(const std::string& error) {
    std::cerr << "Syntax error (line " << lineIndex + 1 << "): " << error << std::endl;
    std::exit(1);
}

void reportIndentationError() {
    reportError("Invalid indentation");
}

std::vector<std::string> splitLines(const std::string& multilineStatement) {
    std::string delimiter(1, END_LINE);
    std::vector<std::string> result;

    size_t prevEndLineIndex = 0;
    size_t nextEndLineIndex;

    while ((nextEndLineIndex = multilineStatement.find(delimiter, prevEndLineIndex)) != std::string::npos) {
        result.push_back(multilineStatement.substr(prevEndLineIndex, nextEndLineIndex - prevEndLineIndex));
        prevEndLineIndex = nextEndLineIndex + 1;
    }

    result.push_back(multilineStatement.substr(prevEndLineIndex));

    return result;
}

int getWhitespacePrefixCount(const std::string& line) {
    int count = 0;

    for (auto it = line.begin(); it != line.end() && *it == INDENT_CHAR; it++) {
        count++;
    }

    return count;
}

bool isBlankLine(const std::string& line) {
    return std::all_of(line.begin(), line.end(), ::isspace);
}

void appendToLastLine(const std::string& sequence) {
    if (lines.empty()) {
        lines.push_back(sequence);
    } else {
        lines.back().append(sequence);
    }
}

void addNewLine() {
    lines.emplace_back("");
}

size_t appendLines(const std::string& multilineStatement) {
    if (multilineStatement.empty()) {
        return 0;
    }

    auto separateLines = splitLines(multilineStatement);

    for (size_t i = 0; i < separateLines.size(); i++) {
        appendToLastLine(separateLines[i]);

        if (i != separateLines.size() - 1) {
            addNewLine();
        }
    }

    return separateLines.size() - 1;
}

void markBlock() {
    if (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }

    appendToLastLine(std::string(1, INDENT_CHAR));
    appendToLastLine(std::string(1, BLOCK_SEPARATOR));
    addNewLine();
}

void markStatement() {
    appendToLastLine(std::string(1, INDENT_CHAR));

    if (!lines.empty() && !isBlankLine(lines.back())) {
        appendToLastLine(std::string(1, STATEMENT_SEPARATOR));
    }

    addNewLine();
}

void processStatements() {
    if (!isFirstStatement) {
        markStatement();
    }

    isFirstStatement = false;
}

void processIndentation(int indent) {
    if (!lines.empty() && lines.back().ends_with(';')) {
        reportError("Unexpected SEMICOLON");
    }

    markBlock();

    indents.push(indent);
}

void processOutdentation(int indent) {
    markStatement();

    while (indent < indents.top()) {
        markBlock();

        indents.pop();
    }
}

void processIndent(const std::string& line) {
    auto indent = getWhitespacePrefixCount(line);
    auto previousIndent = indents.top();

    if (indent % INDENT != 0 || indent - previousIndent > INDENT) {
        reportIndentationError();
    }

    if (indent > previousIndent) {
        processIndentation(indent);
    } else if (indent == previousIndent) {
        processStatements();
    } else {
        processOutdentation(indent);
    }
}

void outputProcessedLines() {
    auto blankLineIndexIterator = blankLineIndexes.begin();
    int outputLineIndex = 0;

    for (auto& line : lines) {
        while (blankLineIndexIterator != blankLineIndexes.end() && outputLineIndex == *blankLineIndexIterator) {
            std::cout << std::endl;

            outputLineIndex++;
            blankLineIndexIterator++;
        }

        std::cout << line << std::endl;

        outputLineIndex++;
    }
}

int main() {
    indents.push(0);

    yylex();

    outputProcessedLines();

    return 0;
}

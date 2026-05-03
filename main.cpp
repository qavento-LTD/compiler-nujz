#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <ctime>

using namespace std;

// --- الأجزاء الأساسية (التي قمت ببنائها سابقاً مع تطوير بسيط) ---

enum class TokenType { keywords, indentifier, operators, number, STRING };
struct Token { TokenType type; string value; };

// ذاكرة المتغيرات مع ميزة "وقت آخر استخدام" (للميزة الثورية)
struct Variable {
    int value;
    int accessCount = 0;
    time_t lastUsed;
};
map<string, Variable> symbolTable;

vector<Token> lexer(string source) {
    vector<Token> tokens;
    int i = 0;
    while (i < source.length()) {
        char current = source[i];
        if (isspace(current)) { i++; continue; }
        if (current == '"') {
            string text = ""; i++;
            while (i < source.length() && source[i] != '"') { text += source[i]; i++; }
            i++; tokens.push_back({TokenType::STRING, text}); continue;
        }
        if (isalpha(current)) {
            string word = "";
            while (i < source.length() && isalnum(source[i])) { word += source[i]; i++; }
            if (word == "out") tokens.push_back({TokenType::keywords, word});
            else tokens.push_back({TokenType::indentifier, word});
            continue;
        }
        if (isdigit(current)) {
            string num = "";
            while (i < source.length() && isdigit(source[i])) { num += source[i]; i++; }
            tokens.push_back({TokenType::number, num}); continue;
        }
        if (current == '=') tokens.push_back({TokenType::operators, "="});
        i++;
    }
    return tokens;
}

void parser(vector<Token> tokens) {
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::keywords && tokens[i].value == "out") {
            i++;
            if (i < tokens.size()) {
                if (tokens[i].type == TokenType::indentifier) {
                    string vName = tokens[i].value;
                    if (symbolTable.count(vName)) {
                        // تطبيق الميزة الثورية: التحقق من "مشاعر" المتغير
                        Variable &v = symbolTable[vName];
                        v.accessCount++;
                        v.lastUsed = time(0);
                        
                        if(v.accessCount > 5) cout << "😊 [Happy Var] ";
                        cout << v.value << endl;
                    } else {
                        cout << "😢 Error: Variable '" << vName << "' feels invisible (not defined)." << endl;
                    }
                }
                else if (tokens[i].type == TokenType::number) cout << tokens[i].value << endl;
                else if (tokens[i].type == TokenType::STRING) cout << tokens[i].value << endl;
            }
        }
        else if (tokens[i].type == TokenType::indentifier && (i + 1 < tokens.size())) {
            if (tokens[i + 1].type == TokenType::operators && tokens[i + 1].value == "=") {
                string varName = tokens[i].value;
                if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::number) {
                    symbolTable[varName] = { stoi(tokens[i + 2].value), 0, time(0) };
                    i += 2;
                }
            }
        }
    }
}

// --- نظام الـ Runtime التفاعلي (REPL) ---

void runREPL() {
    cout << "Welcome to Nujz Runtime v1.0 [Emotional Engine Enabled]" << endl;
    cout << "Type 'exit' to quit." << endl;
    string line;
    while (true) {
        cout << "Nujz >> ";
        getline(cin, line);
        if (line == "exit") break;
        if (line.empty()) continue;

        vector<Token> tokens = lexer(line);
        parser(tokens);
    }
}

int main(int argc, char* argv[]) {
    // إذا لم يتم تمرير ملف، شغل الـ Runtime التفاعلي
    if (argc < 2) {
        runREPL();
    } else {
        // إذا تم تمرير ملف، شغله كالمعتاد
        ifstream file(argv[1]);
        if (!file.is_open()) {
            cerr << "Could not open file: " << argv[1] << endl;
            return 1;
        }
        stringstream buffer;
        buffer << file.rdbuf();
        parser(lexer(buffer.str()));
    }
    return 0;
}

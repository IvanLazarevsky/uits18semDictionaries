#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

class DictionaryException  {
    string message;
public:
    DictionaryException(const std::string& msg) : message(msg) {}
    virtual string getMessage() const {
        return message;
    }

    virtual ~DictionaryException();
};

DictionaryException::~DictionaryException() {}

class ConfigurationException : public DictionaryException {
public:
    using DictionaryException::DictionaryException;
};

class FileFormatException : public DictionaryException {
public:
    using DictionaryException::DictionaryException;
    string getMessage() const override {
        return "Invalid file format: " + DictionaryException::getMessage();
    }
};

class FileNotFoundException : public DictionaryException {
public:
    FileNotFoundException(const string& filename) : DictionaryException("Cannot find file " + filename) {}
};


vector<string> find_word_f1(const string& filename, const std::string& word) {
    ifstream file(filename);
    if(!file) {
        throw FileNotFoundException(filename);
    }
    vector<string> result;
    std::string line;
    while(getline(file, line)) {
        istringstream iss(line);
        string dict_word;
        iss >> dict_word;
        string definition;
        if(!getline(file, definition)) {
            throw FileFormatException("Expected definition after " + dict_word);
        }
        if(dict_word == word)
            result.push_back(definition);
    }

    return result;
}

vector<string> find_word_context(const string& filename, const std::string& word) {
    ifstream file(filename);
    if(!file) {
        throw FileNotFoundException(filename);
    }

    vector<string> result;
    std::string line;
    std::string tokens;
    std::string punct = ".,!?;:-\"()[]";
    while(getline(file, line)) {
        string tokens = line;
        for(char& c : tokens) {
            if(punct.find(c) != string::npos)
                c = ' ';
        }

        istringstream iss(tokens);
        string dict_word;
        while(iss >> dict_word) {
            if(dict_word == word) {
                result.push_back(line);
                break;
            }
        }
    }
    return result;
}


class Config {
public:
    virtual vector<string> getF1Extensions() const = 0;
    virtual vector<string> getCtxExtensions() const = 0;
    virtual vector<string> getFilenames() const = 0;

    virtual ~Config() {}
};


class DefaultConfig : public Config {
    string prefix;
    vector<string> filenames = {"default.dict", "default.cdict"};
public:
    DefaultConfig(const string& prefix="") : prefix(prefix) {
        for(string& filename : filenames) {
            filename = prefix + filename;
        }
    }

    vector<string> getFilenames() const override {
        return filenames;
    }

    vector<string> getCtxExtensions() const override {
        return {"cdict", "ctx"};
    }

    vector<string> getF1Extensions() const override {
        return {"dict"};
    }
};


class Dictionary {
public:
    virtual vector<string> find(const string& word) const = 0;
    virtual string getName() const = 0;
    virtual ~Dictionary() = default;
};

class F1Dictionary : public Dictionary {
    string filename;
public:
    F1Dictionary(string f) : filename(f) {}
    vector<string> find(const string& word) const override {
        return find_word_f1(filename, word);
    }
    string getName() const override {
        return filename;
    }
};

class CtxDictionary : public Dictionary {
    string filename;
public:
    CtxDictionary(string f) : filename(f) {}
    vector<string> find(const string& word) const override {
        return find_word_context(filename, word);
    }
    string getName() const override {
        return filename;
    }
};


Dictionary* getDictionary(Config* config, const string& filename) {
    string extension;
    size_t dot_pos = filename.rfind(".");
    if(dot_pos != string::npos) {
        extension = filename.substr(dot_pos + 1);
        for(const string& ext : config->getF1Extensions()) {
            if(ext == extension)
                return new F1Dictionary(filename);
        }
        for(const string& ext : config->getCtxExtensions()) {
//            cout << ext << " " << extension << endl;
            if(ext == extension)
                return new CtxDictionary(filename);
        }
        throw ConfigurationException("Cannot find a handler for ." + extension + " extension");
    }
    else {
        throw ConfigurationException("File has no extension");
    }

}


int main()
{
    vector<Dictionary*> dictionaries;
    try {
        Config* config = new DefaultConfig("/home/ivan/programming/C++/Dictionaries/");
        vector<string> files = config->getFilenames();
        for(string f : files)
            dictionaries.push_back(getDictionary(config, f));
    }
    catch(ConfigurationException& ex) {
        cout << "Can't configure program, an error occured" << endl;
        cout << ex.getMessage() << endl;
    }

    while(true) {
        try {
            cout << "Enter word: ";

            string word;
            cin >> word;

            for(Dictionary* dict : dictionaries) {
                vector<string> res = dict->find(word);
                cout << dict->getName() << endl;
                if(!res.empty()) {
                    for(string def : res)
                        cout << def << endl;
                }
                else
                    cout << "Nothing found" << endl;
                cout << endl;
            }
        }
        catch(DictionaryException& ex) {
            cout << "Warn: " << ex.getMessage() << endl;
        }
    }



    return 0;
}



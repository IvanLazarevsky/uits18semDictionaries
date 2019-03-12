// Интерфейс конфига
class Config {
public:
    virtual vector<string> getF1Extensions() const = 0; 
    virtual vector<string> getCtxExtensions() const = 0;
    virtual vector<string> getFilenames() const = 0; 

    virtual ~Config() {}
};

// Функция getDictionary возвращает полиморфный объект, тип которого выбирается в зависимости от конфигурации и имени файла.
Dictionary* getDictionary(Config* config, const string& filename);

// Набросок main
int main()
{
    // Вектор полиморфных словарей
    vector<Dictionary*> dictionaries;
    // Конфигурация программы
    try {
        Config* config = new DefaultConfig("/home/ivan/Programming/QtWorkspace/Dictionaries/");
        vector<string> files = config->getFilenames();
        for(string f : files)
            dictionaries.push_back(getDictionary(config, f));
    }
    catch(ConfigurationException& ex) {
        cout << "Can't configure program, an error occured" << endl;
        cout << ex.getMessage() << endl;
        exit(1);
    }
    // Цикл ввода и поиска
    while(true) {
        try {
            cout << "Enter word: ";

            string word;
            cin >> word;

            for(Dictionary* dict : dictionaries) {
                // Список определений слова из этого словаря. find - виртуальная функция.
                vector<string> res = dict->find(word);
                cout << dict->getName() << endl; // У словаря есть имя (например - имя файл с ним)
		// Выводим все определения если есть хоть одно
                if(!res.empty()) {
                    for(string def : res)
                        cout << def << endl;
                }
                else // Иначе пишем, что ничего не нашли
                    cout << "Nothing found" << endl;
                cout << endl;
            }
        }
	// Обработка ошибок не завершает программу.
        catch(DictionaryException& ex) { 
            cout << "Warn: " << ex.getMessage() << endl;
        }
    }
    return 0;
}

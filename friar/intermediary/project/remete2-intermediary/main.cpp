#include <QCoreApplication>
#include <QLocale>
#include <QTimer>   //TEMPORARY!!!
#include <QTranslator>

#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "remete2-intermediary_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    puts("Executing Qt console application...");

    //////////
    //TEMPORARY!!!
    do {
        putchar('>');

        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) {
            continue;   // E.g. an Enter pressed
        }

        std::string commandName = tokens[0];
        tokens.erase(tokens.begin()); // Remove the command name from the tokens

        if (commandName == "help" || commandName == "h" || commandName == "?") {
            puts("Help: (c)onnect, (d)isconnect, E(x)it");
        } else if (commandName == "connect" || commandName == "c") {
            puts(commandName.c_str());
        } else if (commandName == "disconnect" || commandName == "d") {
            puts(commandName.c_str());
        } else if (commandName == "exit" || commandName == "x") {
            puts(commandName.c_str());
            //QTimer::singleShot(1000, &a, &QCoreApplication::quit);
            break;
        }
    } while (true);
    //////////

    puts("Exiting Qt console application...");
    return 0;

    //puts("Entering Qt application event loop...");
    //return a.exec();
}

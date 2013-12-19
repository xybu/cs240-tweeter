#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "tweeter.h"

const char OPT_OAUTH[] = "-o";
const char OPT_XAUTH[] = "-x";
const char OPT_USERNAME[] = "-u";
const char OPT_PASSWORD[] = "-p";
const char OPT_STATUS[] = "-m";
const char OPT_STATUS_FILE[] = "-f";
const char OPT_STATUS_STDIN[] = "-r";

const char USAGE[] = "\n"
                     "Usage: tweet [OPTION]...\n"
                     "Get OAuth access tokens from Twitter's OAuth service and "
                     "(optionally) post a status update on a user's timeline\n"
                     "\nOptions:\n"
                     "  %1\t\tLink with Twitter OAuth service, i.e get access tokens\n"
                     "  %2\t\tLink with Twitter XAuth service, i.e get access tokens using the XAuth protocol\n"
                     "  %3 <username>\tTwitter username to be used while using XAuth (-x option)\n"
                     "  %4 <password>\tTwitter password to be used while using XAuth (-x option)\n"
                     "  %5\t\tStatus update message, enclosed in double quotes\n"
                     "  %6 <filename>\tThe file that contains status update message, used when -m is not specified\n"
                     "  %7\t\tRead message from standard in\n";


class Helper : public QObject
{
    Q_OBJECT

public:
    Helper() : QObject(), tweeter_(this), waitForMsg_(false), msg_(QString()) {}

public slots:

    void processArgs() {

        QStringList argList = qApp->arguments();

        QByteArray help = QString(USAGE).arg(OPT_OAUTH,
                                             OPT_XAUTH,
                                             OPT_USERNAME,
                                             OPT_PASSWORD,
                                             OPT_STATUS,
                                             OPT_STATUS_FILE,
                                             OPT_STATUS_STDIN).toLatin1();

        const char* helpText = help.constData();

        connect(&tweeter_, SIGNAL(linkingFailed()),
                this, SLOT(onLinkingFailed()));
        connect(&tweeter_, SIGNAL(linkingSucceeded()),
                this, SLOT(onLinkingSucceeded()));

        if (argList.contains(OPT_OAUTH)) {
            if (argList.contains(OPT_STATUS)) {
                waitForMsg_ = true;
                msg_ = argList.at(argList.indexOf(OPT_STATUS) + 1);
            } else if (argList.contains(OPT_STATUS_FILE)) {
                waitForMsg_ = true;
                QFile msgFile(argList.at(argList.indexOf(OPT_STATUS_FILE) + 1));
                QTextStream in(&msgFile);
                if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
                    msg_ = in.readAll();
                else {
                    qDebug() << "\nError: File is not readable text format!";
                    qDebug() << helpText;
                    msgFile.close();
                    qApp->exit(1);
                }
                msgFile.close();
            } else if (argList.contains(OPT_STATUS_STDIN)) {
                qDebug() << "Please type the message to tweet:\n";
                waitForMsg_ = true;
                QTextStream in(stdin);
                msg_ = in.readAll();  // This is how you read the entire line
            }
            // Start OAuth
            tweeter_.doOAuth();
        } else if (argList.contains(OPT_XAUTH)) {
            if (!(argList.contains(OPT_USERNAME) && argList.contains(OPT_PASSWORD))) {
                qDebug() << "\nError: Username or Password missing!";
                qDebug() << helpText;
                qApp->exit(1);
            }

            QString username = argList.at(argList.indexOf(OPT_USERNAME) + 1);
            QString password = argList.at(argList.indexOf(OPT_PASSWORD) + 1);

            if (argList.contains(OPT_STATUS)) {
                waitForMsg_ = true;
                msg_ = argList.at(argList.indexOf(OPT_STATUS) + 1);
            } else if (argList.contains(OPT_STATUS_FILE)) {
                waitForMsg_ = true;
                QFile msgFile(argList.at(argList.indexOf(OPT_STATUS_FILE) + 1));
                QTextStream in(&msgFile);
                if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
                    msg_ = in.readAll();
                else {
                    qDebug() << "\nError: File is not readable text format!";
                    qDebug() << helpText;
                    msgFile.close();
                    qApp->exit(1);
                }
                msgFile.close();
            } else if (argList.contains(OPT_STATUS_STDIN)) {
                qDebug() << "Please type the message to tweet:\n";
                waitForMsg_ = true;
                QTextStream in(stdin);
                msg_ = in.readAll();  // This is how you read the entire line
            }
            // Start XAuth
            tweeter_.doXAuth(username, password);
        } else if (argList.contains(OPT_STATUS)) {
            QString statusMessage = argList.at(argList.indexOf(OPT_STATUS) + 1);
            postStatusUpdate(statusMessage);
        } else if (argList.contains(OPT_STATUS_FILE)) {
            waitForMsg_ = true;
            QFile msgFile(argList.at(argList.indexOf(OPT_STATUS_FILE) + 1));
            QTextStream in(&msgFile);
            if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
                msg_ = in.readAll();
            else {
                qDebug() << "\nError: File is not readable text format!";
                qDebug() << helpText;
                msgFile.close();
                qApp->exit(1);
            }
            msgFile.close();
        } else if (argList.contains(OPT_STATUS_STDIN)) {
            qDebug() << "Please type the message to tweet:\n";
            waitForMsg_ = true;
            QTextStream in(stdin);
            msg_ = in.readAll();  // This is how you read the entire line
        } else {
            qDebug() << helpText;
            //qDebug() << "Please type the message to tweet:\n";
            qApp->exit(1);
        }
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        if (waitForMsg_) {
            postStatusUpdate(msg_);
        } else {
            qApp->quit();
        }
    }

private slots:

    void postStatusUpdate(const QString& msg) {
        connect(&tweeter_, SIGNAL(statusPosted()),
                qApp, SLOT(quit()));
        tweeter_.postStatusUpdate(msg);
    }

private:
    Tweeter tweeter_;
    bool waitForMsg_;
    QString msg_;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Helper helper;
    QTimer::singleShot(0, &helper, SLOT(processArgs()));

    return a.exec();
}

#include "main.moc"

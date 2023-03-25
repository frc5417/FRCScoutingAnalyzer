#ifndef H_UTIL
#define H_UTIL

#include <QString>
#include <QStringList>

class Util {

public:

static bool matchNumLessThan(QString v1, QString v2)
{
    return Util::findDouble(v1, "mn") < Util::findDouble(v1, "mn");
}

static double findDouble(QString matchData, QString key) {
    QString val = Util::findString(matchData, key);
    if (val.isEmpty()) val = "0";
    return val.toDouble();
}

static QString findString(QString matchData, QString key) {
    QStringList args = matchData.split(",");
    for (int i = 0; i < args.size(); i++)
    {
        if (args[i].startsWith(key + "=")) {
            return args[i].replace(key + "=", "");
        }
    }
    return "";
}

static double findPointValue(QStringList datasetData, QString key) {
    for (int i = 0; i < datasetData.length(); i++) {
	if (datasetData[i].split("|")[0] == key) {
		return datasetData[i].split("|")[3].toDouble();
	}
    }

    return 0.0;
}

};

#endif

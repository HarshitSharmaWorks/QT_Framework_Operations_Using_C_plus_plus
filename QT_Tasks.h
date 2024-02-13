#ifndef QT_TASKS_H
#define QT_TASKS_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

struct ColumnNames_CSV {
    string time;
    string latitude;
    string longitude;
    string course;
    string kts;
    string mph;
    string altitudeFeet;
    string reportingFacility;

    ColumnNames_CSV(string t, string lat, string lon, string c, string k, string m, string alt, string rf) :
        time(t), latitude(lat), longitude(lon), course(c), kts(k), mph(m), altitudeFeet(alt), reportingFacility(rf) {}

    void display() const {
        cout << "\nTime: " << time << endl;
        cout << "Latitude: " << latitude << endl;
        cout << "Longitude: " << longitude << endl;
        cout << "Course: " << course << endl;
        cout << "KTS: " << kts << endl;
        cout << "MPH: " << mph << endl;
        cout << "AltitudeFeet: " << altitudeFeet << endl;
        cout << "Reporting Facility : " << reportingFacility << endl;
    }

    json toJSON() const
    {
        json jsonObject;
        jsonObject["Time"] = time;
        jsonObject["Latitude"] = latitude;
        jsonObject["Longitude"] = longitude;
        jsonObject["Course"] = course;
        jsonObject["Kts"] = kts;
        jsonObject["MPH"] = mph;
        jsonObject["AltitudeFeet"] = altitudeFeet;
        jsonObject["ReportingFacility"] = reportingFacility;
        return jsonObject;
    }
};
class QT_Tasks : public QWidget {
    Q_OBJECT

public:
    QT_Tasks(QWidget* parent = nullptr);

private slots:
    void on_locationButton_clicked();
    void on_readCSVButton_clicked();
    void on_sortCSVButton_clicked();
    void on_printJSONFileButton_clicked(); 
    void on_writeDatabaseButton_clicked();
    void on_displaySortedDataButton_clicked();

private:
    QPushButton* locationButton;
    QPushButton* readCSVButton;
    QPushButton* sortCSVButton;
    QPushButton* printJSONFileButton; 
    QPushButton* writeDatabaseButton;
    QLineEdit* filePathLineEdit;
    QString csvFilePath; 
    vector<ColumnNames_CSV> columnNames;
    QPushButton* displaySortedDataButton;
};

#endif // QT_TASKS_H

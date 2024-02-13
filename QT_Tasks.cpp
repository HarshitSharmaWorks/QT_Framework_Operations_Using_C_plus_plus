#include "QT_Tasks.h"
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <Vector>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QInputDialog>
#include <QGuiApplication>
#include <QPushButton>
#include <QStandardItemModel> 
#include <QTableView>

using namespace std;
using json = nlohmann::json;
using namespace sql;



bool Sort_By_Time(const ColumnNames_CSV& a, const ColumnNames_CSV& b) {
    string t1 = a.time;
    string t2 = b.time;

    if (t1.substr(t1.length() - 2) == "PM" && t1.substr(0, 2) != "12") {
        int hour = stoi(t1.substr(0, 2));
        hour += 12;
        t1.replace(0, 2, to_string(hour));
    }
    else if (t1.substr(t1.length() - 2) == "AM" && t1.substr(0, 2) == "12") {
        t1.replace(0, 2, "00");
    }

    if (t2.substr(t2.length() - 2) == "PM" && t2.substr(0, 2) != "12") {
        int hour = stoi(t2.substr(0, 2));
        hour += 12;
        t2.replace(0, 2, to_string(hour));
    }
    else if (t2.substr(t2.length() - 2) == "AM" && t2.substr(0, 2) == "12") {
        t2.replace(0, 2, "00");
    }

    return t1 < t2;
}

QT_Tasks::QT_Tasks(QWidget* parent) : QWidget(parent) {
    locationButton = new QPushButton("Choose File");
    readCSVButton = new QPushButton("Read CSV");
    sortCSVButton = new QPushButton("Sort CSV");
    printJSONFileButton = new QPushButton("Print JSON File");
    writeDatabaseButton = new QPushButton("Write to Database");
    displaySortedDataButton = new QPushButton("Display Sorted Data");


    printJSONFileButton->setEnabled(false);
    writeDatabaseButton->setEnabled(false);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(locationButton);
    layout->addWidget(readCSVButton);
    layout->addWidget(sortCSVButton);
    layout->addWidget(printJSONFileButton);
    layout->addWidget(writeDatabaseButton);
    layout->addWidget(displaySortedDataButton);


    connect(locationButton, &QPushButton::clicked, this, &QT_Tasks::on_locationButton_clicked);
    connect(readCSVButton, &QPushButton::clicked, this, &QT_Tasks::on_readCSVButton_clicked);
    connect(sortCSVButton, &QPushButton::clicked, this, &QT_Tasks::on_sortCSVButton_clicked);
    connect(printJSONFileButton, &QPushButton::clicked, this, &QT_Tasks::on_printJSONFileButton_clicked);
    connect(writeDatabaseButton, &QPushButton::clicked, this, &QT_Tasks::on_writeDatabaseButton_clicked);
    connect(displaySortedDataButton, &QPushButton::clicked, this, &QT_Tasks::on_displaySortedDataButton_clicked);

}

void QT_Tasks::on_locationButton_clicked() {
    csvFilePath = QFileDialog::getOpenFileName(this, "Open CSV File", "", "CSV Files (*.csv)");
    if (!csvFilePath.isEmpty()) {
        qDebug() << "File selected: " << csvFilePath;
    }
    QMessageBox::information(this, "", "File Selected Successfuly");

}

void QT_Tasks::on_readCSVButton_clicked() {
    if (csvFilePath.isEmpty()) {
        qDebug() << "No File";
        return;
    }

    ifstream ReadFile(csvFilePath.toStdString());
    if (!ReadFile.is_open()) {
        qDebug() << "Not Opened";
        return;
    }

    QTextEdit* outputWindow = new QTextEdit();
    outputWindow->setWindowTitle("CSV File Data");

    QString fileData;
    string row;
    while (getline(ReadFile, row)) {
        fileData.append(QString::fromStdString(row) + "\n");
    }

    outputWindow->setText(fileData);
    outputWindow->showMaximized();

    ReadFile.close();
}

void QT_Tasks::on_sortCSVButton_clicked() {
    if (csvFilePath.isEmpty()) {
        qDebug() << "No CSV file selected";
        return;
    }

    ifstream ReadFile(csvFilePath.toStdString());
    if (!ReadFile.is_open()) {
        qDebug() << "Not Opened";
        return;
    }

    columnNames.clear();
    string row;
    getline(ReadFile, row);
    while (getline(ReadFile, row)) {
        string time, latitude, longitude, course, kts, mph, altitudeFeet, reportingFacility;
        stringstream inputString(row);
        getline(inputString, time, ',');
        getline(inputString, latitude, ',');
        getline(inputString, longitude, ',');
        getline(inputString, course, ',');
        getline(inputString, kts, ',');
        getline(inputString, mph, ',');
        getline(inputString, altitudeFeet, ',');
        getline(inputString, reportingFacility, ',');
        ColumnNames_CSV data(time, latitude, longitude, course, kts, mph, altitudeFeet, reportingFacility);
        columnNames.push_back(data);
    }

    sort(columnNames.begin(), columnNames.end(), Sort_By_Time);
    printJSONFileButton->setEnabled(true);
    writeDatabaseButton->setEnabled(true);
    QMessageBox::information(this, "", "Data Sorted successfully.");

    QTextEdit* sortedDataWindow = new QTextEdit();
    sortedDataWindow->setWindowTitle("Sorted CSV Data");

    QString sortedDataString;
    for (const auto& data : columnNames) {
        sortedDataString.append(QString::fromStdString(data.time) + ", " +
            QString::fromStdString(data.latitude) + ", " +
            QString::fromStdString(data.longitude) + ", " +
            QString::fromStdString(data.course) + ", " +
            QString::fromStdString(data.kts) + ", " +
            QString::fromStdString(data.mph) + ", " +
            QString::fromStdString(data.altitudeFeet) + ", " +
            QString::fromStdString(data.reportingFacility) + "\n");
    }

    sortedDataWindow->setText(sortedDataString);
    sortedDataWindow->showMaximized();

}

void QT_Tasks::on_printJSONFileButton_clicked() {
    json jsonArray;
    for (const auto& data2 : columnNames) {
        jsonArray.push_back(data2.toJSON());
    }

    QString jsonFilePath = QFileDialog::getSaveFileName(this, "Save JSON File", "", "JSON Files (*.json)");
    if (!jsonFilePath.isEmpty()) {
        ofstream jsonFile(jsonFilePath.toStdString());
        jsonFile << jsonArray.dump(4);
        jsonFile.close();
        QTextEdit* outputWindow = new QTextEdit();
        outputWindow->setWindowTitle("Generated JSON Data");
        outputWindow->setText(QString::fromStdString(jsonArray.dump(4)));
        outputWindow->show();
    }
}

void QT_Tasks::on_writeDatabaseButton_clicked() {
    QString server = QInputDialog::getText(this, "Database Connection", "Enter the MySQL server address (e.g., tcp://localhost:3306): ");
    QString username = QInputDialog::getText(this, "Database Connection", "Enter the MySQL username: ");
    QString password = QInputDialog::getText(this, "Database Connection", "Enter the MySQL password: ");
    QString database = QInputDialog::getText(this, "Database Connection", "Enter the MySQL database name: ");

    try {
        Driver* driver = get_driver_instance();
        Connection* con = driver->connect(server.toStdString(), username.toStdString(), password.toStdString());
        con->setSchema(database.toStdString());
        Statement* stmt = con->createStatement();
        stmt->execute("CREATE TABLE IF NOT EXISTS QT_table(Time VARCHAR(20), Latitude VARCHAR(20), Longitude VARCHAR(20), Course VARCHAR(20), kts VARCHAR(20), mph VARCHAR(20), AltitudeFeet VARCHAR(20), ReportingFacility VARCHAR(100))");
        delete stmt;
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO FlightData(Time, Latitude, Longitude, Course, kts, mph, AltitudeFeet, ReportingFacility) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        for (const auto& data : columnNames) {
            pstmt->setString(1, data.time);
            pstmt->setString(2, data.latitude);
            pstmt->setString(3, data.longitude);
            pstmt->setString(4, data.course);
            pstmt->setString(5, data.kts);
            pstmt->setString(6, data.mph);
            pstmt->setString(7, data.altitudeFeet);
            pstmt->setString(8, data.reportingFacility);
            pstmt->executeUpdate();
        }
        qDebug() << "Data stored successfully to the database";
        QMessageBox::information(this, "", "Data Stored successfully.");
    }
    catch (SQLException& e) {
        qDebug() << "Error: " << e.what();
    }
}
void QT_Tasks::on_displaySortedDataButton_clicked() {
    QWidget* displayWindow = new QWidget();
    QVBoxLayout* displayLayout = new QVBoxLayout(displayWindow);
    QTableView* tableView = new QTableView();
    QStandardItemModel* model = new QStandardItemModel(columnNames.size(), 8, displayWindow);
    model->setHorizontalHeaderLabels(QStringList() << "Time" << "Latitude" << "Longitude" << "Course" << "kts" << "mph" << "AltitudeFeet" << "ReportingFacility");
    for (int row = 0; row < columnNames.size(); ++row) {
        const auto& data = columnNames[row];
        model->setItem(row, 0, new QStandardItem(QString::fromStdString(data.time)));
        model->setItem(row, 1, new QStandardItem(QString::fromStdString(data.latitude)));
        model->setItem(row, 2, new QStandardItem(QString::fromStdString(data.longitude)));
        model->setItem(row, 3, new QStandardItem(QString::fromStdString(data.course)));
        model->setItem(row, 4, new QStandardItem(QString::fromStdString(data.kts)));
        model->setItem(row, 5, new QStandardItem(QString::fromStdString(data.mph)));
        model->setItem(row, 6, new QStandardItem(QString::fromStdString(data.altitudeFeet)));
        model->setItem(row, 7, new QStandardItem(QString::fromStdString(data.reportingFacility)));
    }

    tableView->setModel(model);
    displayLayout->addWidget(tableView);
    displayWindow->setWindowTitle("Sorted Data");
    displayWindow->showMaximized();
}
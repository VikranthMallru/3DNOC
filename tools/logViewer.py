
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, QPushButton, QAction, QLineEdit, QMessageBox, QComboBox
import threading
import sys
import numpy as np


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        colNum = 0
        ColData = ["Clock","VA","TransID","PA","Core","Source","Req State","Resp State","Req Type","Resp Type","Probe Type","C2X","CL","IL","PF", "PacketPtr",
                    "BA","PackID","Tag","Index","Bank","Way","ReqPort","IssuePort","RCS","MCS","LUS","WA","ReqT","DecT","IssT","FwtT","MissRT","RespT","EvictT","WBT","Drop","VictimPkt","QCred","SHRCred",
                    "PackID","Channel","SrcL","DestL","SrcDir","DestDir",
                    "CurrL","LSD","LRD","OnRampT","OffRampT","Message"]
        self.MainWindow = MainWindow
        MainWindow.setObjectName("MainWindow")
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.tableWidget = QtWidgets.QTableWidget(self.centralwidget)
        self.tableWidget.setGeometry(QtCore.QRect(0, 100,1850, 800))
        self.tableWidget.setObjectName("tableWidget")
        self.tableWidget.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.combobox = QComboBox(self.centralwidget)
        self.combobox.setGeometry(QtCore.QRect(10,10,250, 35))
        self.combobox.addItems(ColData)
        self.textbox = QLineEdit('',self.centralwidget)
        self.textbox.setGeometry(QtCore.QRect(260, 10,100, 35))
        self.button = QPushButton('Filter',self.centralwidget)
        self.button.setGeometry(QtCore.QRect(400, 10,100, 35))
        self.button.clicked.connect(self.Filter)

        self.buttonColumn = QPushButton('Hide',self.centralwidget)
        self.buttonColumn.setGeometry(QtCore.QRect(600, 10,100, 35))
        self.buttonColumn.clicked.connect(self.hideColumn)

        numCols = len(ColData)
        self.tableWidget.setColumnCount(numCols)
        for col in ColData:
            item = QtWidgets.QTableWidgetItem()
            item.setText(QtCore.QCoreApplication.translate("MainWindow", ColData[colNum]))
            self.tableWidget.setVerticalHeaderItem(colNum, item)
            colNum += 1
        self.tableWidget.setHorizontalHeaderLabels(ColData)
        self.tableWidget.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOn)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)


        MainWindow.setWindowTitle(QtCore.QCoreApplication.translate("MainWindow", "LogViewer"))
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
        MainWindow.showMaximized()
        MainWindow.show()

    def hideColumn(self):
        self.tableWidget.setColumnHidden(self.combobox.currentIndex(),True)

    def Filter(self):
        textBoxValue = self.textbox.text()
        self.textbox.setText("")
        row = 0
        inputRow = 0
        self.filteredRecord = []
        for logRecordItem in self.logRecord:
            if logRecordItem[self.combobox.currentIndex()] == textBoxValue:
                self.filteredRecord.append(self.logRecord[inputRow])
                row = row + 1
            inputRow = inputRow + 1
        self.viewData(self.MainWindow)
    

    def loadData(self):
        logFile = open('/mnt/OS/Study/NOC Cache Project/3-DNoC-main/TransactionLog', 'r')
        row = 0
        self.logRecord = []
        while(True):
            line = logFile.readline()
            if not line:
                break
            logLine = line.split("M:")
            transDetails = logLine[0].split("\t")
            del transDetails[-1]
            cachePacketLine = logLine[1].split("P:")
            if len(cachePacketLine) == 2:
                pktDetails = cachePacketLine[0].split("\t")
                del pktDetails[-1]
                nptDetails = ["","","","","","","","","","",""]
                message = cachePacketLine[1]                
            else:
                networkPacketLine = logLine[1].split("N:")
                if len(networkPacketLine) == 2:
                    pktDetails = ["","","","","","","","","","","","","","","","","","","","","","","",""]
                    nptDetails = networkPacketLine[0].split("\t")
                    del nptDetails[-1]
                    message = networkPacketLine[1] 
                else:
                    pktDetails = ["","","","","","","","","","","","","","","","","","","","","","","","",""]
                    nptDetails = ["","","","","","","","","",""]
                    message = logLine[1]
            colData = np.concatenate((transDetails,pktDetails,nptDetails,message), axis=None)
            self.logRecord.append(colData)
            row += 1
        self.filteredRecord = self.logRecord
        self.viewData(self.MainWindow)
        logFile.close()

    def viewData(self,MainWindow):
        row = 0
        self.tableWidget.setRowCount(0)
        for logRecordItem in self.filteredRecord:
            self.tableWidget.insertRow(row)
            __sortingEnabled = self.tableWidget.isSortingEnabled()
            self.tableWidget.setSortingEnabled(False)
            colNums = 0
            colData = self.filteredRecord[row]
            for col in colData:
                item = self.tableWidget.setItem(row, colNums, QtWidgets.QTableWidgetItem(colData[colNums]))
                colNums += 1
            self.tableWidget.setSortingEnabled(__sortingEnabled)
            row = row + 1

        self.tableWidget.resizeColumnsToContents();

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    logRecord = ui.loadData()
    sys.exit(app.exec_())

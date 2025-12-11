from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, QPushButton, QAction, QLineEdit, QMessageBox, QComboBox
import threading
import sys
import re
import numpy as np
import os
from pathlib import Path
from sys import platform
import shlex
from subprocess import run,PIPE

log_file_path = '/home/debiprasanna/ucc/src/TransactionLog'

class Ui_MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

    def setupUi(self):
        colNum = 0
        ColData = ["Clock","VA","TransID","PA","Core","Source","Req State","Resp State","Req Type","Resp Type","Probe Type","C2X","CL","IL","PF", "ReplacedTag","ReplacedWay", 
                    "BA","PackID","Tag","Index","Bank","Way","ReqPort","IssuePort","RCS","MCS","LUS","WA","ReqT","DecT","IssT","FwtT","MissRT","RespT","EvictT","WBT","Drop","VictimPkt","QCred","SHRCred",
                    "BA","PackID","Tag","Index","Way","LUS",
                    "PackID","Channel","SrcL","DestL","SrcDir","DestDir","CurrL","LSD","LRD","OnRampT","OffRampT",
                    "Message"]
        self.setObjectName("MainWindow")
        self.centralwidget = QtWidgets.QWidget(self)
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

        self.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(self)
        self.menubar.setObjectName("menubar")
        self.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(self)
        self.statusbar.setObjectName("statusbar")
        self.setStatusBar(self.statusbar)

        self.setWindowTitle(QtCore.QCoreApplication.translate("MainWindow", "LogViewer"))
        QtCore.QMetaObject.connectSlotsByName(self)

    def hideColumn(self):
        self.tableWidget.setColumnHidden(self.combobox.currentIndex(),True)

    def Filter(self):
        textBoxValue = self.textbox.text()
        row = 0
        inputRow = 0
        self.filteredRecord = []
        for logRecordItem in self.logRecord:
            if logRecordItem[self.combobox.currentIndex()] == textBoxValue:
                self.filteredRecord.append(self.logRecord[inputRow])
                row = row + 1
            inputRow = inputRow + 1
        self.viewData()

    def loadData(self, filePath = None):

        logFile = open(log_file_path, "r")
        row = 0
        self.logRecord = []
        while(True):
            line = logFile.readline()
            if not line:
                break
            completeLogLine = line.split("C:")
            logLineWithTrans = completeLogLine[0]
            logLineWithCacheDirNetMsg = completeLogLine[1]
            logLineWithCacheDirNetMsgArr = logLineWithCacheDirNetMsg.split("D:")
            logLineWithCache = logLineWithCacheDirNetMsgArr[0]
            logLineWithDirNetMsg = logLineWithCacheDirNetMsgArr[1]
            logLineWithDirNetMsgArr = logLineWithDirNetMsg.split("N:")
            logLineWithDir = logLineWithDirNetMsgArr[0]
            logLineWithNetMsg = logLineWithDirNetMsgArr[1]
            logLineWithNetMsgArr = logLineWithNetMsg.split("M:")
            logLineWithNet = logLineWithNetMsgArr[0]
            logLineWithMsg = logLineWithNetMsgArr[1]

            transDetails = logLineWithTrans.split("\t")
            del transDetails[-1]

            cacheDetails = logLineWithCache.split("\t")
            del cacheDetails[-1]

            dirDetails = logLineWithDir.split("\t")
            del dirDetails[-1]

            networkDetails = logLineWithNet.split("\t")
            
            if len(cacheDetails) == 0:
                cacheDetails = ["","","","","","","","","","","","","","","","","","","","","","",""]

            if len(dirDetails) == 0:
                dirDetails = ["","","","","",""]
            
            if len(networkDetails) == 0:
                networkDetails = ["","","","","","","","","",""]

            del networkDetails[-1]
            colData = np.concatenate((transDetails,cacheDetails,dirDetails,networkDetails,logLineWithMsg), axis=None)
            self.logRecord.append(colData)
            row += 1
        self.filteredRecord = self.logRecord
        self.viewData()
        logFile.close()


    def applyFilter(self, txt):
        self.combobox.setCurrentIndex(3)
        self.textbox.setText(str(txt))
        self.Filter()

    def viewData(self):
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

        self.tableWidget.resizeColumnsToContents()

def displayPacket(pkt_id):  
    ui = Ui_MainWindow()
    ui.setupUi()
    logRecord = ui.loadData()
    ui.viewData()
    ui.applyFilter(pkt_id)
    return ui

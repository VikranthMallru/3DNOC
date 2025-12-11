from PyQt5 import uic
from PyQt5.QtWidgets import *
from helpers import parse
from helpers import packetDetail
from helpers import blockDetail
import math
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, QPushButton, QAction, QLineEdit, QMessageBox, QComboBox
import threading
import sys
import numpy as np
from pathlib import Path

queues_qt = {
    "ReqCtrl":{
        "RDREQ[0]" : "RDREQ",
        "WRREQ[0]" : "WRREQ",
        "PF[0]" : "PF",
        "PBR_REQ[0]": "PBR_REQ",
        "PBR_RSP[0]" : "PBR_RSP",
        "FILL[0]" : "FILL",
    },
    "RespCtrl":{
        "RDRESP[0]":"RD_RESP",
        "MISS[0]" : "MISS",
        "VICTIM[0]": "VICTIM",
        "PRB_REQ[0]": "PRB_REQ",
        "PBR_RESP[0]": "PBR_RESP",
        "PFTRAIN[0]": "PFTRAIN"
    },
    "SHR":{
        "RD_SHR": "RD_SHR",
        "VIC_BUF": "VIC_BUF",
        "PRB_SHR": "PRB_SHR"
    },
    "IssCtrl":{
        "0":"IssCtrl0",
        "1" : "IssCtrl1",
        "2": "IssCtrl2",
        "3": "IssCtrl3"
    },
    "MDCtrl[0]":{
        "DRD":"DRD0",
        "DWR":"DWR0",
        "TWR": "TWR0",
        "MDR":"MDR0"
    },
    "MDCtrl[1]":{
        "DRD":"DRD1",
        "DWR":"DWR1",
        "TWR": "TWR1",
        "MDR":"MDR1"
    },
    "MDCtrl[2]":{
        "DRD":"DRD2",
        "DWR":"DWR2",
        "TWR": "TWR2",
        "MDR":"MDR2"
    },
    "MDCtrl[3]":{
        "DRD":"DRD3",
        "DWR":"DWR3",
        "TWR": "TWR3",
        "MDR":"MDR3"
    }
}

data_sample = [{'ReqCtrl': {'RDREQ[0]': {'M': 16, 'A': 6, 'C': 10, 'Packets': ['10063', '10064', '10065', '10087', '10088', '10089', '10090', '10091', '10092', '10093', '', '', '', '', '', '']}, 'WRREQ[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PF[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PBR_REQ[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PBR_RSP[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'FILL[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}}, 'IssCtrl': {'0': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '1': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '2': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '3': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[0]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 0, 'C': 1, 'Packets': ['10067']}}, 'MDCtrl[1]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[2]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[3]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 0, 'C': 1, 'Packets': ['10048']}}, 'RespCtrl': {'RDRESP[0]': {'M': 16, 'A': 14, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'MISS[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'VICTIM[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PRB_REQ[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PBR_RESP[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PFTRAIN[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}}, 'SHR': {'RD_SHR': {'M': 32, 'A': 2, 'C': 30, 'Packets': ['10049', '10050', '10051', '10052', '10053', '10054', '10055', '10056', '10057', '10058', '10059', '10060', '10061', '10062', '10068', '10069', '10070', '10072', '10073', '10074', '10075', '10076', '10077', '10078', '10080', '10081', '10082', '10083', '10084', '10085', '', '']}, 'VIC_BUF': {'M': 32, 'A': 32, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PRB_SHR': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}}}, {'ReqCtrl': {'RDREQ[0]': {'M': 16, 'A': 6, 'C': 10, 'Packets': ['10063', '10064', '10065', '10087', '10088', '10089', '10090', '10091', '10092', '10093', '', '', '', '', '', '']}, 'WRREQ[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PF[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PBR_REQ[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PBR_RSP[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'FILL[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}}, 'IssCtrl': {'0': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '1': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '2': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, '3': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[0]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 0, 'C': 1, 'Packets': ['10067']}}, 'MDCtrl[1]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[2]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}}, 'MDCtrl[3]': {'DRD': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'DWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'TWR': {'M': 1, 'A': 1, 'C': 0, 'Packets': ['']}, 'MDR': {'M': 1, 'A': 0, 'C': 1, 'Packets': ['10048']}}, 'RespCtrl': {'RDRESP[0]': {'M': 16, 'A': 14, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'MISS[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'VICTIM[0]': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PRB_REQ[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PBR_RESP[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}, 'PFTRAIN[0]': {'M': 8, 'A': 8, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '']}}, 'SHR': {'RD_SHR': {'M': 32, 'A': 2, 'C': 30, 'Packets': ['10049', '10050', '10051', '10052', '10053', '10054', '10055', '10056', '10057', '10058', '10059', '10060', '10061', '10062', '10068', '10069', '10070', '10072', '10073', '10074', '10075', '10076', '10077', '10078', '10080', '10081', '10082', '10083', '10084', '10085', '', '']}, 'VIC_BUF': {'M': 32, 'A': 32, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}, 'PRB_SHR': {'M': 16, 'A': 16, 'C': 0, 'Packets': ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']}}}]
enable_npy = False
npy_path = 'L1DCache.npy'

class MyGUI(QMainWindow):

    def __init__(self):
        super(MyGUI, self).__init__()
        uic.loadUi("qtDesigner.ui", self)
        self.show()
        
        if enable_npy:
            path = Path(npy_path)
            if path.is_file():
                self.data = np.load(npy_path, allow_pickle = True)
            else:
                # self.data = data_sample
                self.data = parse.parse_data()
                np.save(npy_path, self.data)

        else:
            self.data = parse.parse_data()
        
        self.queues = queues_qt

        self.cycle = 0

        self.total_cycles.setText(str(len(self.data)-1))
        self.current_cycle.setText("No started")
        n = 100
        self.progressBar.setMaximum(100 * n)
        self.progressBar.setValue(math.floor(0 * n))
        self.progressBar.setFormat("%.02f %%" % 0)

        self.Start.setEnabled(True)
        self.Start.clicked.connect(self.startDisplay)
        self.startButtonsEnabled = False
        self.packetButtonsConnected = False

        self.goToCycleBtn.clicked.connect(
            lambda: self.jumpCycle(self.goToCycle.text()))

    def displayPacket(self):
        setattr(self, self.sender().text(),
                packetDetail.displayPacket(self.sender().text()))
        getattr(self, self.sender().text()).showMaximized()
        getattr(self, self.sender().text()).show()

        #setattr(self, self.sender().text(),
        #        blockDetail.displayAddress())
        #getattr(self, self.sender().text()).showMaximized()
        #getattr(self, self.sender().text()).show()
        

    def displayCycle(self, cycle):
        if cycle < 0:
            self.cycle = len(self.data)+cycle
            cycle = self.cycle
        elif cycle >= len(self.data):
            self.cycle = cycle % len(self.data)
            cycle = self.cycle
        else:
            self.cycle = cycle

        self.current_cycle.setText(str(self.cycle))

        value = float(self.cycle) * 100 / float(len(self.data)-1)
        value = round(value, 2)
        self.progressBar.setValue(math.floor(value * 100))
        self.progressBar.setFormat("%.02f %%" % value)

        all_queues = self.data[cycle]

        for queue_type in all_queues.keys():
            token = queue_type
            queues = all_queues[token]
            for queue in queues.keys():
                queue_details = queues[queue]
                m = queue_details["M"]
                a = queue_details["A"]
                c = queue_details["C"]
                packets = queue_details["Packets"]

                prefix = self.queues[token][queue]

                label_m = prefix+"_"+"M"
                label_a = prefix+"_"+"A"
                label_c = prefix+"_"+"C"

                try: 
                    x = getattr(self, label_m)
                    x.setText(str(m))
                except:
                    print("Some error in ", label_m)
                
                try: 
                    x = getattr(self, label_a)
                    x.setText(str(a))
                except:
                    print("Some error in ", label_a)
                
                try: 
                    x = getattr(self, label_c)
                    x.setText(str(c))
                except:
                    print("Some error in ", label_c)

                for idx, pkt in enumerate(packets):
                    buttonName = prefix+"_"+str(idx+1)

                    try:
                        x = getattr(self, buttonName)
                        x.setText(str(pkt))

                        if not self.packetButtonsConnected:
                                x.clicked.connect(lambda: self.displayPacket())

                        if pkt != "":
                            x.setEnabled(True)
                            
                        else:
                            x.setEnabled(False)
                    except:
                        print("Some error in ", buttonName)


        self.packetButtonsConnected = True

    def enableSupportButtons(self):
        self.startButtonsEnabled = True

        self.Next.setEnabled(True)
        self.Next.clicked.connect(lambda: self.nextCycle(1))

        self.Next_5.setEnabled(True)
        self.Next_5.clicked.connect(lambda: self.nextCycle(5))

        self.Next_10.setEnabled(True)
        self.Next_10.clicked.connect(lambda: self.nextCycle(10))

        self.Previous.setEnabled(True)
        self.Previous.clicked.connect(lambda: self.prevCycle(1))

        self.Previous_5.setEnabled(True)
        self.Previous_5.clicked.connect(lambda: self.prevCycle(5))

        self.Previous_10.setEnabled(True)
        self.Previous_10.clicked.connect(lambda: self.prevCycle(10))

    def startDisplay(self):
        self.displayCycle(0)
        if not self.startButtonsEnabled:
            self.enableSupportButtons()

    def nextCycle(self, inc=1):
        self.displayCycle(self.cycle+inc)

    def prevCycle(self, dec=1):
        self.displayCycle(self.cycle-dec)

    def jumpCycle(self, cycle=0):
        try:
            cycle = int(cycle)
        except:
            print("Please enter a valid cycle number")
            return

        self.displayCycle(cycle)


def main():
    app = QApplication([])
    window = MyGUI()
    app.exec_()


if __name__ == '__main__':
    main()

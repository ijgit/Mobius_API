import sys
from PyQt5.QtWidgets import QWidget, QApplication, QVBoxLayout, QHBoxLayout, QCalendarWidget
from PyQt5.QtGui import QPainter, QColor, QFont
from PyQt5.QtCore import Qt, QRectF, QDate
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import requests
import datetime as dt


plt.style.use(['seaborn']) # seaborn, ggplot


cse = "http://203.253.128.161:7579/Mobius"
ae = 'sch20171518'
payload = {}
headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SOrigin'
}


def getCreateTime(d=0, h=8):
    ct = dt.datetime.now() - dt.timedelta(days=d) - dt.timedelta(hours=h)
    stringformat = "%Y%m%dT%H0000"
    ct = ct.strftime(stringformat)
    return ct


def reqData(day, cnt):
    if type(cnt) != str:
        print('cnt param must be str')
        return

    crb = getCreateTime()

    if day == 1:
        url = f'{cse}/{ae}/{cnt}/la'
    else:
        cra = getCreateTime(day)
        url = f'{cse}/{ae}/{cnt}?rcn=4&cra={cra}&crb={crb}'

    res = requests.get(url, headers=headers, data=payload)
    res = res.json()  # .loads(res.text)
    res = res if day == 1 else res['m2m:rsp']
    return res


def dataRefining(day):
    log = dict()
    for i in range(day-1, -1, -1):
        k = getCreateTime(i)[4:-7]
        log[k] = 0

    data = reqData(day, "time_test")
    data = data['m2m:cin']

    for value in data:
        k = (value['con']).split(',')[0][4:]
        val = (value['con']).split(',')[1]
        val = int(val[0:2]) * 60 + int(val[2:])
        if k in log:
            log[k] += val

    return log


def dataRefining_calendar(day):
    data = reqData(day, "time_test")
    data = data['m2m:cin']

    log = dict()

    for value in data:
        k = (value['con']).split(',')[0]  # [4:]
        val = (value['con']).split(',')[1]
        val = int(val[0:2]) * 60 + int(val[2:])
        if k in log:
            log[k] += val
        else:
            log[k] = val
    return log


class App(QWidget):

    def __init__(self):
        super().__init__()

        self.fig = plt.Figure()
        self.canvas = FigureCanvas(self.fig)

        self.left = 100
        self.top = 100
        self.width = 1500
        self.height = 800

        self.layout = QHBoxLayout()
        self.setLayout(self.layout)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.init_ui()
        self.updateGraph()

    def init_ui(self):
        # graph layout
        graphLayout = QVBoxLayout()
        graphLayout.addWidget(self.canvas)

        # canvas Layout
        canvasLayout = QVBoxLayout()
        canvasLayout.addStretch(1)
        self.layout.addLayout(graphLayout)
        self.layout.addLayout(canvasLayout)

        # calendar
        cal = CalendarWidget(self)
        cal.setGridVisible(True)
        self.layout.addWidget(cal)

    def updateGraph(self):
        log1 = dataRefining(7)
        log2 = dataRefining(30)

        ax1 = self.fig.add_subplot(211)
        ax1.title.set_text('7-day game play time')
        ax1.plot(*zip(*log1.items()), marker='o')
        ax1.set_xlabel('date')
        ax1.set_ylabel('min')
        #plt.grid(color='w', linestyle='solid')

        ax2 = self.fig.add_subplot(212)
        ax2.title.set_text('30-day game play time')
        ax2.bar(*zip(*log2.items()))
        ax2.set_xticklabels(log2.keys(), rotation=70, fontsize='small')
        ax2.set_xlabel('date')
        ax2.set_ylabel('min')
        #plt.grid(color='w', linestyle='solid')

        self.fig.tight_layout()
        self.canvas.draw()


class CalendarWidget(QCalendarWidget):

    def paintCell(self, painter, rect, date):
        log = dataRefining_calendar(30)

        painter.setRenderHint(QPainter.Antialiasing, True)
        if date.toString("yyyyMMdd") in log.keys():

            painter.save()
            painter.drawRect(rect)
            painter.setPen(QColor(0, 0, 0))
            painter.setFont(QFont('Decorative', 8))
            painter.drawText(QRectF(rect), Qt.TextSingleLine, f"\n\t{str(date.day())}")

            painter.setPen(QColor(0, 9*16+9, 16*15+15))
            painter.setFont(QFont('Decorative', 10))
            painter.drawText(rect, Qt.AlignCenter, f"{str(log[date.toString('yyyyMMdd')])} min")
            painter.restore()
        else:
            QCalendarWidget.paintCell(self, painter, rect, date)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    ex.setWindowTitle('원익아 술 사')
    ex.show()
    sys.exit(app.exec_())
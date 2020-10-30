# yyyymmdd,HH:MM

import sys
from PyQt5.QtWidgets import QWidget, QApplication, QVBoxLayout, QHBoxLayout, QDesktopWidget
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import requests
import datetime as dt

plt.style.use('seaborn')

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
    cra = getCreateTime(day)
    url = f'{cse}/{ae}/{cnt}?rcn=4&cra={cra}&crb={crb}'

    res = requests.get(url, headers=headers, data=payload)
    res = res.json()  # .loads(res.text)
    return res['m2m:rsp']


def dataRefining(day):
    log = dict()
    for i in range(day - 1, -1, -1):
        k = getCreateTime(i)[4:-7]
        log[k] = 0

    data = reqData(day, "time_test")
    if 'm2m:cin' in data:
        data = data['m2m:cin']

    for value in data:
        con = (value['con']).split(',')
        d, t = con[0][4:], con[1]
        h = int(t.split(':')[0])
        m = int(t.split(':')[1])
        if d in log:
            log[d] += h * 60 + m

    return log


class App(QWidget):

    def __init__(self):
        super().__init__()

        self.fig = plt.Figure()
        self.canvas = FigureCanvas(self.fig)

        self.left = 100
        self.top = 100
        self.width = 700
        self.height = 800

        self.layout = QHBoxLayout()
        self.setLayout(self.layout)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.winCenter()
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

    def winCenter(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def updateGraph(self):
        log1 = dataRefining(1)
        log2 = dataRefining(7)
        log3 = dataRefining(30)

        play_time = list(log1.values())[0]

        group_names = ['other', 'play time']
        group_sizes = [24 * 60 - play_time, play_time]
        group_colors = ['lightskyblue', 'lightcoral']
        group_explodes = (0.1, 0)
        ax1 = self.fig.add_subplot(311)
        ax1.pie(group_sizes,
                explode=group_explodes,
                labels=group_names,
                colors=group_colors,
                autopct='%1.2f%%',
                shadow=True,
                startangle=60)
        ax1.axis('equal')
        ax1.title.set_text('Percentage of time spent playing games during the day')

        ax2 = self.fig.add_subplot(312)
        ax2.title.set_text('7-day game play time')
        ax2.plot(*zip(*log2.items()), marker='o')
        ax2.set_xlabel('date')
        ax2.set_ylabel('min')
        ax2.grid(True)

        ax3 = self.fig.add_subplot(313)
        ax3.title.set_text('30-day game play time')
        ax3.bar(*zip(*log3.items()))
        ax3.set_xticklabels(log3.keys(), rotation=70, fontsize='small')
        ax3.set_xlabel('date')
        ax3.set_ylabel('min')
        ax3.grid(True)

        self.fig.tight_layout()
        self.canvas.draw()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    ex.show()
    sys.exit(app.exec_())
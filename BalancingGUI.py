#!/usr/bin/env python


if False:
    import sip
    sip.settracemask(0x3f)

import sys
import serial
from PyQt4 import Qt
import PyQt4.Qwt5 as Qwt
from PyQt4.Qwt5.anynumpy import *

ser = serial.Serial('/dev/ttyUSB0', 57600)

class DataPlot(Qwt.QwtPlot):
    
    def __init__(self, *args):
        Qwt.QwtPlot.__init__(self, *args)

        self.setCanvasBackground(Qt.Qt.white)
        self.alignScales()

        # Initialize data
        self.x = arange(0.0, 1000.1, 0.5)
        self.y = zeros(len(self.x), Float)
        self.z = zeros(len(self.x), Float)

        self.setTitle("Inclination Angle")
        self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.BottomLegend);

        self.curveR = Qwt.QwtPlotCurve("Filtered Angle")
        self.curveR.attach(self)
        self.curveL = Qwt.QwtPlotCurve("Raw Angle")
        self.curveL.attach(self)

        self.curveL.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,
                                        Qt.QBrush(),
                                        Qt.QPen(Qt.Qt.yellow),
                                        Qt.QSize(7, 7)))

        self.curveR.setPen(Qt.QPen(Qt.Qt.red))
        self.curveL.setPen(Qt.QPen(Qt.Qt.blue))

        mY = Qwt.QwtPlotMarker()
        mY.setLabelAlignment(Qt.Qt.AlignRight | Qt.Qt.AlignTop)
        mY.setLineStyle(Qwt.QwtPlotMarker.HLine)
        mY.setYValue(0.0)
        mY.attach(self)

        self.setAxisTitle(Qwt.QwtPlot.xBottom, "Time")
        self.setAxisTitle(Qwt.QwtPlot.yLeft, "Degrees")
    
        self.startTimer(1)
        self.phase = 0.0

    # __init__()

    def alignScales(self):
        self.canvas().setFrameStyle(Qt.QFrame.Box | Qt.QFrame.Plain)
        self.canvas().setLineWidth(1)
        for i in range(Qwt.QwtPlot.axisCnt):
            scaleWidget = self.axisWidget(i)
            if scaleWidget:
                scaleWidget.setMargin(0)
            scaleDraw = self.axisScaleDraw(i)
            if scaleDraw:
                scaleDraw.enableComponent(
                    Qwt.QwtAbstractScaleDraw.Backbone, False)

    # alignScales()
    
    def timerEvent(self, e):

        #Read Serial and Plot
        self.y = concatenate((self.y[:1], self.y[:-1]), 1)
        self.y[0] = ser.readline() 

        self.curveR.setData(self.x, self.y)

        self.replot()
        self.phase += 0.01

	
    # timerEvent()

# class DataPlot

def make():
    demo = DataPlot()
    demo.resize(800, 600)
    demo.show()
    return demo


def main(args): 
    app = Qt.QApplication(args)
    demo = make()
    sys.exit(app.exec_())
    


if __name__ == '__main__':
    main(sys.argv)



#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <queue>
#include <iostream>

#include "display.hpp"

namespace mg {

  enum taskType {lineD, fillD, rectangleD, circleD, pointD, textD};
  QApplication* qapp;

  struct drawTask {
    taskType type;
    int x1, x2, y1, y2;
    color c;
    pen p;
    font f;
    std::string text;
  };



  inline QColor genQColor(color c) {
    return QColor(c.red, c.green, c.blue, c.alpha);
  }

  inline QPen genQPen(pen p) {
    QPen pen;
    pen.setColor(genQColor(p.col));
    pen.setWidth(p.thickness);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    return pen;
  }

  inline QFont genQFont(font f) {
    QFont font(QString::fromStdString(f.family));
    font.setPixelSize(f.size);
    font.setWeight(f.weight);
    return font;
  }

  class qtDisplay;
  class mainWindow;
  class qtContext;

  class mainWindow : public QMainWindow {
  private:
    QTimer *timer;
    QPainter *paint;
    qtDisplay* actualClient;
    std::queue<drawTask> tasks;
    renderer* rend;
    friend class qtContext;
    bool forceUpdate;
  protected:
    void pushTask(drawTask t) {
      tasks.push(t);
    }
    friend class qtContext;
  public:
    mainWindow(qtDisplay* client, renderer* r);
    void paintEvent(QPaintEvent *event);
  };

  class qtContext : public context {
    mainWindow* w;
    friend class mainWindow;
  public:
    qtContext(mainWindow* win, int wid, int hei);
    void drawLine(int x1, int y1, int x2, int y2, pen p);
    void fill(int x1, int x2, int y1, int y2, color c);
    void fill(color c);
    void drawRectangle(int x1, int y1, int x2, int y2, color c, pen p);
    void drawCircle(int x1, int y1, int x2, int y2, color c, pen p);
    void drawText(int x, int y, std::string t, font f, color c);
    void setPixel(int x, int y, color c);
  };

  class qtDisplay : public display {
    QApplication* a;
    mainWindow* w;
    qtContext* cont;
    friend class mainWindow;
  public:
    qtDisplay(QApplication* app, renderer* r, int width, int height);
    ~qtDisplay();
    void start();
    context* getContext();
  };

  mainWindow::mainWindow(qtDisplay* client, renderer* r) {
    actualClient = client;
    rend = r;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer->start(1000/60);
  }

  void mainWindow::paintEvent(QPaintEvent *event) {
    if(forceUpdate || rend->check()) rend->update(actualClient);
    else return;

    paint = new QPainter(this);

    while(!tasks.empty()) {
      drawTask t = tasks.front();
      tasks.pop();

      switch (t.type) {
        case lineD:
          paint->setPen(genQPen(t.p));
          paint->drawLine(t.x1, t.y1, t.x2, t.y2);
        break;
        case rectangleD:
          paint->setPen(genQPen(t.p));
          paint->setBrush(genQColor(t.c));
          paint->drawRect(t.x1, t.y1, t.x2 - t.x1, t.y2 - t.y1);
        break;
        case circleD:
          paint->setPen(genQPen(t.p));
          paint->setBrush(genQColor(t.c));
          paint->drawEllipse(t.x1, t.y1, t.x2 - t.x1, t.y2 - t.y1);
        break;
        case fillD:
          paint->fillRect(t.x1, t.y1, t.x2 - t.x1, t.y2 - t.y1, genQColor(t.c));
        break;
        case textD:
          paint->setFont(genQFont(t.f));
          paint->setPen(genQPen(t.p));
          paint->drawText(t.x1, t.y1, QString::fromStdString(t.text));
        break;
        case pointD:
          paint->setPen(genQPen(t.p));
          paint->drawPoint(t.x1, t.y1);
        break;
      }
    }
    forceUpdate = false;
    delete paint;
  }

  qtContext::qtContext(mainWindow* win, int wid, int hei) {
    w = win;
    width = wid;
    height = hei;
  }

  void qtContext::drawLine(int x1, int y1, int x2, int y2, pen p) {
    drawTask task;
    task.x1 = x1;
    task.x2 = x2;
    task.y1 = y1;
    task.y2 = y2;
    task.p = p;
    task.type = lineD;
    w->pushTask(task);
  }

  void qtContext::fill(int x1, int x2, int y1, int y2, color c) {
    drawTask task;
    task.x1 = x1;
    task.x2 = x2;
    task.y1 = y1;
    task.y2 = y2;
    task.c = c;
    task.type = fillD;
    w->pushTask(task);
  }

  void qtContext::fill(color c) {
    drawTask task;
    task.x1 = 0;
    task.x2 = width;
    task.y1 = 0;
    task.y2 = height;
    task.c = c;
    task.type = fillD;
    w->pushTask(task);
  }

  void qtContext::drawRectangle(int x1, int y1, int x2, int y2, color c, pen p) {
    drawTask task;
    task.x1 = x1;
    task.x2 = x2;
    task.y1 = y1;
    task.y2 = y2;
    task.p = p;
    task.c = c;
    task.type = rectangleD;
    w->pushTask(task);
  }

  void qtContext::drawCircle(int x1, int y1, int x2, int y2, color c, pen p) {
    drawTask task;
    task.x1 = x1;
    task.x2 = x2;
    task.y1 = y1;
    task.y2 = y2;
    task.p = p;
    task.c = c;
    task.type = circleD;
    w->pushTask(task);
  }

  void qtContext::drawText(int x, int y, std::string t, font f, color c) {
    drawTask task;
    task.x1 = x;
    task.y1 = y;
    task.f = f;
    task.c = c;
    pen p;
    p.thickness = 1;
    p.col = c;
    task.p = p;
    task.text = t;
    task.type = textD;
    w->pushTask(task);
  }

  void qtContext::setPixel(int x, int y, color c) {
    drawTask task;
    task.x1 = x;
    task.y1 = y;
    pen p;
    p.thickness = 1;
    p.col = c;
    task.p = p;
    task.c = c;
    task.type = pointD;
    w->pushTask(task);
  }


  qtDisplay::qtDisplay(QApplication* app, renderer* r, int width, int height) : display(r) {
    a = app;
    w = new mainWindow(this, rend);
    w->resize(width, height);
    w->show();
    cont = new qtContext(w, width, height);
  }

  qtDisplay::~qtDisplay() {
  }

  void qtDisplay::start() {
    a->exec();
  }

  context* qtDisplay::getContext() {
    return cont;
  }

  display* initQt(int argc, char **argv, renderer* rend) {
    qapp = new QApplication(argc, argv);
    return (display*)new qtDisplay(qapp, rend, 800, 600);
  }

}

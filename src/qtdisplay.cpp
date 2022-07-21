#include <QApplication>
#include <QMainWindow>
#include <QWindow>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QCheckBox>
#include <QSignalMapper>
#include <queue>
#include <vector>
#include <iostream>

#include "display.hpp"
#include "controls.hpp"
#include "event.hpp"

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
  class qtMainWindow;
  class qtContext;
  class qtControlWindow;

  class qtMainWindow : public QMainWindow {
    Q_OBJECT
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
    qtMainWindow(qtDisplay* client, renderer* r);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
  public slots:
    void nrender();
  };

  class qtContext : public context {
  protected:
    qtMainWindow* w;
  public:
    qtContext(qtMainWindow* win, int wid, int hei);
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
    qtMainWindow* w;
    qtControlWindow* cw;
    qtContext* cont;
  protected:
    void event(mg::event);
    friend class qtMainWindow;
  public:
    qtDisplay(QApplication* app, renderer* r, eventHandler* e, controls* c, int width, int height);
    ~qtDisplay();
    void start();
    context* getContext();
  };

  class qtControlWindow : public QWidget {
    Q_OBJECT
    controls* c;
    // QSignalMapper* mapper;
    std::vector<unsigned int> linking;
    std::vector<QCheckBox*> checkboxes;
  public:
    qtControlWindow(controls* con);
  public slots:
    void change(int);
  };

  qtControlWindow::qtControlWindow(controls* con) : QWidget() {
    // mapper = new QSignalMapper(this);
    c = con;
    this->setFixedSize(QSize(400, 300));

    for (int i = 0; i < c->getControlsCount(); i++) {
      control q = c->getControl(i);
      if(q.type == control::boolT) {
        linking.push_back((1 << 24) + checkboxes.size());
        QCheckBox* ch = new QCheckBox(this);
        ch->setGeometry(0, 30*i, 400, 30);
        ch->setText(QString::fromStdString(q.userDescription));
        connect(ch, &QCheckBox::stateChanged, [this, i] {change(i);} );
        checkboxes.push_back(ch);
      } else linking.push_back(0);
    }




  }

  void qtControlWindow::change(int i) {
    int type = (linking[i] & 0xff000000) >> 24;
    if(type == 1) {
      c->setValue(i, (bool)(checkboxes[linking[i] & 0x00ffffff]->checkState()));
    }
  }

  qtMainWindow::qtMainWindow(qtDisplay* client, renderer* r) {
    actualClient = client;
    rend = r;
    setMouseTracking(true);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nrender()));
    timer->start(1000/60);
    forceUpdate = false;
  }

  void qtMainWindow::nrender() {
    if(forceUpdate || rend->check()) repaint();
  }

  void qtMainWindow::paintEvent(QPaintEvent *event) {
    rend->update(actualClient);

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

  qtContext::qtContext(qtMainWindow* win, int wid, int hei) {
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


  qtDisplay::qtDisplay(QApplication* app, renderer* r, eventHandler* ev, controls* con, int width, int height) : display(r, ev) {
    a = app;
    w = new qtMainWindow(this, rend);
    cont = new qtContext(w, width, height);
    cw = new qtControlWindow(con);
    w->resize(width, height);
    w->show();
    cw->show();
  }

  qtDisplay::~qtDisplay() {
  }

  void qtDisplay::start() {
  }

  context* qtDisplay::getContext() {
    return cont;
  }

  void qtDisplay::event(mg::event e) {
    ev->handle(e);
  }

  void qtMainWindow::mousePressEvent(QMouseEvent *ev) {
    int btn;
    if(ev->button() & Qt::LeftButton) btn = 1;
    else if(ev->button() & Qt::RightButton) btn = 2;
    else if(ev->button() & Qt::MiddleButton) btn = 3;
    else btn = 0;
    mg::event e = {.type = mousePressed, .x=ev->x(), .y=ev->y(), .button=btn};
    actualClient->event(e);
  }
  void qtMainWindow::mouseReleaseEvent(QMouseEvent *ev) {
    int btn;
    if(ev->button() & Qt::LeftButton) btn = 1;
    else if(ev->button() & Qt::RightButton) btn = 2;
    else if(ev->button() & Qt::MiddleButton) btn = 3;
    else btn = 0;
    mg::event e = {.type = mouseReleased, .x=ev->x(), .y=ev->y(), .button=btn};
    actualClient->event(e);
  }
  void qtMainWindow::mouseMoveEvent(QMouseEvent *ev) {
    mg::event e = {.type = mouseMoved, .x=ev->x(), .y=ev->y()};
    actualClient->event(e);
  }
  void qtMainWindow::wheelEvent(QWheelEvent *ev) {
    mg::event e = {.type = wheelRotated, .x=(int)ev->position().x(), .y=(int)ev->position().y(), .dx=ev->angleDelta().x(), .dy=ev->angleDelta().y()};
    actualClient->event(e);
  }
  void qtMainWindow::resizeEvent(QResizeEvent *ev) {
    actualClient->getContext()->setWidth(ev->size().width());
    actualClient->getContext()->setHeight(ev->size().height());
    // mg::event e = {.type = windowResized, .x=ev->size().width(), .y=ev->size().height()};
    // actualClient->event(e);
  }

  void qtMainWindow::keyPressEvent(QKeyEvent *ev) {
    mg::event e = {.type = keyPressed, .button=ev->key()};
    actualClient->event(e);
  }
  void qtMainWindow::keyReleaseEvent(QKeyEvent *ev) {
    mg::event e = {.type = keyPressed, .button=ev->key()};
    actualClient->event(e);
  }

  display* initQt(QApplication* app, renderer* rend, eventHandler* ev, controls* con) {
    return (display*)new qtDisplay(app, rend, ev, con, 800, 600);
  }

}

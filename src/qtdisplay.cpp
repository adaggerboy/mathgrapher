#include <QApplication>
#include <QMainWindow>
#include <QWindow>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QCheckBox>
#include <QSignalMapper>
#include <QTextEdit>
#include <QSlider>
#include <QPushButton>
#include <QIcon>
#include <QLabel>
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
    int fps_counter, upd_counter;
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
    void closeEvent(QCloseEvent *event);
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
    void toggleControls();
    void close();
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
    std::vector<unsigned int> linking;
    std::vector<QCheckBox*> checkboxes;
    std::vector<QTextEdit*> textboxes;
    std::vector<QSlider*> sliders;
    std::vector<QPushButton*> buttons;
    std::vector<QLabel*> labels;
    QColor defTextEdit;
  public:
    qtControlWindow(controls* con);
  public slots:
    void change(int);
  };

  qtControlWindow::qtControlWindow(controls* con) : QWidget() {
    c = con;
    this->setFixedSize(QSize(400, 390));

    for (int i = 0; i < c->getControlsCount(); i++) {
      control q = c->getControl(i);
      if(q.type == control::boolT) {
        linking.push_back((1 << 24) + checkboxes.size());
        QCheckBox* ch = new QCheckBox(this);
        ch->setGeometry(0, 30*i, 400, 30);
        ch->setText(QString::fromStdString(q.userDescription));
        if(c->getBoolValue(i)) ch->setCheckState(Qt::Checked);
        connect(ch, &QCheckBox::stateChanged, [this, i] {change(i);} );
        checkboxes.push_back(ch);
      } else if (q.type == control::stringT) {
        linking.push_back((2 << 24) + textboxes.size());
        QTextEdit* ch = new QTextEdit(this);
        int len = q.userDescription.length() * 7;
        QLabel* lb = new QLabel(this);
        lb->setText(QString::fromStdString(q.userDescription));
        lb->setGeometry(3, 30*i, len, 30);
        labels.push_back(lb);
        ch->setGeometry(len, 30*i, 400 - len, 30);
        ch->setText(QString::fromStdString(c->getStringValue(i)));
        defTextEdit = ch->textBackgroundColor();
        connect(ch, &QTextEdit::textChanged, [this, i] {change(i);} );
        textboxes.push_back(ch);
      } else if (q.type == control::doubleT) {
        if (q.lowLimit >= q.highLimit) {
          linking.push_back((3 << 24) + textboxes.size());
          QTextEdit* ch = new QTextEdit(this);
          int len = q.userDescription.length() * 7;
          QLabel* lb = new QLabel(this);
          lb->setText(QString::fromStdString(q.userDescription));
          lb->setGeometry(3, 30*i, len, 30);
          labels.push_back(lb);
          ch->setGeometry(len, 30*i, 400 - len, 30);
          ch->setText(QString::fromStdString(std::to_string(c->getDoubleValue(i))));
          ch->setAutoFillBackground(true);
          defTextEdit = ch->textBackgroundColor();
          connect(ch, &QTextEdit::textChanged, [this, i] {change(i);} );
          textboxes.push_back(ch);
        } else {
          linking.push_back((4 << 24) + sliders.size());
          QSlider* ch = new QSlider(Qt::Horizontal, this);
          int len = q.userDescription.length() * 7;
          QLabel* lb = new QLabel(this);
          lb->setText(QString::fromStdString(q.userDescription));
          lb->setGeometry(3, 30*i, len, 30);
          labels.push_back(lb);
          ch->setGeometry(len, 30*i + 5, 400 - len, 25);
          ch->setRange((int)(q.lowLimit * 100), (int)(q.highLimit * 100));
          ch->setValue((int)(c->getDoubleValue(i) * 100));
          ch->setTracking(true);
          connect(ch, &QSlider::valueChanged, [this, i] {change(i);} );
          sliders.push_back(ch);
        }
      } else if (q.type == control::intT) {
        if (q.lowLimit >= q.highLimit) {
          linking.push_back((5 << 24) + textboxes.size());
          QTextEdit* ch = new QTextEdit(this);
          int len = q.userDescription.length() * 7;
          QLabel* lb = new QLabel(this);
          lb->setText(QString::fromStdString(q.userDescription));
          lb->setGeometry(3, 30*i, len, 30);
          labels.push_back(lb);
          ch->setGeometry(len, 30*i, 400 - len, 30);
          ch->setText(QString::fromStdString(std::to_string(c->getIntValue(i))));
          defTextEdit = ch->textBackgroundColor();
          connect(ch, &QTextEdit::textChanged, [this, i] {change(i);} );
          textboxes.push_back(ch);
        } else {
          linking.push_back((6 << 24) + sliders.size());
          QSlider* ch = new QSlider(Qt::Horizontal, this);
          int len = q.userDescription.length() * 7;
          QLabel* lb = new QLabel(this);
          lb->setText(QString::fromStdString(q.userDescription));
          lb->setGeometry(3, 30*i, len, 30);
          labels.push_back(lb);
          ch->setGeometry(len, 30*i + 5, 400 - len, 25);
          ch->setRange((int)(q.lowLimit), (int)(q.highLimit));
          ch->setValue((int)(c->getDoubleValue(i)));
          ch->setTracking(true);
          connect(ch, &QSlider::valueChanged, [this, i] {change(i);} );
          sliders.push_back(ch);
        }
      } else if(q.type == control::signalT) {
        linking.push_back((7 << 24) + buttons.size());
        QPushButton* ch = new QPushButton(this);
        ch->setGeometry(0, 30*i, 400, 30);
        ch->setText(QString::fromStdString(q.userDescription));
        connect(ch, &QPushButton::clicked, [this, i] {change(i);} );
        buttons.push_back(ch);
      } else linking.push_back(0);
    }
  }

  void qtControlWindow::change(int i) {
    int type = (linking[i] & 0xff000000) >> 24;
    if(type == 1) {
      c->setValue(i, (bool)(checkboxes[linking[i] & 0x00ffffff]->checkState()));
    } else if (type == 2) {
      if(c->setValue(i, textboxes[linking[i] & 0x00ffffff]->toPlainText().toStdString())) textboxes[linking[i] & 0x00ffffff]->setStyleSheet("");
      else textboxes[linking[i] & 0x00ffffff]->setStyleSheet("QTextEdit { background-color: rgb(255, 100, 100); }");
    } else if (type == 3) {
      try {
        if(c->setValue(i, std::stod(textboxes[linking[i] & 0x00ffffff]->toPlainText().toStdString()))) textboxes[linking[i] & 0x00ffffff]->setStyleSheet("");
        else textboxes[linking[i] & 0x00ffffff]->setStyleSheet("QTextEdit { background-color: rgb(255, 100, 100); }");
      } catch (std::exception e) {
        textboxes[linking[i] & 0x00ffffff]->setStyleSheet("QTextEdit { background-color: rgb(255, 100, 100); }");
      }
    } else if (type == 4) {
      c->setValue(i, (double)sliders[linking[i] & 0x00ffffff]->value() / 100.);
    } else if (type == 5) {
      try {
        if(c->setValue(i, (int)std::stoi(textboxes[linking[i] & 0x00ffffff]->toPlainText().toStdString()))) textboxes[linking[i] & 0x00ffffff]->setStyleSheet("");
        else textboxes[linking[i] & 0x00ffffff]->setStyleSheet("QTextEdit { background-color: rgb(255, 100, 100); }");
      } catch (std::exception e) {
        textboxes[linking[i] & 0x00ffffff]->setTextBackgroundColor(Qt::red);
      }
    } else if (type == 6) {
      c->setValue(i, sliders[linking[i] & 0x00ffffff]->value());
    } else if (type == 7) {
      c->sendSignal(i);
    }
  }

  qtMainWindow::qtMainWindow(qtDisplay* client, renderer* r) {
    actualClient = client;
    rend = r;
    setMouseTracking(true);
    fps_counter = upd_counter = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nrender()));
    timer->start(1000/60);
    forceUpdate = false;
  }

  void qtMainWindow::nrender() {
    if(upd_counter == 60) {
      rend->getDebugStruct()->fps = fps_counter;
      fps_counter = upd_counter = 0;
    }
    if(forceUpdate || rend->check()) repaint();
    upd_counter++;
  }

  void qtMainWindow::paintEvent(QPaintEvent *event) {
    fps_counter++;
    rend->update(actualClient);
    perfDebugStruct* dbg = rend->getDebugStruct();
    dbg->tpf = 0;

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
          paint->drawText(t.x1, t.y1 + t.f.size, QString::fromStdString(t.text));
        break;
        case pointD:
          paint->setPen(genQPen(t.p));
          paint->drawPoint(t.x1, t.y1);
        break;
      }
      dbg->tpf++;
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
    w->setCursor(Qt::BlankCursor);
    w->setWindowTitle("MathGrapher v0.1");
    w->setWindowIcon(QIcon("res/icon.png"));
    cw->setWindowTitle("Grapher controls");
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

  void qtDisplay::toggleControls() {
    if(cw->isVisible()) cw->close();
    else cw->show();
  }

  void qtDisplay::close() {
    a->quit();
  }

  void qtMainWindow::mousePressEvent(QMouseEvent *ev) {
    int btn;
    if(ev->button() & Qt::LeftButton) btn = 1;
    else if(ev->button() & Qt::RightButton) btn = 2;
    else if(ev->button() & Qt::MiddleButton) btn = 3;
    else btn = 0;
    mg::event e = {.type = mg::event::mousePressed, .x=ev->x(), .y=ev->y(), .button=btn};
    actualClient->event(e);
  }
  void qtMainWindow::mouseReleaseEvent(QMouseEvent *ev) {
    int btn;
    if(ev->button() & Qt::LeftButton) btn = 1;
    else if(ev->button() & Qt::RightButton) btn = 2;
    else if(ev->button() & Qt::MiddleButton) btn = 3;
    else btn = 0;
    mg::event e = {.type = mg::event::mouseReleased, .x=ev->x(), .y=ev->y(), .button=btn};
    actualClient->event(e);
  }
  void qtMainWindow::mouseMoveEvent(QMouseEvent *ev) {
    mg::event e = {.type = mg::event::mouseMoved, .x=ev->x(), .y=ev->y()};
    actualClient->event(e);
  }
  void qtMainWindow::wheelEvent(QWheelEvent *ev) {
    mg::event e = {.type = mg::event::wheelRotated, .x=(int)ev->position().x(), .y=(int)ev->position().y(), .dx=ev->angleDelta().x(), .dy=ev->angleDelta().y()};
    actualClient->event(e);
  }
  void qtMainWindow::resizeEvent(QResizeEvent *ev) {
    actualClient->getContext()->setWidth(ev->size().width());
    actualClient->getContext()->setHeight(ev->size().height());
    mg::event e = {.type = mg::event::windowResized, .x=ev->size().width(), .y=ev->size().height()};
    actualClient->event(e);
  }

  void qtMainWindow::keyPressEvent(QKeyEvent *ev) {
    if(ev->key() == Qt::Key_E) actualClient->toggleControls();
    mg::event e = {.type = mg::event::keyPressed, .button=ev->key()};
    actualClient->event(e);
  }
  void qtMainWindow::keyReleaseEvent(QKeyEvent *ev) {
    mg::event e = {.type = mg::event::keyReleased, .button=ev->key()};
    actualClient->event(e);
  }

  void qtMainWindow::closeEvent(QCloseEvent *ev) {
    mg::event e = {.type = mg::event::closed};
    actualClient->close();
  }

  display* initQt(QApplication* app, renderer* rend, eventHandler* ev, controls* con) {
    return (display*)new qtDisplay(app, rend, ev, con, 800, 600);
  }

}

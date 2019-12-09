## Qt5新的语法

### Qt4问题
  没有编译期检查：因为函数名被处理成字符串，所有的检查都是在运行时完成的。这就是为什么有时会发生编译通过了，但 slot 并没有被调用。
此时，你就应该去检查 console 的输出，看看有没有什么 warning 说明 connect 并没有成功。
因为处理的是字符串，所以 slot 中的类型名字必须用 signal 的完全一致，而且在头文件中的和实际 connect 语句中的也必须一致。
也就是说，如果你使用了 typedef 或者 namespace，connect 就可能不成功（在 Qt 5 之前的版本中，我们当然也可以使用 namespace，
但是必须保证头文件中的和 connect 语句中的文本完全一致）。

### 优点
```c++
QObject::connect(&a, &Counter::valueChanged,
                 &b, &Counter::setValue);
```
**使用函数指针**

###### 编译器检查

如果把 signal 或者 slot 名字编写错误，或者 slot 的参数同 signal 不一致，你会在编译期就获得一个错误。这肯定会在重构、或者修改 signal 或 slot 的名字时节省很多时间。

另一个影响是，Qt 可以利用static_cast返回更友好的错误信息。

###### 参数的自动类型转换

```c++
class Test : public QObject
{
    Q_OBJECT
public:
    Test() {
        connect(this, &Test::someSignal, this, &Test::someSlot);
    }
signals:
    void someSignal(const QString &);
public:
    void someSlot(const QVariant &);
};
```
###### 连接到任意函数

Qt 的新语法通过函数指针直接调用函数，而不需要 moc 的特殊处理（但是 signal 依然需要）


```c++
static void someFunction() {
    qDebug() << "pressed";
}

// ... somewhere else
QObject::connect(button, &QPushButton::clicked, someFunction);
```

###### C++11 lambda 表达式

```c++
void MyWindow::saveDocumentAs() {
    QFileDialog *dlg = new QFileDialog();
    dlg->open();
    QObject::connect(dlg, &QDialog::finished, [=](int result) {
        if (result) {
            QFile file(dlg->selectedFiles().first());
            // ... save document here ...
        }
        dlg->deleteLater();
    });
}
```
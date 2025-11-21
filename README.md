# Qt Embed CEF Demo

## References

- 一些关于Linux端嵌入cef的内容
  - https://forum.qt.io/topic/74647/qt-cef-integration-on-linux/18
    
    在这个网页中提到：*Cef is built on Gtk2, so it requires gtk handle, not the winId().*
    
    所以在`统信UOS`下采用了`QWidget::createWindowContainer`的方式嵌入cef。

    **为什么麒麟kylin OS下又可以使用`winId`的方式？**

  - https://cloud.tencent.com/developer/ask/sof/109849415

    这个网页里也提到使用`QWidget::createWindowContainer`，但是还说了可以通过`QBackingStore`处理创建`QWindow`时背景颜色。

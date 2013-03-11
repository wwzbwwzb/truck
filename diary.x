
2013-03-04
	1.  看看mesa的swapbuffer实现，为何swap的适合把singlesample miptree 当作目标了，什么是ssmt
	2.  dir2 看上去是一个不断改啊改出来的东西，缺乏系统性。 Xfree上的文档大多都被删掉了，还是看看Xserver的实现比较有参考意义
    hw/xfree86/dri2.c
    确是dri2_get_buffers的适合才分配attachment对应的buffer

  3. 是否有调试 Xserver的方法
        xf86DrvMsg(pScreen->myNum, X_ERROR,

    if (pSrcBuffer == NULL || pDestBuffer == NULL) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[DRI2] %s: drawable has no back or front?\n", __func__);
        return BadDrawable;

        如过前后两个buffer有不存在的，认为是baddrawable， 为什么我的demo 总是赵不到frontbuffer， get_buffers都不分配。

	4. xserver 似乎不会为每个window创建buffer，难道窗口的内容都是通过渲染函数渲染出来的吗？ 合成的过程就不是简单的窗口合成，而是很多窗口才真正绘制出来。
? 是这样的马?
2013-03-09
  : 经过交流，发现的确没有为每个窗口创建buffer， 所有窗口都绘制到root window的buffer里面


	5.  xcb 创建窗口并监听事件 
	  a. 窗口没有FrontBuffer，没有Backbuffer，直接绘制到RootWindow上的。
	  b. 并不是只有一个进程可以访问窗口的事件，所有map窗口的进程和创建窗口进程都可以获得事件
	     :( 实际情况，创建窗口进程最好不接收事件，另一个进程才能正常偷听到全部事件


==============================
6. xcb_dri2_create_drawable 并不创建实际的buffer，知道dri2_get_buffers才进行分配
  create_drawable 可以多次调用，必须在调用get_buffers之前调用。

==============================
==============================
==============================



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







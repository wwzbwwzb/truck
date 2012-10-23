
# 设计预想： 不使用cgi+server方式，直接通过静态页面和服务器完成本地应用。
use nginx + webpy
(static files[js,css,html]) + (dynamic, session,db,plugin)

2. 减少代码尺寸，提供可读性一致性， MVC分离
3. 后端更灵活。
4. 前端（客户）完成大部分的逻辑，布局等。


client ----1. ngix static pages
       ----2. python sso, 
			 	python 并发性，线程模型都不适合并发计算。那么其实任何语言都可以保留http接口,之需要一个公共接口。
       ----3.             xmpp(prosody.im)

python realize both the json & TCP parts

#相信自己，可以做的更好
1. IT projects sites
2. site CEOS howto
3. DVR=xbmc,vlc,tv.v4l, P4P, xmpp, statusnet.
4. 

==++++++++++++++++++++++
git clone git://github.com/webpy/webpy.git


============
先做一个小小的演示：
1. 通过 javascript + HTML 来实现从服务器读取数据库中用户和ID
需要：
	1. javascript json  xmlhttprequest(认证，加密，)
	cookie,session+IP,client.etc,
	2. 加密和认证。

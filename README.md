# 1 背景
目前市面上，原生支持H265播放的浏览量js控件，都有共同痛点：延时比较大，流媒体服务器端依赖资源比较多，都不能够移植和应用到嵌入式设备例如：ipc、nvr等，而tinywebh265就是解决流媒体服务器不好移植、flash空间比较小，很好应用在对实时图像要求比较高领域；

# 2 优点
tinywebh265对比现有有点如下：
1、视频支持h264、h265，音频支持g711a g711u;
2、流媒体服务器占用资源小（依赖库小于80K>;
3、生成js文件可以部署到云端;
4、流媒体服务器采用websocket server方式， 所以整个流延时比较小，实时性好;(小于150ms);

# 3 代码
github地址: [https://github.com/tinywebh265/tinywebh265]

# 4 demo 演示
1、开启 websocket server 流媒体端：
git clone https://github.com/tinywebh265/tinywebh265.git;
cd tinywebh265/ws_server/demo;
make;
./streamapp

2、开启http server（采用易移植thttpd）:
cd tinywebh265/http_server/www;
打开index.html 替换ws://192.168.1.125:8536 中ip 变成刚刚开启服务ip 例如：ws://192.168.1.65:8536;
cd tinywebh265/http_server;
./run.sh 开启http 服务;

3、web 访问 http 服务ip:8090,就可以看到视频;

# 5 联系
    邮箱:tinywebh265@163.com; 微信:tinywebh265

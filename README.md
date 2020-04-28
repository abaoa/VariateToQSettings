# VariateToQSettings
Qt编写的一款由变量快速生成 QSettings 代码的应用
1. 在实际开发中需要使用QSetting 保存配置文件的频率特别多。在敲写了一堆 settings.setValue("key",value); 之后，总要，再敲一堆 value = settings.value("key",value).value<type>();  往往感到无趣，所以是时候改变一下工作的方式了。
     首先，QSettings 保存的多数是变量的值，举个栗子：
	 
声明了以下变量：
```cpp
QString mName; //名字
ushort mAge; //年龄
ushort mA,mB,mC; //三围
ushort mHeight; //身高
int mTemp;
float mXxx;
double mBug;
```
那么我们可以对这样的变量声明进行解析，让他快速生成QSettings 的setValue 和value 语句。
```cpp
auto path = QString("");
QSettings settings(path);
settings.setIniCodec("UTF-8");
settings.beginGroup("config");
settings.setValue("Name",mName); //名字
settings.setValue("Age",mAge); //年龄
settings.setValue("A",mA); //三围
settings.setValue("B",mB); //三围
settings.setValue("C",mC); //三围
settings.setValue("Height",mHeight); //身高
settings.setValue("Temp",mTemp);
settings.setValue("Xxx",mXxx);
settings.setValue("Bug",mBug);
settings.endGroup();
```

```cpp
auto path = QString("");
QSettings settings(path);
settings.setIniCodec("UTF-8");
settings.beginGroup("config");
mName = settings.value("Name",mName).value<QString>(); //名字
mAge = settings.value("Age",mAge).value<ushort>(); //年龄
mA = settings.value("A",mA).value<ushort>(); //三围
mB = settings.value("B",mB).value<ushort>(); //三围
mC = settings.value("C",mC).value<ushort>(); //三围
mHeight = settings.value("Height",mHeight).value<ushort>(); //身高
mTemp = settings.value("Temp",mTemp).value<int>();
mXxx = settings.value("Xxx",mXxx).value<float>();
mBug = settings.value("Bug",mBug).value<double>();
settings.endGroup();
```

大概的界面就是这样的；
![](https://github.com/bao-boyle/VariateToQSettings/raw/master/screenshot/1.png) 

支持基本的数据类型，还支持结构体，复杂的未考虑。
![](https://github.com/bao-boyle/VariateToQSettings/raw/master/screenshot/2.png) 

Gif演示
![](https://github.com/bao-boyle/VariateToQSettings/raw/master/screenshot/3.gif) 

我们只需要复制变量的声明代码，就可以一键生成QSettings 的setValue和value 的代码
是不是很方便呢？欢迎大家使用和优化。


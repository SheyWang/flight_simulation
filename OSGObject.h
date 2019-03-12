#pragma once

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
//#include <osgEarthUtil/Sky>
//#include <osgEarthAnnotation/PlaceNode>
#include <iostream>
#include <string>
#include <Windows.h>
#include <osgEarthUtil/Controls>
#include <osgEarth/ModelLayer>
#include <osg/AnimationPath>
#include <osgParticle/FireEffect>
#include "LabelControlEventHandler.h"

using namespace std;
//#include <osgEarth/GeoData>

class COSGObject
{
public:
	COSGObject(HWND hWnd);
	~COSGObject(void);

	void InitOSG();
	void InitSceneGraph();
	void InitCameraConfig();
	void PreFrameUpdate();
	void PostFrameUpdate();
	static void Render(void * ptr);

	void InitOsgEarth();

	osgViewer::Viewer *getViewer();

	void setChinaBoundariesOpacity(double opt);
	double getChinaBoundariesOpacity();

	void rmvChinaBoundaryes();
	void addChinaBoundaryes();




	//新增地标
	void addLabel();

	//ptr
	//1：指针：COSGObject
	//2：指针：文件名
	//3：整型：地标数量
	static void ReadLabelThread(void * ptr);

	osg::Image* sheng;
	osg::Image* cityCenter;
	osg::Image* city;
	osg::Image* countycity;
	osg::Image* county;
	osg::Image* town;
	osg::Image* vi;
	osgEarth::Style style;

	unsigned int shaanxiParam[3];
	std::string shaanxiTxt;

	unsigned int jiangxiParam[3];
	std::string jiangxiTxt;

	unsigned int shengParam[3];
	std::string shengTxt;



	//编码转换函数1
	void unicodeToUTF8(const wstring &src, string& result);

	//编码转换函数2
	void gb2312ToUnicode(const string& src, wstring& result);

	//编码转换函数3
	void gb2312ToUtf8(const string& src, string& result);

	//添加显示视点信息的控件
	void addViewPointLabel();
	CLabelControlEventHandler *labelEvent;

public:
	HWND m_hWnd;
	osgViewer::Viewer * mViewer;
	osg::Group* mRoot;
	//osg::ref_ptr<osg::Group> mRoot;
	osgEarth::MapNode* mapNode;
	//osg::ref_ptr<osgEarth::MapNode> mapNode;
	osgEarth::Util::EarthManipulator* em;
	//osg::ref_ptr<osgEarth::Util::EarthManipulator> em;

	//国界线图层
	osg::ref_ptr<osgEarth::ModelLayer> china_boundaries;



	osgEarth::Util::SkyNode* sky_node;  //申请一个SkyNode

	//地标
	osg::ref_ptr<osg::Group> earthLabel;

//地表的矩阵操作
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;

public:
	osg::ref_ptr<osg::Node> airport;
	osg::ref_ptr<osg::MatrixTransform> mtAirport;
	//设置机场
	void addAirport();

	osg::ref_ptr<osg::Node> flyAirport;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtfly;

	

public:
	//根据输入的控制点，输出一个路径，控制点格式为（经，纬，高，速）
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);

	//两点间的距离公式
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//计算两点间需要行驶的时间
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//设置一个预设置路径
	void DoAPreLine();

	//生成的预设置路径
	osg::ref_ptr<osg::AnimationPath> apc;

	//启动预设置路径
	void DoPreLineNow();
	void isTrackFly(bool btrack);

	//Build尾迹
	void BuildTail(osg::Vec3 position, osg::MatrixTransform* scaler);

	//Buildribbon
	void BuildRibbon(int size, osg::MatrixTransform* scalar);

//地表的矩阵操作
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn_nchu;


public:
	osg::ref_ptr<osg::Node> buildings;
	osg::ref_ptr<osg::MatrixTransform> mtBuildingsSelf;
	osg::ref_ptr<osg::MatrixTransform> mtbuildings;
	//设置南昌航空大学
	void addNchu();

	

};


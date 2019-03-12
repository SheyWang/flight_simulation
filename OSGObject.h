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




	//�����ر�
	void addLabel();

	//ptr
	//1��ָ�룺COSGObject
	//2��ָ�룺�ļ���
	//3�����ͣ��ر�����
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



	//����ת������1
	void unicodeToUTF8(const wstring &src, string& result);

	//����ת������2
	void gb2312ToUnicode(const string& src, wstring& result);

	//����ת������3
	void gb2312ToUtf8(const string& src, string& result);

	//�����ʾ�ӵ���Ϣ�Ŀؼ�
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

	//������ͼ��
	osg::ref_ptr<osgEarth::ModelLayer> china_boundaries;



	osgEarth::Util::SkyNode* sky_node;  //����һ��SkyNode

	//�ر�
	osg::ref_ptr<osg::Group> earthLabel;

//�ر�ľ������
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;

public:
	osg::ref_ptr<osg::Node> airport;
	osg::ref_ptr<osg::MatrixTransform> mtAirport;
	//���û���
	void addAirport();

	osg::ref_ptr<osg::Node> flyAirport;
	osg::ref_ptr<osg::MatrixTransform> mtFlySelf;
	osg::ref_ptr<osg::MatrixTransform> mtfly;

	

public:
	//��������Ŀ��Ƶ㣬���һ��·�������Ƶ��ʽΪ������γ���ߣ��٣�
	osg::AnimationPath* CreateAirLinePath(osg::Vec4Array* ctrl);

	//�����ľ��빫ʽ
	double GetDis(osg::Vec3 from, osg::Vec3 to);

	//�����������Ҫ��ʻ��ʱ��
	double GetRunTime(osg::Vec3 from, osg::Vec3 to, double speed);

	//����һ��Ԥ����·��
	void DoAPreLine();

	//���ɵ�Ԥ����·��
	osg::ref_ptr<osg::AnimationPath> apc;

	//����Ԥ����·��
	void DoPreLineNow();
	void isTrackFly(bool btrack);

	//Buildβ��
	void BuildTail(osg::Vec3 position, osg::MatrixTransform* scaler);

	//Buildribbon
	void BuildRibbon(int size, osg::MatrixTransform* scalar);

//�ر�ľ������
public:
	osg::ref_ptr<osg::CoordinateSystemNode> csn_nchu;


public:
	osg::ref_ptr<osg::Node> buildings;
	osg::ref_ptr<osg::MatrixTransform> mtBuildingsSelf;
	osg::ref_ptr<osg::MatrixTransform> mtbuildings;
	//�����ϲ����մ�ѧ
	void addNchu();

	

};

